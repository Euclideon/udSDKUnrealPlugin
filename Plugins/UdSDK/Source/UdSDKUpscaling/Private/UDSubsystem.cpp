#include "UDSubsystem.h"
#include "Runtime/RHI/Public/RHI.h"
#include "ImageUtils.h"
#include "Slate/SceneViewport.h"
#include "Engine/GameViewportClient.h"
#include "ObjectStorageSettings.h"
#include "UdSDKCompositeViewExtension.h"
#include "UdSDKDefine.h"
#include "udContext.h"

template <typename ValueType>
void ResizeArray(TArray<ValueType>& Array, int32 Size)
{
	Array.Empty(Size);
	Array.AddUninitialized(Size);
}

uint32_t vcVoxelShader_Black(udPointCloud* /*pPointCloud*/, const udVoxelID* /*pVoxelID*/, const void* pUserData)
{
	FUdAsset* pData = (FUdAsset*)pUserData;

	return 0x00000000;
}

uint32_t vcVoxelShader_Colour(udPointCloud* pPointCloud, const udVoxelID* pVoxelID, const void* pUserData)
{
	uint32_t color = 0;
	udPointCloud_GetNodeColour(pPointCloud, pVoxelID, &color);

	return (0xffffff & color);
}

void FuncMat2Array(double* array, const FMatrix& Mat)
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			array[i * 4 + j] = Mat.M[i][j];
};

UUDSubsystem::UUDSubsystem()
{
	Width = 0;
	Height = 0;
	LoginFlag = false;
	ViewExtension = nullptr;
}

UUDSubsystem::~UUDSubsystem()
{
	//TODO: Cleanup properly
	if (IsLogin())
		Exit();
}

void UUDSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOG(LogTemp, Display, TEXT("UnlimitedDetail | INIT SUBSYSTEM"));
	LoginFunction();
}

void UUDSubsystem::Deinitialize()
{
	UE_LOG(LogTemp, Display, TEXT("UnlimitedDetail | DEINIT SUBSYSTEM"));
	Exit();
}

// Does NOT init the udContext
int UUDSubsystem::Init()
{
	enum udError error = udE_Success;

	if (const UObjectStorageSettings* Settings = GetDefault<UObjectStorageSettings>())
	{
		ServerUrl = Settings->ServerPath.ToString();
		APIKey = Settings->Password.ToString();
		if (ServerUrl.IsEmpty() || APIKey.IsEmpty())
			error = udE_Failure;
	}
	else
	{
		ServerUrl = "";
		APIKey = "";
		error = udE_Failure;
	}

	return error;
}

// Called as a result of clicking Login() from the widget
int UUDSubsystem::LoginFunction()
{
	// Logging startup values at login
	UE_LOG(LogTemp, Display, TEXT("Auth values at start of Login() function"));
	UE_LOG(LogTemp, Display, TEXT("Server: %s"), *ServerUrl);

	// Get an error ready and default to failure
	enum udError error = udE_Failure;

	// Unsure what happens to login reqs
	if (LoginFlag)
	{
		UDSDK_WARNING_MSG("Have login!");
		return error;
	}

	// Initialize login related data
	// Will fail if logins are not entered into the UI etc
	error = (udError)Init(); // Maybe fix this cast?
	
	if (error != udE_Success) // Should fail basically due to user error only
	{
		UDSDK_ERROR_MSG("Initialization failed!");
		UE_LOG(LogTemp, Warning, TEXT("Auth values after failing initialization: "));
		UE_LOG(LogTemp, Warning, TEXT("Server: %s"), *ServerUrl);
		
		UE_LOG(LogTemp, Warning, TEXT("Current State of Connect Attempt: %d "), error);
		return error;
	}
	
	{
		FScopeLock ScopeLockInst(&DataMutex);
		InstanceArray.Reset();
	}

	const FString ApplicationVersion = "0.0";
	const FString ApplicationName = "UE5_Client";

	error = udContext_ConnectWithKey(&pContext, TCHAR_TO_UTF8(*ServerUrl), TCHAR_TO_UTF8(*ApplicationName), TCHAR_TO_UTF8(*ApplicationVersion), TCHAR_TO_UTF8(*APIKey));
	if (error != udE_Success)
	{
		UDSDK_ERROR_MSG("udContext_ConnectWithKey (Error: %s)", GetError(error));
		return error;
	}

	error = udRenderContext_Create(pContext, &pRenderer);
	if (error != udE_Success)
	{
		UDSDK_ERROR_MSG("udRenderContext_Create (Error: %s)", GetError(error));
		return error;
	}

	if (!ViewExtension)
	{
		ViewExtension = FSceneViewExtensions::NewExtension<FUdSDKCompositeViewExtension>();
	}
	else
	{
		UDSDK_WARNING_MSG("The ViewExtension object already exists");
	}
	
	LoginFlag = true;

	return error;
}

