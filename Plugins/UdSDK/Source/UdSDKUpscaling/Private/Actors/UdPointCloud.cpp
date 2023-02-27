#include "UdPointCloud.h"
#include "UdPointCloudRoot.h"
#include "UdSDKMacro.h"
#include "UdSDKDefine.h"

#include "UdSDKComposite.h"
#include "..\..\Public\Actors\UdPointCloud.h"


AUdPointCloud::AUdPointCloud()
{
	//UDSDK_INFO_MSG("AUdPointCloud::AUdPointCloud : %d", GetUniqueID());
	PrimaryActorTick.bCanEverTick = true;
	bWasDuplicatedForPIE = false;
	bWasHiddenEd = false;
	bWasSelectedInEditor = false;
#if WITH_EDITOR
	bWasHiddenEd = IsHiddenEd();
	bWasSelectedInEditor = IsSelectedInEditor();
#endif //WITH_EDITOR
	RootComponent = CreateDefaultSubobject<UUdPointCloudRoot>(TEXT("PointCloud"));
	RootComponent->SetMobility(EComponentMobility::Static);
}

AUdPointCloud::~AUdPointCloud()
{
	DestroyPointCloud();
}

void AUdPointCloud::UpdatePointCloudTransform(const FTransform& InTransform)
{
	UE_LOG(LogTemp, Warning, TEXT("%s calling!"), TEXT(__FUNCTION__));

	if (bWasDuplicatedForPIE)
	{
		return;
	}

	if (!CUdSDKComposite::Get()->IsLogin())
	{
		return;
	}

	CUdSDKComposite::Get()->AsyncSetTransform(GetUniqueID(), InTransform);
}

void AUdPointCloud::SetUrl(FString InUrl)
{
	UE_LOG(LogTemp, Warning, TEXT("%s calling!"), TEXT(__FUNCTION__));

	if (InUrl != this->Url)
	{
		Url = InUrl;
		DestroyPointCloud();
	}
}

void AUdPointCloud::RefreshPointCloud()
{
	UE_LOG(LogTemp, Warning, TEXT("%s calling!"), TEXT(__FUNCTION__));
	//UDSDK_INFO_MSG("AUdPointCloud::RefreshPointCloud : %d", GetUniqueID());
	ReloadPointCloud();
}

void AUdPointCloud::BeginPlay()
{
	LoadPointCloud();
	//UDSDK_SCREENDE_DEBUG_MSG("AUdPointCloud::BeginPlay : %d", GetUniqueID());
}

void AUdPointCloud::OnConstruction(const FTransform& Transform)
{
	//UDSDK_SCREENDE_DEBUG_MSG("AUdPointCloud::OnConstruction : %d", GetUniqueID());
}

bool AUdPointCloud::ShouldTickIfViewportsOnly() const
{
	//UDSDK_INFO_MSG("AUdPointCloud::ShouldTickIfViewportsOnly : %d", GetUniqueID());
	return UpdateInEditor;
}

void AUdPointCloud::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (SuspendUpdate) {
	//	return;
	//}

#if WITH_EDITOR
	if (bWasHiddenEd != IsHiddenEd())
	{
		if (IsHiddenEd())
		{
			DestroyPointCloud();
		}
		else
		{
			LoadPointCloud();
		}
		bWasHiddenEd = IsHiddenEd();
	}

	if (bWasSelectedInEditor != IsSelectedInEditor())
	{
		SelectPointCloud(IsSelectedInEditor());

		bWasSelectedInEditor = IsSelectedInEditor();
	}
#endif //WITH_EDITOR

	//if (!pAsset.Get())
	//{
	//	LoadPointCloud();
	//}
}

void AUdPointCloud::BeginDestroy()
{
	//UDSDK_SCREENDE_DEBUG_MSG("AUdPointCloud::BeginDestroy : %d", GetUniqueID());

	if (!bWasDuplicatedForPIE)
	{
		CUdSDKComposite::Get()->LoginDelegate.Remove(LoginDelegateHandle);
		CUdSDKComposite::Get()->ExitFrontDelegate.Remove(ExitDelegateHandle);
	}

	AActor::BeginDestroy();
}

void AUdPointCloud::Destroyed()
{
	//UDSDK_SCREENDE_DEBUG_MSG("AUdPointCloud::Destroyed : %d", GetUniqueID());
	DestroyPointCloud();

	AActor::Destroyed();
}

void AUdPointCloud::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DestroyPointCloud();
	//UDSDK_SCREENDE_DEBUG_MSG("AUdPointCloud::EndPlay : %d", GetUniqueID());
	AActor::EndPlay(EndPlayReason);
}

void AUdPointCloud::PostLoad()
{
	//UDSDK_SCREENDE_DEBUG_MSG("AUdPointCloud::PostLoad : %d", GetUniqueID());
	//UDSDK_INFO_MSG("AUdPointCloud::PostLoad : %d->(%s)", GetUniqueID(), *RootComponent->GetRelativeTransform().GetLocation().ToString());
	Super::PostLoad();
	if (!bWasDuplicatedForPIE)
	{
		LoginDelegateHandle = CUdSDKComposite::Get()->LoginDelegate.AddUObject(this, &AUdPointCloud::LoginPointCloud);
		ExitDelegateHandle = CUdSDKComposite::Get()->ExitFrontDelegate.AddUObject(this, &AUdPointCloud::ExitPointCloud);
		//UDSDK_SCREENDE_DEBUG_MSG("%d->bWasDuplicatedForPIE : %d", GetUniqueID(), bWasDuplicatedForPIE);
	}
	LoadPointCloud();
}

