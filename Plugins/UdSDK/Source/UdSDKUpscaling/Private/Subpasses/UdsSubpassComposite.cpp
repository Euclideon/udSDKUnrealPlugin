#include "UdsSubpassComposite.h"
#include "UdSDKComposite.h"
// #include "Engine/TextureRenderTarget2D.h"
// #include "SceneRenderTargets.h"
#include "EdGraphSchema_K2_Actions.h"
#include "ExternalTexture.h"
#include "UdSDKSubsystem.h"
// #include "UdSDKSubsystem.h"

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
	UE_LOG(LogTemp, Warning, TEXT("Create resources running ..."));
	// Immediately after a return here // error  FParameters::Composite::DepthTexture was not set. throws
	// UE_LOG(LogTemp, Warning, TEXT("Returning early as a test"));
	// return;

	// check(false); // Adding here so i dont get lost
	if (Data->bEnabled) // We can't at all prevent execution here or we throw nulls later right?
	{
		UE_LOG(LogTemp, Warning, TEXT("Create resources AND data valid running ..."));
		// Cache a ref to the subsystem
		// Can use this as a generic singleton to store UD related data
		UUdSDKSubsystem* udSubsystem = GEngine->GetEngineSubsystem<UUdSDKSubsystem>();

		

		
		// trying to prevent Resource->bProduced || Resource->bExternal || Resource->bQueuedForUpload error on texture
		
		// These are the original 4.27 lines that needed to be refactored beacuse FSceneRenderTargets has been replaced with FSceneTextures
		// const FSceneRenderTargets& SceneContext = FSceneRenderTargets::Get(GraphBuilder.RHICmdList); // 4.27 line Refactor me beacuse this was depreciated in 5.0
		//Data->SceneDepthTexture = GraphBuilder.RegisterExternalTexture(SceneContext.SceneDepthZ, TEXT("SceneDepthTexture")); // 4.27 line Original line that no longer works

		// TODO - Investigate if this can be better handled?
		// Not sure if this should be done here / every frame
		// there MUST be a better way to handle this
		// FSceneTextures::InitializeViewFamily(GraphBuilder, *(FViewFamilyInfo*)View.Family);
		
		// Will be null if bIsSceneTexturesInitialized is false in ViewFamily
		if(View.GetSceneTexturesChecked() == nullptr)
		{
			// Scene textures are not ready yet
			UE_LOG(LogTemp, Warning, TEXT("Scene textures are not ready yet"));
		}

		// Depth fails the following conditions: Resource->bProduced || Resource->bExternal || Resource->bQueuedForUpload
		// This technically works, but can fail early because the depth isn't produced until after the first few frames
		Data->SceneDepthTexture = View.GetSceneTextures().Depth.Target; // Should register external instead?

		
		
		// Get the size
		FIntPoint DepthExtent = View.GetSceneTextures().Depth.Target->Desc.Extent; // This is grabbed now
		FVector2d DepthExtentF = FVector2d(DepthExtent.X, DepthExtent.Y);

		FIntPoint ColorExtent = PassInputs.SceneColor.Texture->Desc.Extent; // This is passed in early
		FVector2d ColorExtentF = FVector2d(ColorExtent.X, ColorExtent.Y);



		// if the system is valid, set some data about what we intend to render
		if (IsValid(udSubsystem))
		{
			UE_LOG(LogTemp, Warning, TEXT("Subsystem is valid, depth: - X: %d, Y: %d"), DepthExtent.X , DepthExtent.Y);

			// We save a copy of the buffer width and height so that when we ask UD for a render, we have the correct dimensions
			udSubsystem->SetDepthExtents(DepthExtent.X, DepthExtent.Y);
			// udSubsystem->SetColorExtents(ColorExtent.X, ColorExtent.Y);

		}

		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Subsystem in udsubpasscomposit is NOT valid ..."));
		}

		UE_LOG(LogTemp, Warning, TEXT("DepthExtentF - X: %f, Y: %f"), DepthExtentF.X , DepthExtentF.Y);
		UE_LOG(LogTemp, Warning, TEXT("ColorExtentF - X: %f, Y: %f"), ColorExtentF.X , ColorExtentF.Y);
		
		Data->ColorDepthExtentRatio.X = ColorExtentF.X/DepthExtentF.X;
		Data->ColorDepthExtentRatio.Y = ColorExtentF.Y/DepthExtentF.Y;
		
		UE_LOG(LogTemp, Warning, TEXT("LITTERAL val: %f"), ColorExtentF.X);
		UE_LOG(LogTemp, Warning, TEXT("LITTERAL val: %f"), DepthExtentF.X);
		UE_LOG(LogTemp, Warning, TEXT("DepthExtent - X: %d, Y: %d"), DepthExtent.X , DepthExtent.Y);
		
		UE_LOG(LogTemp, Warning, TEXT("Ratio at time of CreateResources - X: %f, Y: %f"),Data->ColorDepthExtentRatio.X ,Data->ColorDepthExtentRatio.Y );

		
		//static_cast<float>
		
		// CurSceneDepth.

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

		// Save the ratio to correct the editor depth size bug
		PassParameters->Composite.ColorDepthRatioX = Data->ColorDepthExtentRatio.X;
		PassParameters->Composite.ColorDepthRatioY = Data->ColorDepthExtentRatio.Y;

		//UE_LOG(LogTemp, Warning, TEXT("The float value is: %f"), Data->ColorDepthExtentRatio.Y);
		UE_LOG(LogTemp, Warning, TEXT("Ratio at time of PostProcess - X: %f, Y: %f"),Data->ColorDepthExtentRatio.X ,Data->ColorDepthExtentRatio.Y );

		
		
		
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