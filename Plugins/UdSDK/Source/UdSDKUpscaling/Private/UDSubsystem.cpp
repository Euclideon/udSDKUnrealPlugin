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

int UUDSubsystem::Load(uint32 InUniqueID, TSharedPtr<FUdAsset> OutAssert)
{
	enum udError error = udE_Failure;

	if (!LoginFlag)
	{
		UDSDK_ERROR_MSG("Not logged in!");
		return error;
	}

	{
		FScopeLock ScopeLock(&DataMutex);
		if (AssetsMap.Contains(InUniqueID))
		{
			UDSDK_ERROR_MSG("1:AUdPointCloud creating udPointCloud instance already exists!");
			return error;
		}
	}

	FString folder = OutAssert->folder;
	FString uri = OutAssert->url;

	if (uri.IsEmpty())
	{
		UDSDK_ERROR_MSG("Url is Empty!");
		return error;
	}

	struct udPointCloudHeader header;
	memset(&header, 0, sizeof(header));

	struct udPointCloud* pModel = NULL;

	error = udPointCloud_Load(pContext, &pModel, TCHAR_TO_UTF8(*uri), &header);
	if (error != udE_Success)
	{
		UDSDK_ERROR_MSG("udPointCloud_Load error : %s %s", GetError(error), *uri);
		return error;
	}

	udDouble3 pivot = udDouble3::create(header.pivot[0], header.pivot[1], header.pivot[2]);

	OutAssert->pivot.X = header.pivot[0];
	OutAssert->pivot.Y = header.pivot[1];
	OutAssert->pivot.Z = header.pivot[2];

	udRenderInstance inst;
	memset(&inst, 0, sizeof(udRenderInstance));
	inst.pPointCloud = pModel;
	memcpy(inst.matrix, header.storedMatrix, sizeof(header.storedMatrix));
	inst.pVoxelShader = vcVoxelShader_Black;
	inst.pVoxelUserData = (void*)OutAssert.Get();

	OutAssert->pPointCloud = pModel;

	uint32_t attributeOffset;
	if (udAttributeSet_GetOffsetOfStandardAttribute(&header.attributes, udSA_ARGB, &attributeOffset) == udE_Success)
	{
		inst.pVoxelShader = vcVoxelShader_Colour;
	}

	{
		FScopeLock ScopeLock(&DataMutex);
		if (AssetsMap.Contains(InUniqueID))
		{
			UDSDK_ERROR_MSG("2:AUdPointCloud creating udPointCloud instance already exists!");

			error = udPointCloud_Unload(&inst.pPointCloud);
			if (error != udE_Success)
			{
				UDSDK_ERROR_MSG("Load->udPointCloud_Unload error : %s", GetError(error));
			}

			return error;
		}

		InstanceArray.Push(inst);
		AssetsMap.Add(InUniqueID, OutAssert);
	}

	return error;
}

int UUDSubsystem::Remove(uint32 InUniqueID)
{
	FScopeLock ScopeLock(&DataMutex);
	enum udError error = udE_Success;
	void* pPointCloud = nullptr;
	if (TSharedPtr<FUdAsset> Asset = AssetsMap.FindRef(InUniqueID))
	{
		if (Asset->pPointCloud)
		{
			pPointCloud = Asset->pPointCloud;
		}
		AssetsMap.Remove(InUniqueID);
	}
	if (pPointCloud)
	{
		uint32 Index = 0;
		for (auto inst : InstanceArray)
		{
			if (inst.pPointCloud == pPointCloud)
			{
				error = udPointCloud_Unload(&inst.pPointCloud);
				if (error != udE_Success)
				{
					UDSDK_ERROR_MSG("udPointCloud_Unload error : %s", GetError(error));
				}
				break;
			}
			Index++;
		}
		InstanceArray.RemoveAt(Index);
	}
	return error;
}

bool UUDSubsystem::Find(uint32 InUniqueID)
{
	FScopeLock ScopeLock(&DataMutex);
	if (TSharedPtr<FUdAsset> Asset = AssetsMap.FindRef(InUniqueID))
	{
		return true;
	}
	return false;
}