int UUDSubsystem::Exit()
{
	//FScopeLock ScopeLock(&CallMutex);
	enum udError error = udE_Failure;

	ViewExtension = nullptr;

	ServerUrl = ""; // udcloud.com
	APIKey = "";

	Width = 0;
	Height = 0;

	if (LoginFlag)
	{
		LoginFlag = false;
		{
			FScopeLock ScopeLock(&DataMutex);
			for (auto inst : InstanceArray)
			{
				error = udPointCloud_Unload(&inst.pPointCloud);
				if (error != udE_Success)
				{
					UDSDK_ERROR_MSG("udPointCloud_Unload error : %s", GetError(error));
				}
			}
			InstanceArray.Reset();

			AssetsMap.Reset();
		}
		

		if (pRenderView)
		{
			error = udRenderTarget_Destroy(&pRenderView);
			if (error != udE_Success)
			{
				UDSDK_ERROR_MSG("udRenderTarget_Destroy error : %s", GetError(error));
			}
			pRenderView = nullptr;
		}

		if (pRenderer)
		{
			error = udRenderContext_Destroy(&pRenderer);
			if (error != udE_Success)
			{
				UDSDK_ERROR_MSG("udRenderContext_Destroy error : %s", GetError(error));
			}
			pRenderer = nullptr;
		}

		if (pContext)
		{
			error = udContext_Disconnect(&pContext, false);
			if (error != udE_Success)
			{
				UDSDK_ERROR_MSG("udContext_Disconnect error : %s", GetError(error));
			}
			pContext = nullptr;
		}
	}
	
	return error;
}

FUDPointCloudHandle* UUDSubsystem::Load(FString URL)
{
	enum udError error = udE_Failure;

	if (!LoginFlag)
	{
		UDSDK_ERROR_MSG("Not logged in!");
		return nullptr;
	}

	{
		FScopeLock ScopeLock(&DataMutex);
		FUDPointCloudHandle* Asset = AssetsMap.Find(URL);
		if (Asset)
		{
			return Asset;
		}
	}

	FUDPointCloudHandle Asset = {};
	udPointCloudHeader header = {};

	error = udPointCloud_Load(pContext, &Asset.PointCloud, TCHAR_TO_UTF8(*URL), &header);
	if (error != udE_Success)
	{
		UDSDK_ERROR_MSG("udPointCloud_Load error : %s %s", GetError(error), *URL);
		return nullptr;
	}

	Asset.VoxelShaderFunc = vcVoxelShader_Black;

	uint32_t attributeOffset = 0;
	if (udAttributeSet_GetOffsetOfStandardAttribute(&header.attributes, udSA_ARGB, &attributeOffset) == udE_Success)
	{
		Asset.VoxelShaderFunc = vcVoxelShader_Colour;
	}

	udDouble3 pivot = udDouble3::create(header.pivot[0], header.pivot[1], header.pivot[2]);

	Asset.Pivot.X = header.pivot[0];
	Asset.Pivot.Y = header.pivot[1];
	Asset.Pivot.Z = header.pivot[2];

	{
		FScopeLock ScopeLock(&DataMutex);
		AssetsMap.Add(URL, Asset);
	}

	return AssetsMap.Find(URL);
}

