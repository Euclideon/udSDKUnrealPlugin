#include "UdPointCloudRoot.h"
#include "Actors/UdPointCloud.h"
#include "Engine/World.h"
#include "UdSDKMacro.h"


UUdPointCloudRoot::UUdPointCloudRoot()
{
	//PrimaryComponentTick.bCanEverTick = false;
	SetUsingAbsoluteLocation(true);
	// UDSDK_INFO_MSG("UUdPointCloudRoot::UUdPointCloudRoot : %d", GetUniqueID());
}

void UUdPointCloudRoot::ApplyWorldOffset(
	const FVector& InOffset,
	bool bWorldShift)
{
	USceneComponent::ApplyWorldOffset(InOffset, bWorldShift);

	//const FIntVector& oldOrigin = this->GetWorld()->OriginLocation;

	//UDSDK_INFO_MSG("UUdPointCloudRoot::ApplyWorldOffset : %d,%s,%s", GetUniqueID(), *InOffset.ToString(), *oldOrigin.ToString());
}


void UUdPointCloudRoot::BeginPlay()
{
	Super::BeginPlay();
	//UDSDK_INFO_MSG("UUdPointCloudRoot::BeginPlay : %d", GetUniqueID());
}

bool UUdPointCloudRoot::MoveComponentImpl(
	const FVector& Delta, const FQuat& NewRotation, bool bSweep, FHitResult* OutHit, EMoveComponentFlags MoveFlags,
	ETeleportType Teleport)
{
	bool result = USceneComponent::MoveComponentImpl(Delta, NewRotation, bSweep, OutHit, MoveFlags, Teleport);

	if (IsUsingAbsoluteLocation())
	{
		// UE_LOG(LogTemp, Warning, TEXT("%s: Using absolute location."), TEXT(__FUNCTION__));

		if (AUdPointCloud* pPointCloud = GetOwner<AUdPointCloud>())
		{
			//const FVector& newLocation = GetRelativeLocation();
			//const FIntVector& originLocation = GetWorld()->OriginLocation;
			//const FVector& newScale = GetRelativeScale3D();
			//
			//Transform.SetLocation(FVector(originLocation) + newLocation);
			//Transform.SetScale3D(newScale);
			//Transform.SetRotation(NewRotation);
			pPointCloud->UpdatePointCloudTransform(GetRelativeTransform());
			//UDSDK_INFO_MSG("UUdPointCloudRoot::MoveComponentImpl : newLocation %s", *newLocation.ToString());
			//UDSDK_INFO_MSG("UUdPointCloudRoot::MoveComponentImpl : originLocation %s", *originLocation.ToString());
		}
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MoveComponentImpl: Not using absolute location.")); // refactor 
	}


	return result;
}
