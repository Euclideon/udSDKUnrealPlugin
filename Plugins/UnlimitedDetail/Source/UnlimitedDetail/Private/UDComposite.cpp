#include "UDComposite.h"

#include "Subpasses/UdsSubpassFirst.h"
#include "Subpasses/UdsSubpassComposite.h"
#include "Subpasses/UdsSubpassLast.h"

#define EXECUTE_STEP(step) \
	for (FUdsSubpass* Subpass : FUdsubpasses) \
	{ \
		Subpass->step(GraphBuilder, View, PassInputs); \
	}

DECLARE_GPU_STAT(UnlimitedDetailCompositeResolutionPass)

FUDComposite::FUDComposite(EUdsMode InMode, TArray<TSharedPtr<FUdsData>> InViewData) : Mode(InMode) , ViewData(InViewData)
{
	if (Mode != EUdsMode::None)
	{

		// subpasses will run in the order in which they are registered
		// ensure this subpass always runs first
		RegisterSubpass<FUdsSubpassFirst>();
		
		RegisterSubpass<FUdsSubpassComposite>();

		// ensure this subpass always runs last.
		RegisterSubpass<FUdsSubpassLast>();
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EUdsMode == none, upscaler cannot run"));
	}
}

ISpatialUpscaler* FUDComposite::Fork_GameThread(const class FSceneViewFamily& ViewFamily) const
{
	// FSceneTextures::InitializeViewFamily(GraphBuilder, *(FViewFamilyInfo*)View.Family);
	
	// ViewFamily.Views[0]
	//UE_LOG(LogTemp, Warning, TEXT("Game thread fork running"));
	// the object we return here will get deleted by UE4 when the scene view tears down, so we need to instantiate a new one every frame.
	return new FUDComposite(Mode, ViewData);
}

FScreenPassTexture FUDComposite::AddPasses(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs) const
{
	//UE_LOG(LogTemp, Warning, TEXT("Add passes running"));
	
	RDG_GPU_STAT_SCOPE(GraphBuilder, UnlimitedDetailCompositeResolutionPass);
	check(PassInputs.SceneColor.IsValid());

	
	
	TSharedPtr<FUdsData> Data = GetDataForView(View);
	for (FUdsSubpass* Subpass : FUdsubpasses)
	{
		Subpass->SetData(Data.Get());
	}

	if (!Data->bInitialized)
	{
		EXECUTE_STEP(ParseEnvironment);
		EXECUTE_STEP(CreateResources);
	}

	if (Mode == EUdsMode::UpscalingOnly || Mode == EUdsMode::Combined)
	{
		EXECUTE_STEP(Upscale);
	}

	if (Mode == EUdsMode::PostProcessingOnly || Mode == EUdsMode::Combined)
	{
		EXECUTE_STEP(PostProcess);
	}

	FScreenPassTexture FinalOutput = Data->FinalOutput;
	return MoveTemp(FinalOutput);
}

TSharedPtr<FUdsData> FUDComposite::GetDataForView(const FViewInfo& View) const
{
	for (int i = 0; i < View.Family->Views.Num(); i++)
	{
		if (View.Family->Views[i] == &View)
		{
			return ViewData[i];
		}
	}
	return nullptr;
}