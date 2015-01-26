#include "..\\include\\ShaderCommon.hpp"

std::string shader::writeGBuffer(std::string colorVar, std::string normalVar, std::string specLevelVar, std::string specPowerVar, std::string depthVar)
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE "gl_FragData[0] = " << colorVar << ";"
	// TODO: We don't need a whole 16 bits of precision to store specular power; just the integer part is enough.
	// Can we pack both specular power and level into a single 16-bit float?
	CODE "gl_FragData[1] = vec4(" << normalVar << ".x, " << normalVar << ".y, " << normalVar << ".z, " << specPowerVar << ");"
	CODE "gl_FragData[2] = vec4(" << depthVar << ", " << specLevelVar << ", 0, 0);"
	CODE_END
	return o.str();
}

// Variable types are the same as writeGBuffer.
// coord : vec2 fragment coordinate
// Note that the G-Buffer will only be read at integer pixel locations, because sub-sampling the 
// normal and depth buffers is generally not correct. That is why we can't do MSAA here.
std::string shader::readGBuffer(std::string colorVar, std::string normalVar, std::string specLevelVar, std::string specPowerVar, std::string depthVar, std::string coord)
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE "ivec2 sampleCoordTemp = ivec2(" << coord << ");"
	CODE "vec4 " << colorVar << " = texelFetch(GBufferColor, sampleCoordTemp, 0);"
	CODE "vec4 normalVarTemp = texelFetch(GBufferNormal, sampleCoordTemp, 0);"
	CODE "vec3 " << normalVar << " = normalVarTemp.xyz;"
	CODE "float " << specPowerVar << " = normalVarTemp.w;"
	//CODE "float " << specLevelVar << " = (normalVarTemp.w - trunc(normalVarTemp.w)) * 1000.0;"
	//CODE "float " << specLevelVar << " = modf(normalVarTemp.w, " << specPowerVar << ") * (256);"
	CODE "vec4 depthVarTemp = texelFetch(GBufferDepth, sampleCoordTemp, 0);"
	CODE "float " << depthVar << " = depthVarTemp.x;"
	CODE "float " << specLevelVar << " = depthVarTemp.y;"
	CODE_END
	return o.str();
}

std::string shader::includeGBuffer()
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE "uniform sampler2D GBufferColor;"
	CODE "uniform sampler2D GBufferNormal;"
	CODE "uniform sampler2D GBufferDepth;"
	CODE_END
	return o.str();
}

void shader::setGBufferPSConstants(irr::video::IMaterialRendererServices* services)
{
	irr::s32 texPositions[] = {0, 1, 2};

    services->setPixelShaderConstant("GBufferColor", &texPositions[0], 1);
    services->setPixelShaderConstant("GBufferNormal", &texPositions[1], 1);
	services->setPixelShaderConstant("GBufferDepth", &texPositions[2], 1);
}

std::string shader::lightAttenuationFactor(std::string coeffs, std::string dist)
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE "1.0 / (" << coeffs << ".x + " << coeffs << ".y * " << dist << " + " << coeffs << ".z * " << dist << " * " << dist << ");"
	CODE_END
	return o.str();
}

std::string shader::blinnPhongBRDF(std::string L, std::string V, std::string surfNorm, std::string specLevel, std::string specPower,std::string outDiffuseCoeff, std::string outSpecCoeff)
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE "vec3 H = normalize(" << L << " + " << V << ");"
	CODE ""
	CODE "float " << outDiffuseCoeff << " = max(dot(" << surfNorm << " ,L), 0);"
	CODE "float " << outSpecCoeff << " = float(diffuseCoeff > 0) * " << specLevel << " * pow(max(dot(normal, H), 0), " << specPower << ");"
	CODE_END
	return o.str();
}

std::string shader::vsmShadow(std::string lightDistRescaled, std::string shadowMap, std::string shadowTexCoord, std::string outShadowFactor)
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE "vec2 vsmMoments = texture(" << shadowMap << ", " << shadowTexCoord << ").xy;"
	CODE "vsmMoments.x += " << getFPBias() << ";"
	CODE "float " << outShadowFactor << " = chebyshevUpperBound(vsmMoments, " << lightDistRescaled << ");"
	CODE vsmLightBleedReduction(outShadowFactor)
	CODE_END
	return o.str();
}

