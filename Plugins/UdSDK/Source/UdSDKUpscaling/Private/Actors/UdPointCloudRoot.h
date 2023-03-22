#pragma once

#include "Components/SceneComponent.h"
#include "CoreMinimal.h"
#include "UdPointCloudRoot.generated.h"


UCLASS(Blueprintable, BlueprintType)
class UUdPointCloudRoot : public USceneComponent
{
	GENERATED_BODY()

public:
	UUdPointCloudRoot();

	virtual void
	ApplyWorldOffset(const FVector& InOffset, bool bWorldShift) override;

protected:
	virtual void BeginPlay() override;
	virtual bool MoveComponentImpl(const FVector& Delta, const FQuat& NewRotation, bool bSweep, FHitResult* OutHit = NULL, EMoveComponentFlags MoveFlags = MOVECOMP_NoFlags, ETeleportType Teleport = ETeleportType::None) override;
};