void UUDSubsystem::Remove(FUDPointCloudHandle** UDPointCloudHandlePtr)
{
	FScopeLock ScopeLock(&DataMutex);

	if (UDPointCloudHandlePtr == nullptr || *UDPointCloudHandlePtr == nullptr)
		return;

	FUDPointCloudHandle* PCI = *UDPointCloudHandlePtr;
	*UDPointCloudHandlePtr = nullptr;

	enum udError error = udE_Success;

	FUDPointCloudHandle* Asset = AssetsMap.Find(PCI->URL);

	if (Asset)
	{
		--Asset->RefCount;

		if (Asset->RefCount <= 0)
		{
			udPointCloud_Unload(&PCI->PointCloud);
			AssetsMap.Remove(PCI->URL);
		}
	}
	else
	{
		// No idea how it got here
	}
}

bool UUDSubsystem::Find(FString URL)
{
	FScopeLock ScopeLock(&DataMutex);
	FUDPointCloudHandle* Asset = AssetsMap.Find(URL);

	return (Asset != nullptr);
}

bool UUDSubsystem::QueueInstance(FUDPointCloudHandle* PCI, const FMatrix& InMatrix, const FSceneView* View)
{
	UE_LOG(LogTemp, Display, TEXT("Setting transform ..."));
	
	if (!PCI->PointCloud)
	{
		return false;
	}

	FScopeLock ScopeLock(&DataMutex);

	udRenderInstance RenderInstance = {};
	RenderInstance.pPointCloud = PCI->PointCloud;
	RenderInstance.pVoxelShader = PCI->VoxelShaderFunc;
	RenderInstance.pVoxelUserData = (void*)(View->Family);

	for (int i = 0; i < 4; ++i)
	{
		RenderInstance.matrix[0 + i * 4] = InMatrix.M[i][0];
		RenderInstance.matrix[1 + i * 4] = InMatrix.M[i][1];
		RenderInstance.matrix[2 + i * 4] = InMatrix.M[i][2];
		RenderInstance.matrix[3 + i * 4] = InMatrix.M[i][3];
	}

	InstanceArray.Push(RenderInstance);

	return true;
}

void UUDSubsystem::ResetForNextViewport(const FSceneView& View)
{
	//InstanceArray.Empty();
}