int UUDSubsystem::SetTransform(uint32 InUniqueID, const FMatrix& InMatrix)
{
	if (!IsLogin())
		return -1;

	// TODO
	// BUG - Potentially throws here if you change scenes at all? OR While streaming UDS?
	
	UE_LOG(LogTemp, Display, TEXT("Setting transform ..."));
	
	FScopeLock ScopeLock(&DataMutex);
	enum udError error = udE_Success;
	void* pPointCloud = nullptr;
	if (TSharedPtr<FUdAsset> Asset = AssetsMap.FindRef(InUniqueID))
	{
		if (Asset->pPointCloud)
		{
			pPointCloud = Asset->pPointCloud;
		}
		if (pPointCloud)
		{
			for (auto& inst : InstanceArray)
			{
				if (inst.pPointCloud == pPointCloud)
				{
					//FTransform t;
					//t.SetLocation(InTransform.GetLocation());
					//t.SetScale3D(InTransform.GetScale3D() * Asset->scale_xyz);
					//t.SetRotation(InTransform.GetRotation());
					//FuncMat2Array(inst.matrix, t.ToMatrixWithScale());

					for (int i = 0; i < 4; ++i)
					{
						inst.matrix[0 + i * 4] = InMatrix.M[i][0];
						inst.matrix[1 + i * 4] = InMatrix.M[i][1];
						inst.matrix[2 + i * 4] = InMatrix.M[i][2];
						inst.matrix[3 + i * 4] = InMatrix.M[i][3];
					}
					
					//UDSDK_SCREENDE_DEBUG_MSG("SetTransform::Location : %d : %s", InUniqueID, *InTransform.GetLocation().ToString());
					//inst.matrix[12] = InTransform.GetLocation().X;
					//inst.matrix[13] = InTransform.GetLocation().Y;
					//inst.matrix[14] = InTransform.GetLocation().Z;

					//udDouble3 ud_position;
					//udDouble3 ud_scale;
					//udDouble4x4 storedMatrix;
					//udDouble3 pivot = udDouble3::create(Asset->pivot.X, Asset->pivot.Y, Asset->pivot.Z);
					//if (1)
					//{
					//	ud_position = udDouble3::create(InTransform.GetLocation().X, InTransform.GetLocation().Y, InTransform.GetLocation().Z);
					//
					//	ud_scale = udDouble3::create(InTransform.GetScale3D().X * Asset->scale_xyz.X,
					//		InTransform.GetScale3D().Y * Asset->scale_xyz.Y, 
					//		InTransform.GetScale3D().Z * Asset->scale_xyz.Z);
					//
					//	FVector Euler = InTransform.GetRotation().Euler();
					//	udDouble3 euler = udDouble3::create(Euler.X, Euler.Y, Euler.Z);
					//	storedMatrix = udDouble4x4::translation(pivot) *
					//		udDouble4x4::rotationYPR(UD_DEG2RAD(euler), ud_position) *
					//		udDouble4x4::scaleNonUniform(ud_scale) *
					//		udDouble4x4::translation(-pivot);
					//	memcpy(inst.matrix, storedMatrix.a, sizeof(double) * 16);
					//}
					
					break;
				}
			}
		}
	}
	
	return error;
}

// The main function for rendering out UD images
int UUDSubsystem::CaptureUDSImage(const FSceneView& View)
{
	// TODO - If Correct width/height can be marshaled into this function, we may not require the EngineSubsystem below
	// possible refactor here later ...
	// UUdSDKSubsystem* udSingletonSubSystem = GEngine->GetEngineSubsystem<UUdSDKSubsystem>();

	// prep an empty error
	enum udError error = udE_Failure;
	
	if (!LoginFlag)
	{
		return error;
	}

	{
		FScopeLock ScopeLock(&DataMutex);
		if (InstanceArray.Num() == 0)
		{
			return error;
		}
	}

	
	
	// These values are incorrect, but are at least visually plausable.
	// If we can however get our singleton correctly, we should use the more accurate values contained within that
	// auto vartest = View.UnconstrainedViewRect.Width();
	int32 nWidth = View.UnconstrainedViewRect.Width();
	int32 nHeight = View.UnconstrainedViewRect.Height();
	
	// UE_LOG(LogTemp, Display, TEXT("%s: Unscaled Width: %d, Unscaled Height: %d"), TEXT(__FUNCTION__), View.UnscaledViewRect.Width(), View.UnscaledViewRect.Height());
	
	// Subsystem is valid, we can get better values here
	// We need to ensure the values are reasonable beacuse they can begin with junk values
	/*
	if (udSingletonSubSystem != nullptr) // Better way to check this?
	{
		UE_LOG(LogTemp, Display, TEXT("%s: Subsystem Width: %d, Height: %d."), TEXT(__FUNCTION__), udSingletonSubSystem->Width(), udSingletonSubSystem->Height());
		
		// Get the width
		if (udSingletonSubSystem->Width() > 0 && udSingletonSubSystem->Width() < 2048)
		{
			//nWidth = udSingletonSubSystem->Width();
		}
		
		// Get the height
		if (udSingletonSubSystem->Height() > 0 && udSingletonSubSystem->Height() < 2048)
		{
			///nHeight = udSingletonSubSystem->Height();
		}
	}
	*/

	// Return early if we have really invalid values?
	if (nWidth <= 0 || nHeight <= 0)
	{
		UDSDK_ERROR_MSG("Error, width or height = 0 : %s", GetError(error));
		return error;
	}

	// TODO - This needs to be handled better.
	// Hardcap the render to some reasonable number - sometimes the width/height can come out at unreasonably large numbers which signifies that this frame shouldnt render
	if (nWidth >= 8192 || nHeight >= 8192)
	{
		check(false);
		UDSDK_ERROR_MSG("Error, width or height too big : %s", GetError(error));
		return error;
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
