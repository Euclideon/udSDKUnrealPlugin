#include "UdPointCloudRoot.h"
#include "Engine/World.h"
#include "UdSDKMacro.h"
#include "UdSDKDefine.h"
#include "UdSDKComposite.h"

#define DEFAULT_SCREEN_SIZE	(0.0025f)
#define ARROW_SCALE			(80.0f)
#define ARROW_RADIUS_FACTOR	(0.03f)
#define ARROW_HEAD_FACTOR	(0.2f)
#define ARROW_HEAD_ANGLE	(20.f)

/** Represents a UArrowComponent to the scene manager. */
class FPointcloudSceneProxy final : public FPrimitiveSceneProxy
{
public:
	SIZE_T GetTypeHash() const override
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}

	FPointcloudSceneProxy(UUdPointCloudRoot* Component)
		: FPrimitiveSceneProxy(Component)
		, VertexFactory(GetScene().GetFeatureLevel(), "FArrowSceneProxy")
		, myRoot(Component)
		, ArrowColor(FColor::Green)
		, ArrowSize(50)
		, ArrowLength(80)
		, bIsScreenSizeScaled(false)
		, ScreenSize(0.0025)
#if WITH_EDITORONLY_DATA
		, bLightAttachment(false)
		, bTreatAsASprite(false)
		, bUseInEditorScaling(true)
		, EditorScale(true)
#endif
	{
		bWillEverBeLit = false;
#if WITH_EDITOR
		// If in the editor, extract the sprite category from the component
		if (GIsEditor)
		{
			SpriteCategoryIndex = GEngine->GetSpriteCategoryIndex(TEXT("Misc"));
		}
#endif	//WITH_EDITOR

		const float HeadAngle = FMath::DegreesToRadians(ARROW_HEAD_ANGLE);
		const float DefaultLength = ArrowSize * ARROW_SCALE;
		const float TotalLength = ArrowSize * ArrowLength;
		const float HeadLength = DefaultLength * ARROW_HEAD_FACTOR;
		const float ShaftRadius = DefaultLength * ARROW_RADIUS_FACTOR;
		const float ShaftLength = (TotalLength - HeadLength * 0.5); // 10% overlap between shaft and head
		const FVector ShaftCenter = FVector(0.5f * ShaftLength, 0, 0);

		TArray<FDynamicMeshVertex> OutVerts;
		BuildConeVerts(HeadAngle, HeadAngle, -HeadLength, TotalLength, 32, OutVerts, IndexBuffer.Indices);
		
		VertexBuffers.InitFromDynamicVertex(&VertexFactory, OutVerts);

		// Enqueue initialization of render resource
		BeginInitResource(&IndexBuffer);
	}

	virtual ~FPointcloudSceneProxy()
	{
		VertexBuffers.PositionVertexBuffer.ReleaseResource();
		VertexBuffers.StaticMeshVertexBuffer.ReleaseResource();
		VertexBuffers.ColorVertexBuffer.ReleaseResource();
		IndexBuffer.ReleaseResource();
		VertexFactory.ReleaseResource();
	}

	// FPrimitiveSceneProxy interface.

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_ArrowSceneProxy_DrawDynamicElements);

		FMatrix EffectiveLocalToWorld;
#if WITH_EDITOR
		if (bLightAttachment)
		{
			EffectiveLocalToWorld = GetLocalToWorld().GetMatrixWithoutScale();
		}
		else
