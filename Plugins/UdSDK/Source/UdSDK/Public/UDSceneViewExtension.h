#pragma once

#include "SceneViewExtension.h"

class FUDSceneViewExtension final : public FSceneViewExtensionBase
{
public:
	FUDSceneViewExtension(const FAutoRegister& AutoRegister);

	void SetupViewFamily(FSceneViewFamily &InViewFamily) override;
	void SetupView(FSceneViewFamily &InViewFamily, FSceneView &InView) override;

	void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override;
};