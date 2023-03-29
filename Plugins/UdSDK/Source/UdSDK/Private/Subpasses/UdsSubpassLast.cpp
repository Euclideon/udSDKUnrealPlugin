#include "UdsSubpassLast.h"

// TODO - Remove me?
void FUdsSubpassLast::ParseEnvironment(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs)
{
	// check(false);
}

void FUdsSubpassLast::CreateResources(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs)
{
	Data->CurrentInputTexture = PassInputs.SceneColor.Texture;
	
	// we're done with the "initialization" steps, so set up to skip them if we run against this object again.
	Data->bInitialized = true;
}