std::string shader::radialAttenuationFactor(std::string L, std::string lightDir, std::string cosInnerAngle, std::string cosOuterAngle, std::string outAttFactor)
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE "float cosAngle = dot(-" << L << ", " << lightDir << ");"
	CODE "float " << outAttFactor << " = 1 - clamp((cosAngle - " << cosInnerAngle << ") / (" << cosOuterAngle << " - " << cosInnerAngle << "), 0, 1);"
	CODE_END
	return o.str();
}

std::string shader::rescaleDistHeader()
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE "uniform vec2 RescaleDistParams;"
	CODE_END
	return o.str();
}

void shader::rescaleDistSetConstants(irr::video::IMaterialRendererServices *services, ICameraContext &cameraContext)
{
	irr::f32 params[2];
	
	params[0] = cameraContext.getCameraMinDist();
	params[1] = cameraContext.getCameraMaxDist() - params[0]; 

	services->setPixelShaderConstant("RescaleDistParams", params, 2);
}

std::string shader::rescaleDist(std::string dist)
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE dist << " = clamp( (" << dist << " - RescaleDistParams.x) / RescaleDistParams.y, 0, 1);"
	CODE_END
	return o.str();
}

std::string shader::getFPBias() 
{
	//return "0.5";
	return "0.0";
}

std::string shader::includeChebyshevUpperBound()
{
	ostringstream o;
	CODE_BEGIN(o)
	// moments.xy: first and second moments about 0
	CODE "float chebyshevUpperBound(vec2 moments, float value)"
	CODE "{"
	CODE "    float p = float(value <= moments.x);"
	CODE "    float minVariance = 0.000001;" // Reduces numerical stability issues. See GPU Gems 3, chapter 8
	CODE "    float variance = max(moments.y - (moments.x * moments.x), minVariance);"
	CODE "    float t = value - moments.x;"
	CODE "    "
	CODE "    float pmax = variance / (variance + (t * t));"
	CODE "    "
	CODE "    return max(p, pmax);"
	CODE "}"
	CODE_END
	return o.str();
}

std::string shader::vsmLightBleedReduction(std::string pmax)
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE "float lbrAmount = 0.5;" // TODO: make this a tunable parameter
	CODE pmax << " = clamp( (" << pmax << " - lbrAmount) / (1 - lbrAmount), 0, 1);"
	CODE_END
	return o.str();
}

//std::string shadowPCSSHeader()
//{
//	ostringstream o;
//	CODE_BEGIN(o)
//	CODE "vec2 PoissonDisk[16] = {"
//	CODE "    float2( -0.94201624, -0.39906216 ),"
//	CODE "    float2( 0.94558609, -0.76890725 ),"
//	CODE "    float2( -0.094184101, -0.92938870 ),"
//	CODE "    float2( 0.34495938, 0.29387760 ),"
//	CODE "    float2( -0.91588581, 0.45771432 ),"
//	CODE "    float2( -0.81544232, -0.87912464 ),"
//	CODE "    float2( -0.38277543, 0.27676845 ),"
//	CODE "    float2( 0.97484398, 0.75648379 ),"
//	CODE "    float2( 0.44323325, -0.97511554 ),"
//	CODE "    float2( 0.53742981, -0.47373420 ),"
//	CODE "    float2( -0.26496911, -0.41893023 ),
//float2
//( 0.79197514, 0.19090188 ),
//float2
//( -0.24188840, 0.99706507 ),
//float2
//( -0.81409955, 0.91437590 ),
//float2
//( 0.19984126, 0.78641367 ),
//float2
//( 0.14383161, -0.14100790 ) 
//	CODE_END
//	return o.str();
//}

std::string shader::projectAndSaveDepth(std::string inputVec, std::string projOut, std::string depthOut)
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE projOut << " = gl_ProjectionMatrix * gl_ModelViewMatrix * " << inputVec << ";"
	CODE depthOut << " = " << projOut << ".w;"
	//CODE projOut << " = gl_ModelViewMatrix * " << inputVec << ";"
	//CODE depthOut << " = " << projOut << ".z;"
	//CODE projOut << " = gl_ProjectionMatrix * " << projOut << ";"
	CODE_END
	return o.str();
}