#endif	//WITH_EDITOR
		{
			EffectiveLocalToWorld = GetLocalToWorld();
		}

		auto ArrowMaterialRenderProxy = new FColoredMaterialRenderProxy(
			GEngine->ArrowMaterial->GetRenderProxy(),
			ArrowColor,
			"GizmoColor"
		);

		Collector.RegisterOneFrameMaterialProxy(ArrowMaterialRenderProxy);

		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (VisibilityMap & (1 << ViewIndex))
			{
				const FSceneView* View = Views[ViewIndex];

				// Calculate the view-dependent scaling factor.
				float ViewScale = 1.0f;
				if (bIsScreenSizeScaled && (View->ViewMatrices.GetProjectionMatrix().M[3][3] != 1.0f))
				{
					const float ZoomFactor = FMath::Min<float>(View->ViewMatrices.GetProjectionMatrix().M[0][0], View->ViewMatrices.GetProjectionMatrix().M[1][1]);
					if (ZoomFactor != 0.0f)
					{
						// Note: we can't just ignore the perspective scaling here if the object's origin is behind the camera, so preserve the scale minus its sign.
						const float Radius = FMath::Abs(View->WorldToScreen(Origin).W * (ScreenSize / ZoomFactor));
						if (Radius < 1.0f)
						{
							ViewScale *= Radius;
						}
					}
				}

#if WITH_EDITORONLY_DATA
				ViewScale *= EditorScale;
#endif

				// Draw the mesh.
				FMeshBatch& Mesh = Collector.AllocateMesh();
				FMeshBatchElement& BatchElement = Mesh.Elements[0];
				BatchElement.IndexBuffer = &IndexBuffer;
				Mesh.bWireframe = false;
				Mesh.VertexFactory = &VertexFactory;
				Mesh.MaterialRenderProxy = ArrowMaterialRenderProxy;

				FDynamicPrimitiveUniformBuffer& DynamicPrimitiveUniformBuffer = Collector.AllocateOneFrameResource<FDynamicPrimitiveUniformBuffer>();
				DynamicPrimitiveUniformBuffer.Set(FScaleMatrix(ViewScale) * EffectiveLocalToWorld, FScaleMatrix(ViewScale) * EffectiveLocalToWorld, GetBounds(), GetLocalBounds(), true, false, AlwaysHasVelocity());
				BatchElement.PrimitiveUniformBufferResource = &DynamicPrimitiveUniformBuffer.UniformBuffer;

				BatchElement.FirstIndex = 0;
				BatchElement.NumPrimitives = IndexBuffer.Indices.Num() / 3;
				BatchElement.MinVertexIndex = 0;
				BatchElement.MaxVertexIndex = VertexBuffers.PositionVertexBuffer.GetNumVertices() - 1;
				Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
				Mesh.Type = PT_TriangleList;
				Mesh.DepthPriorityGroup = SDPG_World;
				Mesh.bCanApplyViewModeOverrides = false;
				Collector.AddMesh(ViewIndex, Mesh);
			}
		}
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = IsShown(View) && (View->Family->EngineShowFlags.BillboardSprites);
		Result.bDynamicRelevance = true;
#if WITH_EDITOR
		if (bTreatAsASprite)
		{
			if (GIsEditor && SpriteCategoryIndex != INDEX_NONE && SpriteCategoryIndex < View->SpriteCategoryVisibility.Num() && !View->SpriteCategoryVisibility[SpriteCategoryIndex])
			{
				Result.bDrawRelevance = false;
			}
		}
#endif
		Result.bShadowRelevance = IsShadowCast(View);
		Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
		Result.bVelocityRelevance = DrawsVelocity() && Result.bOpaque && Result.bRenderInMainPass;
		return Result;
	}

	virtual void OnTransformChanged() override
	{
		Origin = GetLocalToWorld().GetOrigin();

		myRoot->UpdatePointCloudTransform(GetLocalToWorld());
	}

	virtual uint32 GetMemoryFootprint(void) const override { return(sizeof(*this) + GetAllocatedSize()); }
	uint32 GetAllocatedSize(void) const { return(FPrimitiveSceneProxy::GetAllocatedSize()); }

private:
	FStaticMeshVertexBuffers VertexBuffers;
	FDynamicMeshIndexBuffer32 IndexBuffer;
	FLocalVertexFactory VertexFactory;

	UUdPointCloudRoot* myRoot = nullptr;

	FVector Origin;
	FColor ArrowColor;
	float ArrowSize;
	float ArrowLength;
	bool bIsScreenSizeScaled;
	float ScreenSize;
#if WITH_EDITORONLY_DATA
	bool bLightAttachment;
	bool bTreatAsASprite;
	int32 SpriteCategoryIndex;
	bool bUseInEditorScaling;
	float EditorScale;
#endif // #if WITH_EDITORONLY_DATA
};


UUdPointCloudRoot::UUdPointCloudRoot()
{
	//PrimaryComponentTick.bCanEverTick = false;
	// UDSDK_INFO_MSG("UUdPointCloudRoot::UUdPointCloudRoot : %d", GetUniqueID());

	//UDSDK_INFO_MSG("AUdPointCloud::AUdPointCloud : %d", GetUniqueID());
	//bWasDuplicatedForPIE = false;
	//bWasHiddenEd = false;
	//bWasSelectedInEditor = false;

#if WITH_EDITOR
	//bWasHiddenEd = IsHiddenEd();
	//bWasSelectedInEditor = IsSelectedInEditor();
#endif //WITH_EDITOR
}

UUdPointCloudRoot::~UUdPointCloudRoot()
{
	DestroyPointCloud();
}


