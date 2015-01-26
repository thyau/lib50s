#ifndef __SHADEREFFECTRENDERER_HPP__
#define __SHADEREFFECTRENDERER_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Runs pixel shader programs on textures. Create a material using
 * vertex shader VSNormCoords, and pass it to this class to render
 * the pixel shader over the entire texture.
 *
 * Based on CScreenQuad.h from XEffects.
 */

#include "..\\include\\Common.hpp"

class ShaderEffectRenderer {
public:
	ShaderEffectRenderer(IVideoDriver *driver);
	virtual ~ShaderEffectRenderer();

	// Note: the texture must have been created using IVideoDriver->addRenderTargetTexture()
	// Note: to render on top of existing data, you probably want to setClearBackBuffer(false).
	// Optional viewport specifies region of the targetTexture to render onto.
	virtual void setDirectRender(ITexture *targetTexture, recti viewport = recti(0, 0, -1, -1));

	// One texture is input, the other is output. Calling render() multiple times
	// in succession will alternate which one is input and which is output.
	// Initially, targetTexture1 is the input.
	// Optional viewport1 and viewport2 specify the texture regions to render from/to.
	// i.e. shader will read from viewport1 on targeTexture1 and render to viewport2
	// on targetTexture2, and vice versa. 
	// Note: not all shaders may not respect the input viewport, see getInputRegion().
	// Note: the textures must have been created using IVideoDriver->addRenderTargetTexture()
	virtual void setPingPongRender(ITexture *targetTexture1, ITexture *targetTexture2,
		 recti viewport1 = recti(0, 0, -1, -1), recti viewport2 = recti(0, 0, -1, -1));

	// materialType is the material containing the shaders to execute
	// texLayer is the texture layer number to set the input texture to, for the specified material.
	virtual void render(E_MATERIAL_TYPE materialType, u32 texLayer);

	// Returns whichever is the input texture going into the next render() call. May be NULL.
	virtual ITexture* getInputTexture() const;

	// Returns whichever is the output texture going into the next render() call.
	virtual ITexture* getOutputTexture() const;

	// Return the texture that was last rendered to.
	// Result is undefined if render() has not been called yet.
	virtual ITexture* getLastRenderTexture() const;

	// Get the material struct used for rendering.
	// You may modify any of the properties, but rendering may not work properly if you
	// change the ZWriteEnable, Front/BackfaceCulling, Wireframe, and Lighting properties.
	virtual SMaterial& getMaterial() { return m_Material; }

	// Whether to clear the output texture before render.
	void setClearBackBuffer(bool clearBackBuffer) { m_ClearBack = clearBackBuffer; }

	// Whether to clear the Z buffer of the output texture before render.
	// Note: the Z buffer may be shared with other render targets.
	void setClearZBuffer(bool clearZBuffer) { m_ClearZ = clearZBuffer; }

	void setVideoDriver(IVideoDriver *driver);

	// Normalized offset/scale specifying region to read from input texture.
	// Has no effect if the input texture is NULL.
	// Intended for use by shaders.
	void getInputRegion(vector2df &offset, vector2df &scale) const;

protected:
	IVideoDriver *m_VideoDriver;
	ITexture *m_TexIn, *m_TexOut;
	recti m_ViewportIn, m_ViewportOut;
	S3DVertex m_Vertices[6];
	SMaterial m_Material;
	bool m_ClearBack, m_ClearZ;
};

#endif