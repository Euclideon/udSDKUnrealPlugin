#include "UdsSubpassFirst.h"

// TODO - Remove me?
void FUdsSubpassFirst::CreateResources(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs)
{
	//Data->FSROutputTextureDesc = PassInputs.SceneColor.Texture->Desc;
	//Data->FSROutputTextureDesc.Reset();
	//Data->FSROutputTextureDesc.Extent = View.UnscaledViewRect.Max;
	//Data->FSROutputTextureDesc.ClearValue = FClearValueBinding::Black;
	//Data->FSROutputTextureDesc.Flags = TexCreate_ShaderResource | TexCreate_RenderTargetable;
}

void FUdsSubpassFirst::Upscale(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs)
{
	Data->FinalOutput = PassInputs.SceneColor; // later subpasses will override this, if enabled
}

void FUdsSubpassFirst::PostProcess(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs)
{
	// if we never ran the Upscale pass, make sure we get initialized before running the PostProcess pass
	if (!Data->FinalOutput.IsValid())
	{
		check(PassInputs.OverrideOutput.IsValid());
		Data->FinalOutput = PassInputs.OverrideOutput; // later subpasses will override this, if enabled
	}
}