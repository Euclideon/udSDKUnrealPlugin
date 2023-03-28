#pragma once
#include "CoreMinimal.h"
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

typedef uint32_t udVoxelShader(struct udPointCloud* pPointCloud, const struct udVoxelID* pVoxelID, const void* pVoxelUserData);

USTRUCT(BlueprintType)
struct FUDPointCloudHandle
{
	GENERATED_BODY()

public:
	FString URL;

	bool bIsLoaded;

	udPointCloud* PointCloud;
	udVoxelShader* VoxelShaderFunc;

	FVector Pivot;

	int RefCount;
};

USTRUCT()
struct FUDPointCloudInstanceHandle
{
	GENERATED_BODY()

	int64_t id;
	const FSceneInterface* Scene;
	udRenderInstance RenderInstance;
};

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
	void Exit();

	FUDPointCloudHandle* Load(FString URL);
	void Remove(FUDPointCloudHandle* PCI);
	bool Find(FString URL);

	UFUNCTION(BlueprintCallable)
	bool IsLogin() const { return (pContext != nullptr); };

	FTexture2DRHIRef GetColorTexture()const { return ColorTexture; };
	FTexture2DRHIRef GetDepthTexture()const { return DepthTexture; };

	bool IsValid() const { return IsLogin() && GetColorTexture().IsValid() && GetDepthTexture().IsValid(); };

	int64_t QueueInstance(FUDPointCloudHandle* PCI, const FMatrix& InMatrix, FSceneInterface* Scene);
	bool RemoveInstance(int64_t id);
	bool UpdateInstance(int64_t id, const FMatrix &InMatrix);

	int CaptureUDSImage(const FSceneView& View);

protected:
	UPROPERTY(BlueprintReadOnly)
	FString ServerUrl;

	UPROPERTY(BlueprintReadOnly)
	FString APIKey;

private:
	int Init();
	int RecreateUDView(int InWidth, int InHeight, float InFOV);

private:
	FTexture2DRHIRef ColorTexture;
	FTexture2DRHIRef DepthTexture;

	struct udContext* pContext = NULL;
	struct udContextPartial* pContextPartial = NULL; // New 5.1 context partial for web based logins
	struct udRenderContext* pRenderer = NULL;
	struct udRenderTarget* pRenderView = NULL;

	int64_t NextID;

	double ViewArray[16] = {};
	double ProjArray[16] = {};

	// TODO - Are these effectively the same as the width/height values in the other singleton?
	int32 Width = 0;
	int32 Height = 0;

	bool LoadRunning;
	
	FCriticalSection DataMutex;

	TArray<FUDPointCloudInstanceHandle> RenderInstanceHandles;
	TMap<FString, FUDPointCloudHandle> AssetsMap;

	FUdSDKResourceBulkData<FColor> ColorBulkData;
	FUdSDKResourceBulkData<float> DepthBulkData;

	FMatrix ProjectionMatrix;

	TSharedPtr<FUdSDKCompositeViewExtension, ESPMode::ThreadSafe> ViewExtension;
};