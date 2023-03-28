#pragma once

#include "UdsSubpass.h"

class FUdsSubpassComposite : public FUdsSubpass
{
public:
	void ParseEnvironment(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs) override;
	void CreateResources(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs) override;
	void PostProcess(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs) override;

private:
};