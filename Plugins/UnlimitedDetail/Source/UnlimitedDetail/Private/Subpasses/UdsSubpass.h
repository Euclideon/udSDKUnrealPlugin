#pragma once

#include "UdsData.h"

#include "PostProcess/PostProcessUpscale.h"

class FUdsSubpass
{
public:
	typedef ISpatialUpscaler::FInputs FInputs;

	inline void SetData(FUdsData* InData)
	{
		Data = InData;
	};

	virtual void ParseEnvironment(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs) {}
	virtual void CreateResources(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs) {}
	virtual void Upscale(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs) {}
	virtual void PostProcess(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs) {}

protected:
	FUdsData* Data;
};