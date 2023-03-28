#pragma once

#include "Subpasses/UdsData.h"

#include "PostProcess/PostProcessUpscale.h"
#include "PostProcess/TemporalAA.h"

class FUdsSubpass;

enum class EUdsMode
{
	None,
	UpscalingOnly,
	PostProcessingOnly,
	Combined
};

//class FUdSDKCompositeUpscaler final : public ITemporalUpscaler
class FUdSDKCompositeUpscaler final : public ISpatialUpscaler
{
public:
	FUdSDKCompositeUpscaler(EUdsMode InMode, TArray<TSharedPtr<FUdsData>> InViewData);

	// ISpatialUpscaler interface
	const TCHAR* GetDebugName() const override { return TEXT("FUdSDKCompositeUpscaler"); }

	ISpatialUpscaler* Fork_GameThread(const class FSceneViewFamily& ViewFamily) const override;
	FScreenPassTexture AddPasses(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FInputs& PassInputs) const override;

private:
	template <class T>
	T* RegisterSubpass()
	{
		T* Subpass = new T();
		FUdsubpasses.Add(Subpass);
		return Subpass;
	}

	TSharedPtr<FUdsData> GetDataForView(const FViewInfo& View) const;

	EUdsMode Mode;
	TArray<TSharedPtr<FUdsData>> ViewData;
	TArray<FUdsSubpass*> FUdsubpasses;
};