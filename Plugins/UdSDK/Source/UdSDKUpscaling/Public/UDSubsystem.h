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

#include "UDSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE(FUdLoginDelegate);
DECLARE_MULTICAST_DELEGATE(FUdExitDelegate);

class FUdSDKCompositeViewExtension;

UCLASS()
class UDSDKUPSCALING_API UUDSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	UUDSubsystem();
	~UUDSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	int LoginFunction();
	int Exit();

	int Load(uint32 InUniqueID, TSharedPtr<FUdAsset> OutAssert);
	int Remove(uint32 InUniqueID);
	bool Find(uint32 InUniqueID);
	int SetTransform(uint32 InUniqueID, const FMatrix& InMatrix);

	UFUNCTION(BlueprintCallable)
	bool IsLogin() const { return LoginFlag; };

	FTexture2DRHIRef GetColorTexture()const { return ColorTexture; };
	FTexture2DRHIRef GetDepthTexture()const { return DepthTexture; };

	bool IsValid() const { return IsLogin() && GetColorTexture().IsValid() && GetDepthTexture().IsValid() && InstanceArray.Num() > 0; };

	int CaptureUDSImage(const FSceneView& View);

protected:
	UPROPERTY(BlueprintReadOnly)
	FString ServerUrl;

	UPROPERTY(BlueprintReadOnly)
	FString Username;

	UPROPERTY(BlueprintReadOnly)
	FString APIKey;

	UPROPERTY(BlueprintReadOnly)
	bool Offline;

private:
	int Init();
	int RecreateUDView(int InWidth, int InHeight, float InFOV);

private:
	FTexture2DRHIRef ColorTexture;
	FTexture2DRHIRef DepthTexture;

	//bool InitFlag;
	bool LoginFlag;

	struct udContext* pContext = NULL;
	struct udContextPartial* pContextPartial = NULL; // New 5.1 context partial for web based logins
	struct udRenderContext* pRenderer = NULL;
	struct udRenderTarget* pRenderView = NULL;

	double ViewArray[16] = {};
	double ProjArray[16] = {};

	// TODO - Are these effectively the same as the width/height values in the other singleton?
	int32 Width = 0;
	int32 Height = 0;

	bool LoadRunning;
	
	FCriticalSection DataMutex;

	TArray<udRenderInstance> InstanceArray;
	
	TMap<uint32, TSharedPtr<FUdAsset>> AssetsMap;

	FCriticalSection BulkDataMutex;
	FUdSDKResourceBulkData<FColor>ColorBulkData;
	FUdSDKResourceBulkData<float> DepthBulkData;

	FMatrix ProjectionMatrix;

	TSharedPtr<FUdSDKCompositeViewExtension, ESPMode::ThreadSafe> ViewExtension;
};