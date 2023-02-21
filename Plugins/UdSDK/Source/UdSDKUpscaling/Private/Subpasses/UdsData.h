#pragma once

#include "UdsSubpassSharedTypes.h"
#include "PostProcess/PostProcessTonemap.h"

struct FUdsData
{
	bool bInitialized = false;
	bool bEnabled;

	//FRDGTextureDesc FSROutputTextureDesc;
	//FPostProcessSettings ChromaticAberrationPostProcessSettings;
	//FScreenPassTextureViewportParameters PassOutputViewportParams;

	FScreenPassTextureViewport InputViewport;
	FScreenPassTextureViewport OutputViewport;

	FRDGTextureRef CurrentInputTexture;
	FRDGTextureRef SceneDepthTexture;
	FTexture2DRHIRef UdColorTexture;
	FTexture2DRHIRef UdDepthTexture;
	FScreenPassTexture FinalOutput;

	FVector2d ColorDepthExtentRatio; // Adding
};