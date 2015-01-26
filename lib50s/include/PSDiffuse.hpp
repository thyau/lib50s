#ifndef __PSDIFFUSE_HPP__
#define __PSDIFFUSE_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Renders diffuse color, interpolated normal, and depth into the G-buffer.
 * Specular level is taken from the red channel of SpecularColor of the material
 * Specular power is taken from the Shininess of the material
 * 
 * Texture locations:
 *	0: RGBA diffuse color
 */

#include "..\\include\\BasePixelShader.hpp"

class PSDiffuse : public BasePixelShader
{
public:
	virtual std::string	buildSource() override;
	IShaderConstantSetCallBack* getDefaultCallbackObject() override;

protected:
	class Callback : public IShaderConstantSetCallBack
	{
	public:
		virtual void OnSetMaterial(const SMaterial& material) override;
		virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData) override;

	protected:
		float m_SpecularLevel, m_SpecularPower;
	};
};

#endif