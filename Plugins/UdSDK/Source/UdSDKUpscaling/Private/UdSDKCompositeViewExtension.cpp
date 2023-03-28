#include "UdSDKCompositeViewExtension.h"
#include "UDSubsystem.h"
#include "GlobalShader.h"
#include "SceneView.h"
#include "PixelShaderUtils.h"

#include "UdSDKCompositeUpscaler.h"
#include "PostProcess/SceneRenderTargets.h"

FUdSDKCompositeViewExtension::FUdSDKCompositeViewExtension(const FAutoRegister& AutoRegister) :
	FSceneViewExtensionBase(AutoRegister)
{	

}


void FUdSDKCompositeViewExtension::SetupViewFamily(FSceneViewFamily &InViewFamily)
{
	// Required to be implemented by abstract parent class
}

void FUdSDKCompositeViewExtension::SetupView(FSceneViewFamily &InViewFamily, FSceneView &InView)
{
	// Required to be implemented by abstract parent class
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

	if (InViewFamily.GetFeatureLevel() >= ERHIFeatureLevel::SM5)
	{
		TArray<TSharedPtr<FUdsData>> ViewData;

		for (int i = 0; i < InViewFamily.Views.Num(); i++)
		{
			const FSceneView* InView = InViewFamily.Views[i];

			if (ensure(InView))
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
