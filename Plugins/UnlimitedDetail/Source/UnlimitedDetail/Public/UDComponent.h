#pragma once

#include "Components/SceneComponent.h"
#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "UDComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UUDComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

	friend class FPointCloudSceneProxy;

public:
	UUDComponent();

	UFUNCTION(BlueprintGetter, Category = "UnlimitedDetail")
	FString GetUrl() const { return Url; }

	UFUNCTION(BlueprintSetter, Category = "UnlimitedDetail")
	void SetUrl(FString InUrl);

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "UnlimitedDetail")
	void RefreshPointCloud();

private:
	void LoadPointCloud();
	void UnloadPointCloud();

	UPROPERTY(EditAnywhere, BlueprintGetter = GetUrl, BlueprintSetter = SetUrl, Category = "UnlimitedDetail")
	FString Url;

	struct FUDPointCloudHandle* PointCloudHandle;

protected:
	/** Overridable native event for when play begins for this actor. */
	virtual void BeginPlay() override;
	virtual void PostLoad() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void BeginDestroy() override;

	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
