#include "UDComponent.h"
#include "Engine/World.h"
#include "UDDefine.h"
#include "UDSubsystem.h"

/** Represents a UArrowComponent to the scene manager. */
class FPointCloudSceneProxy final : public FPrimitiveSceneProxy
{
public:
	SIZE_T GetTypeHash() const override
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}

	FPointCloudSceneProxy(UUDComponent* Component) : FPrimitiveSceneProxy(Component)
	{
		myRoot = Component;
		bWillEverBeLit = false;
		instance = -1;
		bShouldNotifyOnWorldAddRemove = true;
	}

	virtual ~FPointCloudSceneProxy()
	{
		//TODO: Cleanup here
		if (instance != -1)
		{
			UUDSubsystem *MySubsystem = GEngine->GetEngineSubsystem<UUDSubsystem>();
			MySubsystem->RemoveInstance(instance);
			instance = -1;
		}
	}

	// FPrimitiveSceneProxy interface.

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_ArrowSceneProxy_DrawDynamicElements);

		if (instance == -1)
		{
			// This Log shouldn't be required but it's helping to track down an issue that happens occasionally
			UE_LOG(LogTemp, Warning, TEXT("UnlimitedDetail | Getting dynamic mesh elements without mounted instance."));
		}
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = IsShown(View);
		Result.bDynamicRelevance = true;

		Result.bShadowRelevance = false; //TODO: Consider shadows support
		Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
		Result.bVelocityRelevance = false;

		return Result;
	}

	virtual bool OnLevelAddedToWorld_RenderThread() override
	{
		check(instance == -1);

		UUDSubsystem *MySubsystem = GEngine->GetEngineSubsystem<UUDSubsystem>();
		instance = MySubsystem->QueueInstance(myRoot->PointCloudHandle, GetLocalToWorld(), &GetScene());

		SetForceHidden(false);
		return false;
	}

	virtual void OnLevelRemovedFromWorld_RenderThread() override
	{
		check(instance != -1);

		UUDSubsystem *MySubsystem = GEngine->GetEngineSubsystem<UUDSubsystem>();
		MySubsystem->RemoveInstance(instance);
		instance = -1;
		SetForceHidden(true);
	}

	virtual void OnTransformChanged() override
	{
		UE_LOG(LogTemp, Display, TEXT("UnlimitedDetail | UDS Transform Updated"));
		UUDSubsystem *MySubsystem = GEngine->GetEngineSubsystem<UUDSubsystem>();

		if (instance == -1)
		{
			instance = MySubsystem->QueueInstance(myRoot->PointCloudHandle, GetLocalToWorld(), &GetScene());
		}
		else
		{
			MySubsystem->UpdateInstance(instance, GetLocalToWorld());
		}
	}

	virtual uint32 GetMemoryFootprint(void) const override
	{
		return sizeof(*this) + GetAllocatedSize();
	}

	uint32 GetAllocatedSize(void) const
	{
		return FPrimitiveSceneProxy::GetAllocatedSize();
	}

private:
	UUDComponent* myRoot = nullptr;
	int64_t instance; //TODO: Find we need multiple of these
};


UUDComponent::UUDComponent()
{
	// Intentionally blank
	PointCloudHandle = nullptr;
	Url = "";
}

void UUDComponent::SetUrl(FString InUrl)
{
	if (InUrl != this->Url)
	{
		Url = InUrl;
		UnloadPointCloud();
		LoadPointCloud();
	}
}

void UUDComponent::RefreshPointCloud()
{
	UnloadPointCloud();
	LoadPointCloud();
}

void UUDComponent::LoadPointCloud()
{
	if (PointCloudHandle)
		return;

	UUDSubsystem* MySubsystem = GEngine->GetEngineSubsystem<UUDSubsystem>();

	if (!MySubsystem)
		return;

	if (Url.IsEmpty())
		return;

	 FUDPointCloudHandle* PCI = MySubsystem->Load(GetUrl());
	 PointCloudHandle = PCI;

	 UE_LOG(LogTemp, Display, TEXT("UnlimitedDetail | Component %s | Load PCI | %p | %s"), *GetName(), PointCloudHandle, *PointCloudHandle->URL);
}

void UUDComponent::UnloadPointCloud()
{
	if (!PointCloudHandle)
		return;

	UUDSubsystem* MySubsystem = GEngine->GetEngineSubsystem<UUDSubsystem>();
	
	UE_LOG(LogTemp, Display, TEXT("UnlimitedDetail | Component %s | Unload PCI | %p | %s"), *GetName(), PointCloudHandle, *PointCloudHandle->URL);

	MySubsystem->Remove(PointCloudHandle);
	PointCloudHandle = nullptr;
}

void UUDComponent::BeginPlay()
{
	Super::BeginPlay();

	LoadPointCloud();
}

void UUDComponent::BeginDestroy()
{
	if (PointCloudHandle)
		UnloadPointCloud();

	Super::BeginDestroy();
}

void UUDComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PointCloudHandle)
		UnloadPointCloud();

	Super::EndPlay(EndPlayReason);
}

void UUDComponent::PostLoad()
{
	Super::PostLoad();

	LoadPointCloud();
}

FPrimitiveSceneProxy* UUDComponent::CreateSceneProxy()
{
	return new FPointCloudSceneProxy(this);
}

#if WITH_EDITOR
void UUDComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property)
	{
		return;
	}

	FName PropName = PropertyChangedEvent.Property->GetFName();
	FString PropNameAsString = PropertyChangedEvent.Property->GetName();
	if (PropName == GET_MEMBER_NAME_CHECKED(UUDComponent, Url))
	{
		UnloadPointCloud();
		LoadPointCloud();
	}
}
#endif //WITH_EDITOR
