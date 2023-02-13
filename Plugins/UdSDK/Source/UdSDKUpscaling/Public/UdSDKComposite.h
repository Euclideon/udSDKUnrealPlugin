#pragma once
#include "CoreMinimal.h"
#include <chrono>
#include "udContext.h"
#include "udRenderContext.h"
#include "udPointCloud.h"
#include "udError.h"
#include "udRenderTarget.h"
#include "udConfig.h"
#include "UdSDKMacro.h"
#include "UdSDKDefine.h"
#include "SceneView.h"
#include "Utils/CSingleton.h"
#include "Utils/CThreadPool.h"

DECLARE_MULTICAST_DELEGATE(FUdLoginDelegate);
DECLARE_MULTICAST_DELEGATE(FUdExitDelegate);

class FUdSDKCompositeViewExtension;
class CUdSDKComposite : public CSingleton<CUdSDKComposite>
{
public:
	CUdSDKComposite();
	~CUdSDKComposite();

	
	int LoginLegacy();
	int Exit();

	int Load(uint32 InUniqueID, TSharedPtr<FUdAsset> OutAssert);
	int AsyncLoad(uint32 InUniqueID, TSharedPtr<FUdAsset> OutAssert, const FunCP0& InFunc = nullptr);

	int Remove(uint32 InUniqueID);
	int AsyncRemove(uint32 InUniqueID,const FunCP0 & InFunc = nullptr);

	bool Find(uint32 InUniqueID);
	int AsyncFind(uint32 InUniqueID, const FunCP1& InFunc = nullptr);

	int AsyncSetTransform(uint32 InUniqueID, const FTransform& InTransform);
	int SetTransform(uint32 InUniqueID, const FTransform& InTransform);

	int AsyncSetSelected(uint32 InUniqueID, bool InSelect);
	int SetSelected(uint32 InUniqueID, bool InSelect);
	int SetSelectedByModelIndex(uint32 InModelIndex, bool InSelect);

	bool IsLogin() const {
		return LoginFlag;
	};

	FTexture2DRHIRef GetColorTexture()const {
		return ColorTexture;
	};

	FTexture2DRHIRef GetDepthTexture()const {
		return DepthTexture;
	};

	bool IsValid()const {
		return IsLogin() &&
			GetColorTexture().IsValid() &&
			GetDepthTexture().IsValid() && 
			InstanceArray.Num() > 0;
	};

	int CaptureUDSImage(const FSceneView& View);

	FUdLoginDelegate LoginDelegate;
	FUdExitDelegate ExitFrontDelegate;
	FUdExitDelegate ExitLaterDelegate;
public:
	static void SetSelectColor(const uint32& InValue);
	static uint32 GetSelectColor();
private:
	int Init();
	int RecreateUDView(int InWidth, int InHeight, float InFOV);
	//int LoadThread(int id, int size, const TArray<TSharedPtr<FUdAsset>>& asserts);
	
private:
	FTexture2DRHIRef ColorTexture;
	FTexture2DRHIRef DepthTexture;

	//bool InitFlag;
	bool LoginFlag;

	FString ServerUrl;
	FString Username;
	FString Password;
	bool Offline;
	static uint32 SelectColor;

	struct udContext* pContext = NULL;
	struct udContextPartial* pContextPartial = NULL; // New 5.1 context partial
	struct udRenderContext* pRenderer = NULL;
	struct udRenderTarget* pRenderView = NULL;

	double ViewArray[16] = {0};
	double ProjArray[16] = {0};

	int Width = 0;
	int Height = 0;

	bool LoadRunning;
	//TArray<TSharedPtr<FUdAsset>> AssetArray;
	//FCriticalSection InstanceArrayMutex;

	FCriticalSection DataMutex;

	TArray<udRenderInstance> InstanceArray;
	
	//FCriticalSection AssetsMapMutex;
	TMap<uint32, TSharedPtr<FUdAsset>> AssetsMap;

	FCriticalSection BulkDataMutex;
	FUdSDKResourceBulkData<FColor> ColorBulkData;
	FUdSDKResourceBulkData<float> DepthBulkData;

	FMatrix ProjectionMatrix;

	TSharedPtr<FUdSDKCompositeViewExtension, ESPMode::ThreadSafe> ViewExtension;
};