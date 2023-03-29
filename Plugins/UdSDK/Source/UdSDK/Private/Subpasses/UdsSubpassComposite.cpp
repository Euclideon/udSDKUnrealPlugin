#include "UdsSubpassComposite.h"
#include "UDSubsystem.h"
#include "EdGraphSchema_K2_Actions.h"
#include "ExternalTexture.h"

#include "Runtime/Renderer/Private/SceneRendering.h"

static int32 GUdsComposite = 1;
static FAutoConsoleVariableRef CVarUdsComposite(
	TEXT("r.Uds.Composite.Enabled"),
	GUdsComposite,
	TEXT("Uds Composite Enabled = 1 or 0"),
	ECVF_RenderThreadSafe);


class FSceneRenderTargets;

///
/// PIXEL SHADER
///
class FUdsCompositePS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FUdsCompositePS);
	SHADER_USE_PARAMETER_STRUCT(FUdsCompositePS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_INCLUDE(FCompositePassParameters, Composite)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
	}
};

	IMPLEMENT_GLOBAL_SHADER(FUdsCompositePS, "/Plugins/UdSDK/Private/Uds_Composite.usf", "MainPS", SF_Pixel);

void FUdsSubpassComposite::ParseEnvironment(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs)
{
	Data->bEnabled = GUdsComposite > 0;// && Data->UdColorTexture&& Data->UdDepthTexture;
}

// Create resources is primarily used to prep all the textures for the post processing event
// The scene depth texture can be invalid in the first few frames of the post processing execution, so thats whats causing the headache.
void FUdsSubpassComposite::CreateResources(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs)
{
	// check(false); // Adding here so i dont get lost
	if (Data->bEnabled) // We can't at all prevent execution here or we throw nulls later right?
	{
		// Will be null if bIsSceneTexturesInitialized is false in ViewFamily
		if(View.GetSceneTexturesChecked() == nullptr)
		{
			// Scene textures are not ready yet
			UE_LOG(LogTemp, Warning, TEXT("Scene textures are not ready yet"));
		}

		// Depth fails the following conditions: Resource->bProduced || Resource->bExternal || Resource->bQueuedForUpload
		// This technically works, but can fail early because the depth isn't produced until after the first few frames
		Data->SceneDepthTexture = View.GetSceneTextures().Depth.Target; // Should register external instead?
		
		// MAJOR BUG
		// During editor play, the sizes of the scene depth z, UD's size, and the final buffer size can all fall out of sync
		// So some work needs to be done to ensure the depth for the final UD composit can be scaled into the correct size
		// SO, get the extents of the depth and the frame color, and save the ratio between them
		FIntPoint DepthExtent = View.GetSceneTextures().Depth.Target->Desc.Extent; // This is grabbed now
		FVector2d DepthExtentF = FVector2d(DepthExtent.X, DepthExtent.Y);

		FIntPoint ColorExtent = PassInputs.SceneColor.Texture->Desc.Extent; // This is passed in early
		FVector2d ColorExtentF = FVector2d(ColorExtent.X, ColorExtent.Y);
		
		Data->ColorDepthExtentRatio.X = ColorExtentF.X/DepthExtentF.X;
		Data->ColorDepthExtentRatio.Y = ColorExtentF.Y/DepthExtentF.Y;

		// Setup our textures for use in our post process shader later
		// I beleive this has to be registered as external?
		// Data->SceneDepthTexture = CurSceneDepth; // Might want to find a decent way to pass a zero depth value while scene depth isn't ready?
		Data->CurrentInputTexture = PassInputs.SceneColor.Texture;
		Data->OutputViewport = FScreenPassTextureViewport(PassInputs.SceneColor);
		Data->InputViewport = FScreenPassTextureViewport(PassInputs.SceneColor);
	}

	// Wondering if this edge case is ever viable to handle or if it throws exceptions later
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s data not enabled yet."), __FUNCTION__);
	}
}

void FUdsSubpassComposite::PostProcess(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs)
{
	if (Data->bEnabled)
	{
		FScreenPassRenderTarget Output = PassInputs.OverrideOutput;

		FUdsCompositePS::FParameters* PassParameters = GraphBuilder.AllocParameters<FUdsCompositePS::FParameters>();

		PassParameters->Composite.InputTexture = Data->CurrentInputTexture;
		PassParameters->Composite.DepthTexture = Data->SceneDepthTexture;
		PassParameters->Composite.UdColorTexture = Data->UdColorTexture->GetTexture2D();
		PassParameters->Composite.UdDepthTexture = Data->UdDepthTexture->GetTexture2D();
		PassParameters->RenderTargets[0] = FRenderTargetBinding(Output.Texture, ERenderTargetLoadAction::ENoAction);

		// Save the ratio to correct the editor depth size bug
		PassParameters->Composite.ColorDepthRatioX = Data->ColorDepthExtentRatio.X;
		PassParameters->Composite.ColorDepthRatioY = Data->ColorDepthExtentRatio.Y;
		
		TShaderMapRef<FUdsCompositePS> PixelShader(View.ShaderMap);

		AddDrawScreenPass(GraphBuilder,
			RDG_EVENT_NAME("UdsSubpassComposite (PS)"),
			View, Data->OutputViewport, Data->InputViewport,
			PixelShader, PassParameters,
			EScreenPassDrawFlags::None
		);

		Data->FinalOutput = Output;
		Data->CurrentInputTexture = Output.Texture;
	}
}