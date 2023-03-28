#pragma once

#include "UdsSubpass.h"

class FUdsSubpassFirst : public FUdsSubpass
{
public:
	void CreateResources(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs) override;
	void Upscale(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs) override;
	void PostProcess(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs) override;
};