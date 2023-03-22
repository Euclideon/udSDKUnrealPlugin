#pragma once

#include "Components/SceneComponent.h"
#include "CoreMinimal.h"
#include "UdPointCloudRoot.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UUdPointCloudRoot : public UPrimitiveComponent
{
	GENERATED_BODY()

public:
	UUdPointCloudRoot();
	~UUdPointCloudRoot();

	void UpdatePointCloudTransform(const FMatrix& InMatrix);

	UFUNCTION(BlueprintGetter, Category = "UnlimitedDetail")
	FString GetUrl() const { return Url; }

	UFUNCTION(BlueprintSetter, Category = "UnlimitedDetail")
	void SetUrl(FString InUrl);

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "UnlimitedDetail")
	void RefreshPointCloud();

private:
	void LoadPointCloud();
	void ReloadPointCloud();
	void DestroyPointCloud();
	void LoginPointCloud();

	UPROPERTY(EditAnywhere, BlueprintGetter = GetUrl, BlueprintSetter = SetUrl, Category = "UnlimitedDetail")
	FString Url;

	TSharedPtr<struct FUdAsset> pAsset;

	FDelegateHandle LoginDelegateHandle;
	FDelegateHandle ExitDelegateHandle;

protected:
	/** Overridable native event for when play begins for this actor. */
	virtual void BeginPlay() override;
	
	virtual void BeginDestroy() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostLoad() override;

	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
