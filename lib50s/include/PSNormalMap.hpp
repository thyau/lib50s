#ifndef __PSNORMALMAP_HPP__
#define __PSNORMALMAP_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Renders diffuse color, texture-mapped normal, and depth into the G-buffer.
 * Specular level is taken from the red channel of SpecularColor of the material
 * Specular power is taken from the Shininess of the material
 * 
 * Texture locations:
 *	0: RGBA diffuse color
 *  1: XYZ surface normal
 */

#include "..\\include\\PSDiffuse.hpp"

class PSNormalMap : public PSDiffuse
{
public:
	virtual std::string	buildSource() override;
	virtual IShaderConstantSetCallBack* getDefaultCallbackObject() override;

protected:
	class Callback : public PSDiffuse::Callback
	{
	public:
		virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData) override;
	};
};

#endif