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

#include "EMaterialTypes.h"
#include "IVideoDriver.h"
#include "ITexture.h"
#include "irrTypes.h"
#include "S3DVertex.h"
#include "SMaterial.h"
#include "vector2d.h"

#include "..\\include\\Common.hpp"

class ShaderEffectRenderer {
public:
	ShaderEffectRenderer(irr::video::IVideoDriver *driver);
	virtual ~ShaderEffectRenderer();

	// Note: the texture must have been created using IVideoDriver->addRenderTargetTexture()
	// Note: to render on top of existing data, you probably want to setClearBackBuffer(false).
	// Optional viewport specifies region of the targetTexture to render onto.
	virtual void setDirectRender(irr::video::ITexture *targetTexture, irr::core::recti viewport = irr::core::recti(0, 0, -1, -1));

	// One texture is input, the other is output. Calling render() multiple times
	// in succession will alternate which one is input and which is output.
	// Initially, targetTexture1 is the input.
	// Optional viewport1 and viewport2 specify the texture regions to render from/to.
	// i.e. shader will read from viewport1 on targeTexture1 and render to viewport2
	// on targetTexture2, and vice versa. 
	// Note: not all shaders may not respect the input viewport, see getInputRegion().
	// Note: the textures must have been created using IVideoDriver->addRenderTargetTexture()
	virtual void setPingPongRender(irr::video::ITexture *targetTexture1, irr::video::ITexture *targetTexture2,
		irr::core::recti viewport1 = irr::core::recti(0, 0, -1, -1), irr::core::recti viewport2 = irr::core::recti(0, 0, -1, -1));

	// materialType is the material containing the shaders to execute
	// texLayer is the texture layer number to set the input texture to, for the specified material.
	virtual void render(irr::video::E_MATERIAL_TYPE materialType, irr::u32 texLayer);

	// Returns whichever is the input texture going into the next render() call. May be NULL.
	virtual irr::video::ITexture* getInputTexture() const;

	// Returns whichever is the output texture going into the next render() call.
	virtual irr::video::ITexture* getOutputTexture() const;

	// Return the texture that was last rendered to.
	// Result is undefined if render() has not been called yet.
	virtual irr::video::ITexture* getLastRenderTexture() const;

	// Get the material struct used for rendering.
	// You may modify any of the properties, but rendering may not work properly if you
	// change the ZWriteEnable, Front/BackfaceCulling, Wireframe, and Lighting properties.
	virtual irr::video::SMaterial& getMaterial() { return m_Material; }

	// Whether to clear the output texture before render.
	void setClearBackBuffer(bool clearBackBuffer) { m_ClearBack = clearBackBuffer; }

	// Whether to clear the Z buffer of the output texture before render.
	// Note: the Z buffer may be shared with other render targets.
	void setClearZBuffer(bool clearZBuffer) { m_ClearZ = clearZBuffer; }

	void setVideoDriver(irr::video::IVideoDriver *driver);

	// Normalized offset/scale specifying region to read from input texture.
	// Has no effect if the input texture is NULL.
	// Intended for use by shaders.
	void getInputRegion(irr::core::vector2df &offset, irr::core::vector2df &scale) const;

protected:
	irr::video::IVideoDriver *m_VideoDriver;
	irr::video::ITexture *m_TexIn, *m_TexOut;
	irr::core::recti m_ViewportIn, m_ViewportOut;
	irr::video::S3DVertex m_Vertices[6];
	irr::video::SMaterial m_Material;
	bool m_ClearBack, m_ClearZ;
};

#endif