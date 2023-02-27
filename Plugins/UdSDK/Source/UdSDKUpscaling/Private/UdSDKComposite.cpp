#include "UdSDKComposite.h"
#include "Runtime/RHI/Public/RHI.h"
#include "ImageUtils.h"
#include "Slate/SceneViewport.h"
#include "Engine/GameViewportClient.h"
#include "Settings/ObjectStorageSettings.h"
#include "UdSDKCompositeViewExtension.h"
#include "UdSDKDefine.h"
#include "Utils/CThreadPool.h"
#include "udContext.h"

// #include "UdSDK/Public/UdSDKSubsystem.h"

uint32 CUdSDKComposite::SelectColor = 0xff0071c1;

template <typename ValueType>
void ResizeArray(TArray<ValueType>& Array, int32 Size)
{
	Array.Empty(Size);
	Array.AddUninitialized(Size);
}

uint32_t vcPCShaders_BuildAlpha(FUdAsset* f)
{
	return (f->selected ? CUdSDKComposite::GetSelectColor() : 0x00000000);
}

uint32_t vcVoxelShader_Black(udPointCloud* /*pPointCloud*/, const udVoxelID* /*pVoxelID*/, const void* pUserData)
{
	FUdAsset* pData = (FUdAsset*)pUserData;

	return vcPCShaders_BuildAlpha(pData) | 0x00000000;
}

udFloat3 g_globalSunDirection = udFloat3::create((float)0.1, (float)0.1, (float)0.1);
uint32_t vcVoxelShader_Colour(udPointCloud* pPointCloud, const udVoxelID* pVoxelID, const void* pUserData)
{
	FUdAsset* pData = (FUdAsset*)pUserData;

	uint64_t color64 = 0;
	udPointCloud_GetNodeColour64(pPointCloud, pVoxelID, &color64);
	uint32_t result;
	//uint32_t encNormal = (uint32_t)(color64 >> 32);
	//if (encNormal)
	//{
	//	udFloat3 normal;
	//	normal.x = int16_t(encNormal >> 16) / 32767.f;
	//	normal.y = int16_t(encNormal & 0xfffe) / 32767.f;
	//	normal.z = 1.f - (normal.x * normal.x + normal.y * normal.y);
	//	if (normal.z > 0.001)
	//		normal.z = udSqrt(normal.z);
	//	if (encNormal & 1)
	//		normal.z = -normal.z;
	//
	//	float dot = (udDot(g_globalSunDirection, normal) * 0.5f) + 0.5f;
	//	result = (uint8_t(((color64 >> 16) & 0xff) * dot) << 16)
	//		| (uint8_t(((color64 >> 8) & 0xff) * dot) << 8)
	//		| (uint8_t(((color64 >> 0) & 0xff) * dot) << 0);
	//}
	//else
	{
		result = (uint32_t)color64 & 0xffffff;
	}

	return vcPCShaders_BuildAlpha(pData) | (0xffffff & result);
}

auto FuncMat2Array = [](double* array, const FMatrix& Mat)
{
	static bool transpose = false;
	if (transpose)
	{
		for (int j = 0; j < 4; ++j)
			for (int i = 0; i < 4; ++i)
				array[j * 4 + i] = Mat.M[i][j];
	}
	else
	{
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				array[i * 4 + j] = Mat.M[i][j];
	}
};

CUdSDKComposite::CUdSDKComposite()
{
	Width = 0;
	Height = 0;
	LoginFlag = false;
	ViewExtension = nullptr;
	int32 NumberOfCores = FPlatformMisc::NumberOfCores();
	if (!CThreadPool::Get())
		new CThreadPool(NumberOfCores);
}

CUdSDKComposite::~CUdSDKComposite()
{
	if (IsLogin())
		Exit();


	LoginDelegate.Clear();
	ExitFrontDelegate.Clear();
	ExitLaterDelegate.Clear();

	if (CThreadPool::Get())
		delete CThreadPool::Get();
}

void CUdSDKComposite::SetSelectColor(const uint32& InValue)
{
	SelectColor = InValue;
}

uint32 CUdSDKComposite::GetSelectColor()
{
	return SelectColor;
}

// Does NOT init the udContext
int CUdSDKComposite::Init()
{
	enum udError error = udE_Success;

	if (const UObjectStorageSettings* Settings = GetDefault<UObjectStorageSettings>())
	{
		ServerUrl = Settings->ServerPath.ToString();
		Username = Settings->Username.ToString();
		APIKey = Settings->Password.ToString();
		Offline = Settings->Offline;
		SelectColor = Settings->SelectColor.DWColor();
		if((ServerUrl.IsEmpty() || Username.IsEmpty() || APIKey.IsEmpty()) && !Offline)
			error = udE_Failure;
	}
	else
	{
		ServerUrl = "";
		Username = "";
		APIKey = "";
		Offline = false;
		error = udE_Failure;
	}
	return error;
}

