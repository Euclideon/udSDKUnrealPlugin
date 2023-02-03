#include "UdsSubpassComposite.h"
#include "UdSDKComposite.h"
#include "Engine/TextureRenderTarget2D.h"
// #include "SceneRenderTargets.h"
#include "EdGraphSchema_K2_Actions.h"
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


void FUdsSubpassComposite::CreateResources(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs)
{
	// Immediately after a return here // error  FParameters::Composite::DepthTexture was not set. throws
	UE_LOG(LogTemp, Warning, TEXT("Returning early as a test"));
	// return;

	// check(false); // Adding here so i dont get lost
	if (Data->bEnabled) // We can't at all prevent execution here or we throw nulls later right?
	{
		// trying to prevent Resource->bProduced || Resource->bExternal || Resource->bQueuedForUpload error on texture
		
		// These are the original 4.27 lines that needed to be refactored beacuse FSceneRenderTargets has been replaced with FSceneTextures
		// const FSceneRenderTargets& SceneContext = FSceneRenderTargets::Get(GraphBuilder.RHICmdList); // 4.27 line Refactor me beacuse this was depreciated in 5.0
		//Data->SceneDepthTexture = GraphBuilder.RegisterExternalTexture(SceneContext.SceneDepthZ, TEXT("SceneDepthTexture")); // 4.27 line Original line that no longer works

		// TODO - Investigate if this can be better handled?
		// Not sure if this should be done here / every frame
		FSceneTextures::InitializeViewFamily(GraphBuilder, *(FViewFamilyInfo*)View.Family);

		// Will be null if bIsSceneTexturesInitialized is false in ViewFamily
		if(View.GetSceneTexturesChecked() == nullptr)
		{
			// Scene textures are not ready yet
			UE_LOG(LogTemp, Warning, TEXT("Scene textures are not ready yet"));
		}
		
		/*if (!View.GetSceneTextures().Depth.Resolve->HasBeenProduced())
		{
			UE_LOG(LogTemp, Warning, TEXT("Scene depth has not been produced yet."));
		}*/

		// View.GetSceneTextures().Depth.Resolve->HasBeenProduced();

		// Grab a ref to the depth texture of the current views scene textures
		// Must call InitializeViewFamily before accessing
		FRDGTextureRef CurSceneDepth = View.GetSceneTextures().Depth.Resolve;
		CurSceneDepth = View.GetSceneTextures().Depth.Target;

		
		
		if(CurSceneDepth->HasBeenProduced())
		{
			UE_LOG(LogTemp, Warning, TEXT("Scene depth target has been produced"));
		}

		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Scene depth target has not been produced yet"));
		}

		if(CurSceneDepth->IsExternal())
		{
			UE_LOG(LogTemp, Warning, TEXT("Scene depth target is external"));
		}

		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Scene depth target is NOT external"));
		}
		// CurSceneDepth.

		// Setup our textures for use in our post process shader later
		// I beleive this has to be registered as external?
		Data->SceneDepthTexture = CurSceneDepth; // Might want to find a decent way to pass a zero depth value while scene depth isn't ready?
		Data->CurrentInputTexture = PassInputs.SceneColor.Texture;
		Data->OutputViewport = FScreenPassTextureViewport(PassInputs.SceneColor);
		Data->InputViewport = FScreenPassTextureViewport(PassInputs.SceneColor);
	}

	// Wondering if this edge case is ever viable to handle or if it throws exceptions later
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UDSSubpassCOmposit not enabled yet"));
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