void AUdPointCloud::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	//UDSDK_INFO_MSG("AUdPointCloud::Serialize : %d, IsHidden : %d, IsHiddenEd : %d, IsSelectedInEditor : %d", GetUniqueID(), IsHidden(), IsHiddenEd(), IsSelectedInEditor());
}

#if WITH_EDITOR
void AUdPointCloud::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property)
	{
		return;
	}

	FName PropName = PropertyChangedEvent.Property->GetFName();
	FString PropNameAsString = PropertyChangedEvent.Property->GetName();
	if (
		PropName == GET_MEMBER_NAME_CHECKED(AUdPointCloud, Url)
	)
	{
		//UDSDK_INFO_MSG("AUdPointCloud::PostEditChangeProperty Url : %d", GetUniqueID());
		ReloadPointCloud();
	}
}

void AUdPointCloud::PostEditUndo()
{
	Super::PostEditUndo();
}

void AUdPointCloud::PostEditImport()
{
	Super::PostEditImport();
}

#endif //WITH_EDITOR


void AUdPointCloud::PreDuplicate(FObjectDuplicationParameters& DupParams)
{
	Super::PreDuplicate(DupParams);
	//UDSDK_INFO_MSG("AUdPointCloud::PreDuplicate : %d", GetUniqueID());
}

void AUdPointCloud::PostDuplicate(bool bDuplicateForPIE)
{
	Super::PostDuplicate(bDuplicateForPIE);
	bWasDuplicatedForPIE = bDuplicateForPIE;
	//UDSDK_INFO_MSG("AUdPointCloud::PostDuplicate : %d , DuplicateForPIE : %d", GetUniqueID(), bDuplicateForPIE);
}

void AUdPointCloud::LoadPointCloud()
{
	if (bWasDuplicatedForPIE)
		return;
	if (pAsset.Get())
		return;
	if (!CUdSDKComposite::Get()->IsLogin())
		return;
	if (Url.IsEmpty())
		return;
	const FTransform& Transform = RootComponent->GetRelativeTransform();

	pAsset = MakeShared<FUdAsset>(FUdAsset());
	pAsset->url = GetUrl();
	pAsset->coords = Transform.GetLocation();
	pAsset->geometry = true;
	CUdSDKComposite::Get()->AsyncLoad(GetUniqueID(), pAsset, [this]
	{
		// TODO
		// BUG #1 Causes an editor exception error when switching scenes partway through a load in the editor
		// BUG #2 Causes an exception if the user rapidly switches menus before Ud has had time to load
		// Repro steps: Click login, then switch to a new scene, then switch back
		const FTransform& Transform = RootComponent->GetRelativeTransform();
		CUdSDKComposite::Get()->AsyncSetTransform(GetUniqueID(), Transform);
#if WITH_EDITOR
		// TODO - BUG - throws an error if an object is selected while switching maps
		CUdSDKComposite::Get()->AsyncSetSelected(GetUniqueID(), IsSelectedInEditor());
#endif //WITH_EDITOR
	});
}

void AUdPointCloud::ReloadPointCloud()
{
	UE_LOG(LogTemp, Warning, TEXT("%s calling..."), TEXT(__FUNCTION__));

	if (bWasDuplicatedForPIE)
	{
		UE_LOG(LogTemp, Warning, TEXT("Is duplicated for PIE!"));
		return;
	}

	if (!CUdSDKComposite::Get()->IsLogin())
	{
		UE_LOG(LogTemp, Warning, TEXT("Not duplicated for PIE"));
		return;
	}

	CUdSDKComposite::Get()->AsyncRemove(GetUniqueID(), [this]
	{
		if (Url.IsEmpty())
			return;
		const FTransform& Transform = RootComponent->GetRelativeTransform();
		pAsset = nullptr;
		pAsset = MakeShared<FUdAsset>(FUdAsset());
		pAsset->url = GetUrl();
		pAsset->coords = Transform.GetLocation();
		pAsset->geometry = true;
		CUdSDKComposite::Get()->AsyncLoad(GetUniqueID(), pAsset, [this]
		{
			const FTransform& Transform = RootComponent->GetRelativeTransform();
			CUdSDKComposite::Get()->AsyncSetTransform(GetUniqueID(), Transform);
#if WITH_EDITOR
			CUdSDKComposite::Get()->AsyncSetSelected(GetUniqueID(), IsSelectedInEditor());
#endif //WITH_EDITOR
		});
	});
}

void AUdPointCloud::DestroyPointCloud()
{
	// Whyh do we return early for PIE duplicates?
	if (bWasDuplicatedForPIE)
	{
		return;
	}

	if (!pAsset.Get())
	{
		return;
	}

	CUdSDKComposite::Get()->AsyncRemove(GetUniqueID(), [this]
	{
		pAsset = nullptr;
	});
}

void AUdPointCloud::SelectPointCloud(bool InSelect)
{
	if (bWasDuplicatedForPIE)
		return;
	if (!pAsset.Get())
		return;

	CUdSDKComposite::Get()->AsyncSetSelected(GetUniqueID(), InSelect);
}

void AUdPointCloud::LoginPointCloud()
{
	//UDSDK_INFO_MSG("AUdPointCloud::LoginPointCloud : %d", GetUniqueID());
	LoadPointCloud();
}

void AUdPointCloud::ExitPointCloud()
{
	DestroyPointCloud();
}
