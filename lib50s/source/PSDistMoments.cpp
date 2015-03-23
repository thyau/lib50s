/*
	PSDistMoments.cpp
	Copyright (c) [ORGNAME] 2014
	Author: Timothy Yau
*/

#include "..\\include\\PSDistMoments.hpp"
#include "..\\include\\ShaderCommon.hpp"

using namespace irr;
using namespace irr::video;

using namespace shader;

std::string	PSDistMoments::buildSource()
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE "#version 130"
	CODE ""
	CODE rescaleDistHeader()
	CODE ""
	CODE "varying vec3 VEyeCoord;"
	CODE ""
	CODE "void main() {"
	CODE "    float dist = length(VEyeCoord);"
	CODE      rescaleDist("dist")
	CODE "    gl_FragColor = vec4(dist - " << getFPBias() << ", dist * dist, 0, 0);" 
	//CODE "    gl_FragColor = vec4(dist, dist * dist, 0, 0);" 
	CODE "}"
	CODE_END
	return o.str();
}

void PSDistMoments::Callback::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	rescaleDistSetConstants(services, m_CameraContext);
}