// The main function for rendering out UD images
int UUDSubsystem::CaptureUDSImage(const FSceneView& View)
{
	// prep an empty error
	enum udError error = udE_Failure;
	
	if (!LoginFlag)
	{
		return udE_Failure;
	}

	{
		FScopeLock ScopeLock(&DataMutex);
		if (InstanceArray.Num() == 0)
		{
			return udE_Failure;
		}
	}

	// These values are incorrect, but are at least visually plausable.
	// If we can however get our singleton correctly, we should use the more accurate values contained within that
	// auto vartest = View.UnconstrainedViewRect.Width();
	int32 nWidth = View.UnconstrainedViewRect.Width();
	int32 nHeight = View.UnconstrainedViewRect.Height();
	
	// Return early if we have really invalid values?
	if (nWidth <= 0 || nHeight <= 0)
	{
		UDSDK_ERROR_MSG("Error, width or height = 0 : %s", GetError(error));
		return udE_Failure;
	}

	// TODO - This needs to be handled better.
	// Hardcap the render to some reasonable number - sometimes the width/height can come out at unreasonably large numbers which signifies that this frame shouldnt render
	if (nWidth >= 8192 || nHeight >= 8192)
	{
		check(false);
		UDSDK_ERROR_MSG("Error, width or height too big : %s", GetError(error));
		return udE_Failure;
	}

	error = (udError)RecreateUDView(nWidth, nHeight, View.FOV);
	if (error != udE_Success)
	{
		UDSDK_ERROR_MSG("RecreateUDView error : %s", GetError(error));
		return error;
	}

	FuncMat2Array(ProjArray, ProjectionMatrix);
	FuncMat2Array(ViewArray, View.ViewMatrices.GetViewMatrix());

	{
		FScopeLock ScopeLockData(&BulkDataMutex);
		FScopeLock ScopeLockInst(&DataMutex);

		error = udRenderTarget_SetTargets(pRenderView, ColorBulkData.GetData(), 0xFF000000, DepthBulkData.GetData());
		if (error != udE_Success)
		{
			UDSDK_ERROR_MSG("udRenderTarget_SetTargets error : %s", GetError(error));
			return error;
		}

		error = udRenderTarget_SetMatrix(pRenderView, udRTM_Projection, ProjArray);
		error = udRenderTarget_SetMatrix(pRenderView, udRTM_View, ViewArray);
		
		if (error != udE_Success)
		{
			UDSDK_ERROR_MSG("udRenderTarget_SetMatrix error : %s", GetError(error));
			return error;
		}

		udRenderPicking picking = {};

		udRenderSettings renderOptions;
		memset(&renderOptions, 0, sizeof(udRenderSettings));
		
		renderOptions.pPick = &picking;
		renderOptions.pFilter = nullptr;
		renderOptions.pointMode = udRCPM_Rectangles;
		
		for (int i = 0; i < InstanceArray.Num(); ++i)
		{
			InstanceArray[i].skipRender = (InstanceArray[i].pVoxelUserData == View.Family ? 0 : 1);

			if (InstanceArray[i].skipRender == 0)
			{
				UDSDK_ERROR_MSG("udRenderContext_Render error : %s", GetError(error));
			}
		}

		error = udRenderContext_Render(pRenderer, pRenderView, InstanceArray.GetData(), InstanceArray.Num(), &renderOptions);
		if (error != udE_Success)
		{
			UDSDK_ERROR_MSG("udRenderContext_Render error : %s", GetError(error));
			return error;
		}

		// TODO - Add picking back in
		if (picking.hit)
		{
		//	SetSelectedByModelIndex(picking.modelIndex, true);
		}


	}


	ENQUEUE_RENDER_COMMAND(UpdateTextureData)(
		[this](FRHICommandListImmediate& CommandList) {
		FScopeLock ScopeLock(&BulkDataMutex);
		if (ColorTexture.IsValid() && ColorTexture->GetSizeX() == Width && ColorTexture->GetSizeY() == Height)
		{
			auto Region = FUpdateTextureRegion2D(0, 0, 0, 0, Width, Height);
			RHIUpdateTexture2D(ColorTexture.GetReference(), 0, Region, ColorBulkData.GetTypeSize() * Region.Width, (uint8*)ColorBulkData.GetData());

			//uint32 Stride = 0;
			//void* TextureMemory = GDynamicRHI->LockTexture2D_RenderThread(CommandList, CoefColorTexture.GetReference(), 0, RLM_WriteOnly, Stride, false);
			//if (TextureMemory)
			//{
			//	FMemory::Memcpy(TextureMemory, ColorBulkData.GetData(), srceen_width * srceen_height * ColorBulkData.GetTypeSize());
			//	GDynamicRHI->UnlockTexture2D_RenderThread(CommandList, CoefColorTexture.GetReference(), 0, true);
			//}
		}
		if (DepthTexture.IsValid() && DepthTexture->GetSizeX() == Width && DepthTexture->GetSizeY() == Height)
		{
			auto Region = FUpdateTextureRegion2D(0, 0, 0, 0, Width, Height); // check this maybe?
			RHIUpdateTexture2D(DepthTexture.GetReference(), 0, Region, DepthBulkData.GetTypeSize() * Region.Width, (uint8*)DepthBulkData.GetData());

			//uint32 Stride = 0;
			//void* TextureMemory = GDynamicRHI->LockTexture2D_RenderThread(CommandList, CoefDepthTexture.GetReference(), 0, RLM_WriteOnly, Stride, false);
			//if (TextureMemory)
			//{
			//	FMemory::Memcpy(TextureMemory, DepthBulkData.GetData(), srceen_width * srceen_height * DepthBulkData.GetTypeSize());
			//	GDynamicRHI->UnlockTexture2D_RenderThread(CommandList, CoefDepthTexture.GetReference(), 0, true);
			//}
		}
	});
	return error;
}