void UUdPointCloudRoot::UpdatePointCloudTransform(const FMatrix& InMatrix)
{
	UE_LOG(LogTemp, Warning, TEXT("%s calling!"), TEXT(__FUNCTION__));

	if (!CUdSDKComposite::Get()->IsLogin())
	{
		return;
	}

	CUdSDKComposite::Get()->AsyncSetTransform(GetUniqueID(), InMatrix);
}


void UUdPointCloudRoot::ApplyWorldOffset(const FVector& InOffset, bool bWorldShift)
{
	Super::ApplyWorldOffset(InOffset, bWorldShift);

	//const FIntVector& oldOrigin = this->GetWorld()->OriginLocation;
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
	if (pAsset.Get())
		return;

	if (!CUdSDKComposite::Get()->IsLogin())
		return;

	if (Url.IsEmpty())
		return;

	const FTransform& Transform = GetRelativeTransform();

	pAsset = MakeShared<FUdAsset>(FUdAsset());
	pAsset->url = GetUrl();
	pAsset->coords = Transform.GetLocation();
	pAsset->geometry = true;

	CUdSDKComposite::Get()->AsyncLoad(GetUniqueID(), pAsset, [this]
	{
		// TODO
		// BUG #1 Causes an editor exception error when switching scenes partway through a load in the editor
		// BUG #2 Causes an exception if the user rapidly switches menus before Ud has had time to load
		// Repro steps: Click login, then switch to a new scene, then switch back OR click login then switch to new scene

		if (IsValid(this)) // At this point, I'm not even sure checking Isvalid(this) is safe beacuse this actior could be entirely unloaded by the time this function calls
		{
			UE_LOG(LogTemp, Warning, TEXT("This UDPointCloud is valid"));
		}

		else
		{
			UE_LOG(LogTemp, Warning, TEXT("This UDPointCloud is	 NOT valid"));
		}

		CUdSDKComposite::Get()->AsyncSetTransform(GetUniqueID(), GetComponentToWorld().ToMatrixWithScale());

#if WITH_EDITOR
		// TODO - BUG - throws an error if an object is selected while switching maps
		CUdSDKComposite::Get()->AsyncSetSelected(GetUniqueID(), IsSelectedInEditor());
#endif //WITH_EDITOR
	});
}

void UUdPointCloudRoot::ReloadPointCloud()
{
	UE_LOG(LogTemp, Warning, TEXT("%s calling..."), TEXT(__FUNCTION__));

	if (!CUdSDKComposite::Get()->IsLogin())
	{
		UE_LOG(LogTemp, Warning, TEXT("Not duplicated for PIE"));
		return;
	}

	CUdSDKComposite::Get()->AsyncRemove(GetUniqueID(), [this]
	{
		if (Url.IsEmpty())
			return;

		const FTransform& Transform = GetRelativeTransform();
		pAsset = nullptr;
		pAsset = MakeShared<FUdAsset>(FUdAsset());
		pAsset->url = GetUrl();
		pAsset->coords = Transform.GetLocation();
		pAsset->geometry = true;

		CUdSDKComposite::Get()->AsyncLoad(GetUniqueID(), pAsset, [this]
		{
			CUdSDKComposite::Get()->AsyncSetTransform(GetUniqueID(), GetComponentToWorld().ToMatrixWithScale());
#if WITH_EDITOR
			CUdSDKComposite::Get()->AsyncSetSelected(GetUniqueID(), IsSelectedInEditor());
#endif //WITH_EDITOR
		});
	});
}

void UUdPointCloudRoot::DestroyPointCloud()
{
	if (!pAsset.Get())
	{
		return;
	}

	CUdSDKComposite::Get()->AsyncRemove(GetUniqueID(), [this] {
		pAsset = nullptr;
	});
}

void UUdPointCloudRoot::SelectPointCloud(bool InSelect)
{
	if (!pAsset.Get())
		return;

	CUdSDKComposite::Get()->AsyncSetSelected(GetUniqueID(), InSelect);
}

void UUdPointCloudRoot::LoginPointCloud()
{
	LoadPointCloud();
}

void UUdPointCloudRoot::ExitPointCloud()
{
	DestroyPointCloud();
}


void UUdPointCloudRoot::BeginPlay()
{
	Super::BeginPlay();

	LoadPointCloud();
}

bool UUdPointCloudRoot::MoveComponentImpl(const FVector& Delta, const FQuat& NewRotation, bool bSweep, FHitResult* OutHit, EMoveComponentFlags MoveFlags, ETeleportType Teleport)
{
	bool result = Super::MoveComponentImpl(Delta, NewRotation, bSweep, OutHit, MoveFlags, Teleport);

	UpdatePointCloudTransform(GetComponentToWorld().ToMatrixWithScale());

	return result;
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
	return new FPointcloudSceneProxy(this);
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
