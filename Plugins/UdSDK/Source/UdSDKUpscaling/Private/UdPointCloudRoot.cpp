#include "UdPointCloudRoot.h"
#include "Engine/World.h"
#include "UdSDKMacro.h"
#include "UdSDKDefine.h"
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

	FPointCloudSceneProxy(UUdPointCloudRoot* Component) : FPrimitiveSceneProxy(Component)
	{
		myRoot = Component;
		bWillEverBeLit = false;
	}

	virtual ~FPointCloudSceneProxy()
	{
		//TODO: Cleanup here
	}

	// FPrimitiveSceneProxy interface.

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_ArrowSceneProxy_DrawDynamicElements);

		FMatrix EffectiveLocalToWorld = GetLocalToWorld();

		UUDSubsystem* MySubsystem = GEngine->GetEngineSubsystem<UUDSubsystem>();

		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (VisibilityMap & (1 << ViewIndex))
			{
				const FSceneView* View = Views[ViewIndex];

				if (MySubsystem)
					MySubsystem->QueueInstance(myRoot->PointCloudHandle, GetLocalToWorld(), View);
			}
		}
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = IsShown(View) && (View->Family->EngineShowFlags.BillboardSprites);
		Result.bDynamicRelevance = true;

		Result.bShadowRelevance = IsShadowCast(View);
		Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
		Result.bVelocityRelevance = DrawsVelocity() && Result.bOpaque && Result.bRenderInMainPass;
		return Result;
	}

	virtual uint32 GetMemoryFootprint(void) const override { return(sizeof(*this) + GetAllocatedSize()); }
	uint32 GetAllocatedSize(void) const { return(FPrimitiveSceneProxy::GetAllocatedSize()); }

private:
	UUdPointCloudRoot* myRoot = nullptr;
};


UUdPointCloudRoot::UUdPointCloudRoot()
{
	// Intentionally blank
	PointCloudHandle = nullptr;
}

UUdPointCloudRoot::~UUdPointCloudRoot()
{
	DestroyPointCloud();
}

void UUdPointCloudRoot::SetUrl(FString InUrl)
{
	UE_LOG(LogTemp, Warning, TEXT("%s calling!"), TEXT(__FUNCTION__));

	if (InUrl != this->Url)
	{
		Url = InUrl;
		DestroyPointCloud();
	}
}

void UUdPointCloudRoot::RefreshPointCloud()
{
	UE_LOG(LogTemp, Warning, TEXT("%s calling!"), TEXT(__FUNCTION__));
	//UDSDK_INFO_MSG("AUdPointCloud::RefreshPointCloud : %d", GetUniqueID());
	ReloadPointCloud();
}

void UUdPointCloudRoot::LoadPointCloud()
{
	if (PointCloudHandle)
		return;

	UUDSubsystem* MySubsystem = GEngine->GetEngineSubsystem<UUDSubsystem>();

	if (!MySubsystem)
		return;

	if (Url.IsEmpty())
		return;

	PointCloudHandle = MySubsystem->Load(GetUrl());
}

void UUdPointCloudRoot::ReloadPointCloud()
{
	UE_LOG(LogTemp, Warning, TEXT("%s calling..."), TEXT(__FUNCTION__));

	UUDSubsystem* MySubsystem = GEngine->GetEngineSubsystem<UUDSubsystem>();

	if (!MySubsystem->IsLogin())
	{
		UE_LOG(LogTemp, Warning, TEXT("Not duplicated for PIE"));
		return;
	}

	MySubsystem->Remove(&PointCloudHandle);

	if (Url.IsEmpty())
		return;

	PointCloudHandle = MySubsystem->Load(GetUrl());
}

void UUdPointCloudRoot::DestroyPointCloud()
{
	if (!PointCloudHandle)
	{
		return;
	}

	UUDSubsystem* MySubsystem = GEngine->GetEngineSubsystem<UUDSubsystem>();

	MySubsystem->Remove(&PointCloudHandle);
}

void UUdPointCloudRoot::LoginPointCloud()
{
	LoadPointCloud();
}

void UUdPointCloudRoot::BeginPlay()
{
	Super::BeginPlay();

	LoadPointCloud();
}

void UUdPointCloudRoot::BeginDestroy()
{
	Super::BeginDestroy();
	DestroyPointCloud();
}

void UUdPointCloudRoot::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	DestroyPointCloud();
}

void UUdPointCloudRoot::PostLoad()
{
	Super::PostLoad();

	LoadPointCloud();
}


FPrimitiveSceneProxy* UUdPointCloudRoot::CreateSceneProxy()
{
	return new FPointCloudSceneProxy(this);
}


#if WITH_EDITOR
void UUdPointCloudRoot::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property)
	{
		return;
	}

	FName PropName = PropertyChangedEvent.Property->GetFName();
	FString PropNameAsString = PropertyChangedEvent.Property->GetName();
	if (PropName == GET_MEMBER_NAME_CHECKED(UUdPointCloudRoot, Url))
	{
		//UDSDK_INFO_MSG("AUdPointCloud::PostEditChangeProperty Url : %d", GetUniqueID());
		ReloadPointCloud();
	}
}
#endif //WITH_EDITOR
