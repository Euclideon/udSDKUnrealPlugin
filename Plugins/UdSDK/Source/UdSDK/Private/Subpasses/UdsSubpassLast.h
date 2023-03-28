#pragma once

#include "UdsSubpass.h"

class FUdsSubpassLast : public FUdsSubpass
{
public:
	void ParseEnvironment(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs) override;
	void CreateResources(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs) override;
};