// Called as a result of clicking Login() from the widget
int CUdSDKComposite::LoginFunction()
{
	// Logging startup values at login
	UE_LOG(LogTemp, Display, TEXT("Auth values at start of Login() function"));
	UE_LOG(LogTemp, Display, TEXT("Offline: %d"), (int)Offline);
	UE_LOG(LogTemp, Display, TEXT("Server: %s"), *ServerUrl);
	UE_LOG(LogTemp, Display, TEXT("Username: %s"), *Username);
	// UE_LOG(LogTemp, Display, TEXT("Password: %s"), *APIKey);
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
	
	// Logging startup values at login
	UE_LOG(LogTemp, Display, TEXT("Auth after Init() value"));
	UE_LOG(LogTemp, Display, TEXT("Offline: %d"), (int)Offline);
	UE_LOG(LogTemp, Display, TEXT("Server: %s"), *ServerUrl);
	UE_LOG(LogTemp, Display, TEXT("Username: %s"), *Username);
	// UE_LOG(LogTemp, Display, TEXT("Password: %s"), *APIKey); // Probably dont want to log API keys maybe
	UE_LOG(LogTemp, Display, TEXT("Server: %s"), *ServerUrl);
	
	if (error != udE_Success) // Should fail basically due to user error only
	{
		UDSDK_ERROR_MSG("Initialization failed!");
		UE_LOG(LogTemp, Warning, TEXT("Auth values after failing initialization: "));
		UE_LOG(LogTemp, Warning, TEXT("Offline: %d"), (int)Offline);
		UE_LOG(LogTemp, Warning, TEXT("Server: %s"), *ServerUrl);
		UE_LOG(LogTemp, Warning, TEXT("Username: %s"), *Username);
		
		UE_LOG(LogTemp, Warning, TEXT("Current State of Connect Attempt: %d "), error);
		return error;
	}
	
	{
		FScopeLock ScopeLockInst(&DataMutex);
		InstanceArray.Reset();
	}

	// Should keep or nah? Double check with sam
	error = udConfig_IgnoreCertificateVerification(true); // Not sure if we still need this?

	if (error != udE_Success)
	{
		UDSDK_ERROR_MSG("udConfig_IgnoreCertificateVerification(true) error : %s", GetError(error));
		return error;
	}
	
	{
		const FString ApplicationVersion = "0.0";
		const FString ApplicationName = "UE5_Client";

		error = udContext_ConnectWithKey(&pContext,
			TCHAR_TO_UTF8(*ServerUrl),
			TCHAR_TO_UTF8(*ApplicationName),
			TCHAR_TO_UTF8(*ApplicationVersion),
			TCHAR_TO_UTF8(*APIKey));
	}

	// Log the error state
	UE_LOG(LogTemp, Warning, TEXT("%s, Current State of Connect Attempt: %d "), TEXT(__FUNCTION__), error);

	
	
	if (error != udE_Success)
	{
		UDSDK_ERROR_MSG("udContext_Connect error : %s Offline : %d", GetError(error), (int)Offline);
		return error;
	}

	error = udRenderContext_Create(pContext, &pRenderer);
	if (error != udE_Success)
	{
		UDSDK_ERROR_MSG("udRenderContext_Create error : %s", GetError(error));
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
	LoginDelegate.Broadcast();
	
	return error;
}

int CUdSDKComposite::Exit()
{
	//FScopeLock ScopeLock(&CallMutex);
	ExitFrontDelegate.Broadcast();
	enum udError error = udE_Failure;

	ViewExtension = nullptr;

	ServerUrl = ""; // udstream.euclideon.com and udcloud.euclideon.com
	Username = "";
	APIKey = "";
	Offline = false;

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
	
	ExitLaterDelegate.Broadcast();
	return error;
}

//PRAGMA_DISABLE_OPTIMIZATION
int CUdSDKComposite::Load(uint32 InUniqueID, TSharedPtr<FUdAsset> OutAssert)
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
	FVector coords = OutAssert->coords;
	FVector rotation = OutAssert->rotation;
	FVector scale_xyz = OutAssert->scale_xyz;
	FVector position = OutAssert->position;
	bool b_scale_xyz = OutAssert->b_scale_xyz;
	bool selected = OutAssert->selected;
	bool geometry = OutAssert->geometry;
	double scale = OutAssert->scale;

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

	//double maxDim = 0;
	//for (int i = 0; i < 3; i++) {
	//	if (maxDim < header.boundingBoxExtents[i])
	//		maxDim = header.boundingBoxExtents[i];
	//}
	//float s = (float)(1 / (2 * maxDim));//bounding box extents are relative to centre -> size of model is double the extents

	scale = header.scaledRange;

	udDouble3 ud_position;
	udDouble3 ud_scale;
	udDouble4x4 storedMatrix;
	udDouble3 pivot = udDouble3::create(header.pivot[0], header.pivot[1], header.pivot[2]);
	if (geometry)
	{
		ud_position = udDouble3::create(coords.X, coords.Y, coords.Z);
		if (b_scale_xyz)
			ud_scale = udDouble3::create(scale * scale_xyz.X, scale * scale_xyz.Y, scale * scale_xyz.Z);
		else
			ud_scale = udDouble3::create(scale);
		udDouble3 euler = udDouble3::create(rotation.X, rotation.Y, rotation.Z);
		storedMatrix = udDouble4x4::translation(pivot) *
			udDouble4x4::rotationYPR(UD_DEG2RAD(euler), ud_position) *
			udDouble4x4::scaleNonUniform(ud_scale) *
			udDouble4x4::translation(-pivot);
		memcpy(header.storedMatrix, storedMatrix.a, sizeof(double) * 16);
	}
	else
	{
		storedMatrix = udDouble4x4::create(header.storedMatrix);
		udDoubleQuat orientation;
		(udDouble4x4::translation(-pivot) * storedMatrix * udDouble4x4::translation(pivot)).extractTransforms(ud_position, ud_scale, orientation);
	}

	OutAssert->scale_xyz.X = ud_scale.x;
	OutAssert->scale_xyz.Y = ud_scale.y;
	OutAssert->scale_xyz.Z = ud_scale.z;
	
	OutAssert->pivot.X = header.pivot[0];
	OutAssert->pivot.Y = header.pivot[1];
	OutAssert->pivot.Z = header.pivot[2];

	header.storedMatrix[12] = position.X;
	header.storedMatrix[13] = position.Y;
	header.storedMatrix[14] = position.Z;

	OutAssert->selected = false;


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
//PRAGMA_ENABLE_OPTIMIZATION

//int CUdSDKComposite::AsyncLoad(const TArray<TSharedPtr<FUdAsset>>& asserts)
//{
//	//FScopeLock ScopeLock(&CallMutex);
//
//	enum udError error = udE_Failure;
//
//	if (!LoginFlag)
//	{
//		UDSDK_ERROR_MSG("AsyncLoad -> Not logged in!");
//		return error;
//	}
//
//	CThreadPool::Get()->enqueue([&] {
//		LoadRunning = true;
//
//		std::vector< std::future<int> > results;
//		std::vector< int > states;
//
//		int size = FMath::Min(FPlatformMisc::NumberOfCores(), CThreadPool::Get()->idleCount());
//		assert(size && "ThreadPool is NULL");
//		for (int i = 0; i < size; i++)
//		{
//			results.emplace_back(CThreadPool::Get()->enqueue(C_P3(CUdSDKComposite::LoadThread, this), i, size, asserts));
//			states.emplace_back(0);
//		}
//		for (int i = 0; i < size; i++)
//		{
//			states[i] = results[i].get();
//		}
//		bool loop;
//		do
//		{
//			loop = false;
//			for (int i = 0; i < size; i++)
//			{
//				if (states[i] == 0)
//					loop = true;
//			}
//		} while (loop);
//
//		LoadRunning = false;
//
//		//if (load_finished_func)
//		//	load_finished_func();
//		//
//		//async_load_render();
//
//	});
//
//	return udE_Success;
//}

int CUdSDKComposite::AsyncLoad(uint32 InUniqueID, TSharedPtr<FUdAsset> OutAssert, const FunCP0& InFunc)
{
	//FScopeLock ScopeLock(&CallMutex);

	enum udError error = udE_Failure;

	if (!LoginFlag)
	{
		UDSDK_ERROR_MSG("AsyncLoad -> Not logged in!");
		return error;
	}

	uint32 UniqueID = InUniqueID;
	TSharedPtr<FUdAsset> Assert = OutAssert;
	const FunCP0& Func = InFunc;
	CThreadPool::Get()->enqueue([UniqueID, Assert, Func, this] {
		Load(UniqueID, Assert);
		//FPlatformProcess::Sleep(0.025f);
		if (Func)
			Func();
	});

	return udE_Success;
}

int CUdSDKComposite::Remove(uint32 InUniqueID)
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

int CUdSDKComposite::AsyncRemove(uint32 InUniqueID, const FunCP0& InFunc)
{
	enum udError error = udE_Failure;

	if (!LoginFlag)
	{
		UDSDK_ERROR_MSG("AsyncLoad -> Not logged in!");
		return error;
	}

	uint32 UniqueID = InUniqueID;
	const FunCP0 & Func = InFunc;
	CThreadPool::Get()->enqueue([UniqueID, Func, this] {
		Remove(UniqueID);
		if (Func)
			Func();
	});

	return udE_Success;
}

bool CUdSDKComposite::Find(uint32 InUniqueID)
{
	FScopeLock ScopeLock(&DataMutex);
	if (TSharedPtr<FUdAsset> Asset = AssetsMap.FindRef(InUniqueID))
	{
		return true;
	}
	return false;
}

int CUdSDKComposite::AsyncFind(uint32 InUniqueID, const FunCP1& InFunc)
{
	enum udError error = udE_Failure;

	if (!LoginFlag)
	{
		UDSDK_ERROR_MSG("AsyncLoad -> Not logged in!");
		return error;
	}

	uint32 UniqueID = InUniqueID;
	const FunCP1& Func = InFunc;
	CThreadPool::Get()->enqueue([UniqueID, Func, this] {
		bool bFind = Find(UniqueID);
		if (Func)Func(bFind);
	});

	return udE_Success;
}

int CUdSDKComposite::AsyncSetTransform(uint32 InUniqueID, const FTransform& InTransform)
{
	enum udError error = udE_Failure;

	if (!LoginFlag)
	{
		UDSDK_ERROR_MSG("AsyncLoad -> Not logged in!");
		return error;
	}

	uint32 UniqueID = InUniqueID;
	const FTransform& Transform = InTransform;
	CThreadPool::Get()->enqueue([UniqueID, Transform, this] {
		SetTransform(UniqueID, Transform);
	});

	return udE_Success;
}

//PRAGMA_DISABLE_OPTIMIZATION
int CUdSDKComposite::SetTransform(uint32 InUniqueID, const FTransform& InTransform)
{
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
					FTransform t;
					t.SetLocation(InTransform.GetLocation());
					t.SetScale3D(InTransform.GetScale3D() * Asset->scale_xyz);
					t.SetRotation(InTransform.GetRotation());
					FuncMat2Array(inst.matrix, t.ToMatrixWithScale());
					
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
//PRAGMA_ENABLE_OPTIMIZATION

int CUdSDKComposite::AsyncSetSelected(uint32 InUniqueID, bool InSelect)
{
	enum udError error = udE_Failure;

	if (!LoginFlag)
	{
		UDSDK_ERROR_MSG("AsyncLoad -> Not logged in!");
		return error;
	}

	uint32 UniqueID = InUniqueID;
	const bool& Select = InSelect;
	CThreadPool::Get()->enqueue([UniqueID, Select, this] {
		SetSelected(UniqueID, Select);
	});

	return udE_Success;
}

int CUdSDKComposite::SetSelected(uint32 InUniqueID, bool InSelect)
{
	FScopeLock ScopeLock(&DataMutex);
	enum udError error = udE_Success; // can't fail?
	void* pPointCloud = nullptr;
	if (TSharedPtr<FUdAsset> Asset = AssetsMap.FindRef(InUniqueID)) // but can fail and return nullptr?
	{
		Asset->selected = InSelect;
	}
	return error;
}

int CUdSDKComposite::SetSelectedByModelIndex(uint32 InModelIndex, bool InSelect)
{
	FScopeLock ScopeLock(&DataMutex);
	enum udError error = udE_Success;

	if (InModelIndex < (uint32)InstanceArray.Num())
	{
		udRenderInstance& tmpIns = InstanceArray[InModelIndex];
		FUdAsset* pAsset = static_cast<FUdAsset*>(tmpIns.pVoxelUserData);
		if (pAsset)
		{
			pAsset->selected = InSelect;
		}
	}

	return error;
}
//PRAGMA_DISABLE_OPTIMIZATION
// The main function for rendering out UD images
int CUdSDKComposite::CaptureUDSImage(const FSceneView& View)
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

	// Hardcap the render to some reasonable number
	// TODO - Probably want to handle this better
	if (nWidth >= 2048 || nHeight >= 2048)
	{
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

//PRAGMA_ENABLE_OPTIMIZATION
int CUdSDKComposite::RecreateUDView(int32 InWidth, int32 InHeight, float InFOV)
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

//int CUdSDKComposite::LoadThread(int id, int size, const TArray<TSharedPtr<FUdAsset>>& asserts)
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
