#pragma once

#include "SceneViewExtension.h"

class FUdSDKCompositeViewExtension final : public FSceneViewExtensionBase
{
public:
	FUdSDKCompositeViewExtension(const FAutoRegister& AutoRegister);

	void SetupViewFamily(FSceneViewFamily &InViewFamily) override;
	void SetupView(FSceneViewFamily &InViewFamily, FSceneView &InView) override;

	void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override;
};