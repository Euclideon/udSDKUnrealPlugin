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

	virtual void ApplyWorldOffset(const FVector& InOffset, bool bWorldShift) override;
	
	UFUNCTION(BlueprintGetter, Category = "UdSDK")
	FString GetUrl() const { return Url; }

	UFUNCTION(BlueprintSetter, Category = "UdSDK")
	void SetUrl(FString InUrl);

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "UdSDK")
	void RefreshPointCloud();

private:
	void LoadPointCloud();
	void ReloadPointCloud();
	void DestroyPointCloud();
	void SelectPointCloud(bool InSelect);
	void LoginPointCloud();
	void ExitPointCloud();

	UPROPERTY(EditAnywhere, Category = "UdSDK|Debug")
	bool UpdateInEditor = true;

	UPROPERTY(EditAnywhere, BlueprintGetter = GetUrl, BlueprintSetter = SetUrl, Category = "UdSDK")
	FString Url;

	//uint8 bWasDuplicatedForPIE : 1;
	//uint8 bWasHiddenEd : 1;
	//uint8 bWasSelectedInEditor : 1;

	TSharedPtr<struct FUdAsset> pAsset;

	FDelegateHandle LoginDelegateHandle;
	FDelegateHandle ExitDelegateHandle;

protected:
	/** Overridable native event for when play begins for this actor. */
	virtual void BeginPlay() override;
	
	virtual bool MoveComponentImpl(const FVector& Delta, const FQuat& NewRotation, bool bSweep, FHitResult* OutHit = NULL, EMoveComponentFlags MoveFlags = MOVECOMP_NoFlags, ETeleportType Teleport = ETeleportType::None) override;
	
	virtual void BeginDestroy() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostLoad() override;

	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
