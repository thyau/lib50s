#ifndef __PSSPECULARMAP_HPP__
#define __PSSPECULARMAP_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Renders diffuse color, texture-mapped normal, texture-mapped specular, and
 * and depth into the G-buffer.
 * Specular level is from the red channel of the specular map, and multiplied by 
 * the red channel of SpecularColor.
 * Specular power is from the green channels of the specular map, and multiplied 
 * by the Shininess of the material.
 * 
 * Texture locations:
 *	0: RGBA diffuse color
 *  1: XYZ surface normal
 *  2: RG specular level/power
 */

#include "..\\include\\PSNormalMap.hpp"

class PSSpecularMap : public PSNormalMap
{
public:
	virtual std::string	buildSource() override;
	virtual IShaderConstantSetCallBack* getDefaultCallbackObject() override;

protected:
	class Callback : public PSNormalMap::Callback
	{
	public:
		virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData) override;
	};
};

#endif