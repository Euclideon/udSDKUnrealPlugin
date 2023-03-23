#pragma once

#include "ShaderParameterMacros.h"

BEGIN_SHADER_PARAMETER_STRUCT(FCompositePassParameters, )
	SHADER_PARAMETER_RDG_TEXTURE(Texture2D, InputTexture)
	SHADER_PARAMETER_RDG_TEXTURE(Texture2D, DepthTexture)
	SHADER_PARAMETER_TEXTURE(Texture2D, UdColorTexture)
	SHADER_PARAMETER_TEXTURE(Texture2D, UdDepthTexture)
	//SHADER_PARAMETER(UBMT_FLOAT32, ColorDepthRatio)
	SHADER_PARAMETER(float, ColorDepthRatioX)
	SHADER_PARAMETER(float, ColorDepthRatioY)



//	UBMT_FLOAT32,

END_SHADER_PARAMETER_STRUCT()