int UUDSubsystem::RecreateUDView(int32 InWidth, int32 InHeight, float InFOV)
{
	enum udError error = udE_Success;
	if (InWidth == Width && InHeight == Height)
	{
		return error;
	}

	Width = InWidth;
	Height = InHeight;

	UE_LOG(LogTemp, Display, TEXT("RecreateUDView() Width: %d, Height: %d"), Width, Height);
			

	const float MinZ = GNearClippingPlane;
	const float MaxZ = MinZ;
	const float ModifiedViewFOV = InFOV;
	const float MatrixFOV = FMath::Max(0.001f, ModifiedViewFOV) * (float)PI / 360.0f;

	float const XAxisMultiplier = 1.0f;
	float const YAxisMultiplier = Width / (float)Height;

	ProjectionMatrix = FPerspectiveMatrix(
		MatrixFOV,
		MatrixFOV,
		XAxisMultiplier,
		YAxisMultiplier,
		MinZ,
		MaxZ
	);

	{
		FScopeLock ScopeLock(&BulkDataMutex);
		ETextureCreateFlags TexCreateFlags = TexCreate_Dynamic; // Flags for .SetFlags()
		{

		
			ColorBulkData.ResizeArray(Width * Height);
			const FString DebugName = "RecreateUDView ColorTexture";
			
			//RHICreateTexture2D - DEPRECATED in 5.1, use RHICreateTexture(FRHITextureCreateDesc) instead
			// ColorTexture = RHICreateTexture2D(Width, Height, EPixelFormat::PF_B8G8R8A8, 1, 1, TexCreateFlags, CreateInfo); // 4.27 Line

			// FRHIResourceCreateInfo CreateInfo;
			//FRHIResourceCreateInfo

			// New 5.1 texture descriptor
			FRHITextureCreateDesc ColorTextureDescriptor = FRHITextureCreateDesc::Create2D(*DebugName, Width, Height, EPixelFormat::PF_B8G8R8A8);

			&ColorTextureDescriptor.SetFlags(TexCreateFlags);
			&ColorTextureDescriptor.SetNumMips(1);
			&ColorTextureDescriptor.SetNumSamples(1);

			ColorTexture = RHICreateTexture(ColorTextureDescriptor);
		}

		{
			// Size the array to match the possible screen size
			// Screen size changes in editor all the time so this is required
			DepthBulkData.ResizeArray(Width * Height);
			
			const FString DebugName = "RecreateUDView DepthTexture"; // 5.1 API might require a name to be passed in

			

			// New 5.1 descriptor
			FRHITextureCreateDesc DepthTextureDescr = FRHITextureCreateDesc::Create2D(*DebugName, Width, Height, EPixelFormat::PF_R32_FLOAT);
			
			&DepthTextureDescr.SetNumMips(1);
			&DepthTextureDescr.SetNumSamples(1);
			&DepthTextureDescr.SetFlags(TexCreateFlags);
			
			DepthTexture = RHICreateTexture(DepthTextureDescr);
		}
	}

	

	if (pRenderView)
	{
		error = udRenderTarget_Destroy(&pRenderView);
		if (error != udE_Success)
		{
			UDSDK_ERROR_MSG("udRenderTarget_Destroy error : %s", GetError(error));
			return error;
		}
		pRenderView = nullptr;
	}

	
	error = udRenderTarget_Create(pContext, &pRenderView, pRenderer, Width, Height);
	
	if (error != udE_Success)
	{
		UDSDK_ERROR_MSG("udRenderTarget_Create error : %s", GetError(error));
		return error;
	}
	return error;
}

static int udiv(int x, int y)
{
	return x / y + (x % y != 0);
}

//int UUDSubsystem::LoadThread(int id, int size, const TArray<TSharedPtr<FUdAsset>>& asserts)
//{
//	int length = asserts.Num();
//	int step = udiv(length, size);
//	int begin = step * id;
//	int end = FMath::Min(begin + step, length);
//
//	enum udError error = udE_Failure;
//	for (int j = begin; j < end; j++)
//	{
//		//if (is_release)
//		//	return 1;
//		Load(asserts[j]);
//		
//
//
//		Sleep(25);
//	}
//
//	{
//		//AtomicLock lock(&update_callback_lock);
//		//if (!AutoDock::IsNull(DOCK_MAP))
//		//{
//		//	CWidget_Map * pMapWdg = AutoDock::GetWindow<CWidget_Map*>(DOCK_MAP);
//		//	if (pMapWdg)
//		//	{
//		//		pMapWdg->AutoUpdateSegmentOne();
//		//	}
//		//}
//
//		//AtomicLock lock(&load_thread_lock);
//		//if (load_thread_func)
//		//	load_thread_func();
//	}
//	return 1;
//}
