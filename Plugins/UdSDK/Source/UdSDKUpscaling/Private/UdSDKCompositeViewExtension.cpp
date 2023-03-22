#include "UdSDKCompositeViewExtension.h"
#include "UDSubsystem.h"
#include "GlobalShader.h"
#include "SceneView.h"
#include "PixelShaderUtils.h"

#include "UdSDKCompositeUpscaler.h"
#include "PostProcess/SceneRenderTargets.h"


static TAutoConsoleVariable<int32> CVarEnableUds(
	TEXT("r.Uds.Enabled"),
	1,
	TEXT("Uds Enable"),
	ECVF_RenderThreadSafe);


FUdSDKCompositeViewExtension::FUdSDKCompositeViewExtension(const FAutoRegister& AutoRegister) :
	FSceneViewExtensionBase(AutoRegister)
{	

}


void FUdSDKCompositeViewExtension::SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView)
{
	UUDSubsystem* MySubsystem = GEngine->GetEngineSubsystem<UUDSubsystem>();

	if (MySubsystem)
	{
		UE_LOG(LogTemp, Display, TEXT("Setting up view: %f,%f,%f"), InView.ViewLocation.X, InView.ViewLocation.Y, InView.ViewLocation.Z);
		MySubsystem->ResetForNextViewport(InView);
	}
}

void FUdSDKCompositeViewExtension::BeginRenderViewFamily(FSceneViewFamily& InViewFamily)
{
	// TODO - It would be ideal if this function could correctly marshal the true depth buffer width/height down into the CaptureUDSImage call below
	// I'm unsure of how to do that right now, so for now these values are being cached into a UDSDK engine singleton
	// but this really should be refactored at a later date.

	UUDSubsystem* MySubsystem = GEngine->GetEngineSubsystem<UUDSubsystem>();

	if (!MySubsystem ||	InViewFamily.Views.Num() == 0)
	{
		return;
	}

	if (InViewFamily.GetFeatureLevel() >= ERHIFeatureLevel::SM5 && CVarEnableUds.GetValueOnAnyThread() > 0)
	{
		TArray<TSharedPtr<FUdsData>> ViewData;

		for (int i = 0; i < InViewFamily.Views.Num(); i++)
		{
			const FSceneView* InView = InViewFamily.Views[i];

			if (ensure(InView))
			{
				uint32 ViewIndex0 = 0;
				uint32 ViewIndex1 = 1;
				uint64 EditorViewBitflag0 = (uint64)1 << ViewIndex0;
				uint64 EditorViewBitflag1 = (uint64)1 << ViewIndex1;
#if WITH_EDITOR
				EditorViewBitflag0 = InView->SceneViewInitOptions.EditorViewBitflag;
				EditorViewBitflag1 = InView->SceneViewInitOptions.EditorViewBitflag;
#endif
				if (EditorViewBitflag0 == (uint64)1 << ViewIndex0 || EditorViewBitflag1 == (uint64)1 << ViewIndex1)
				{
					FUdsData* Data = new FUdsData();
					MySubsystem->CaptureUDSImage(*InView);
					Data->UdColorTexture = MySubsystem->GetColorTexture();
					Data->UdDepthTexture = MySubsystem->GetDepthTexture();

					ViewData.Add(TSharedPtr<FUdsData>(Data));

					if (MySubsystem->IsValid())
						InViewFamily.SetSecondarySpatialUpscalerInterface(new FUdSDKCompositeUpscaler(EUdsMode::PostProcessingOnly, ViewData));
				}
			}
		}
	}
}
//PRAGMA_ENABLE_OPTIMIZATION
void FUdSDKCompositeViewExtension::PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView)
{

}

void FUdSDKCompositeViewExtension::PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessingInputs& Inputs)
{

}