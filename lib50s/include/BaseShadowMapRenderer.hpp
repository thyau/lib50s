#ifndef __BASESHADOWMAPRENDERER_HPP__
#define __BASESHADOWMAPRENDERER_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Partial implementation of the IShadowMapRenderer interface to provide common
 * data members and methods.
 */

#include "EMaterialTypes.h"
#include "ISceneManager.h"
#include "ISceneNode.h"
#include "ILightManager.h"
#include "IVideoDriver.h"
#include "irrArray.h"
#include "irrTypes.h"

#include "..\\include\\Common.hpp"
#include "..\\include\\IShadowMapRenderer.hpp"
#include "..\\include\\MaterialLibrary.hpp"
#include "..\\include\\MaterialTypeSwitcher.hpp"
#include "..\\include\\ICameraContext.hpp"
#include "..\\include\\PSBoxBlur.hpp"
#include "..\\include\\ShaderEffectRenderer.hpp"

class BaseShadowMapRenderer : public IShadowMapRenderer
{
public:
	BaseShadowMapRenderer(irr::video::IVideoDriver *videoDriver, MaterialLibrary &materialLib);
	virtual ~BaseShadowMapRenderer();

	virtual void setSceneManager(irr::scene::ISceneManager *sceneManager) override;

	// Mainly for shaders to access the camera parameters used to render the shadow map
	virtual ICameraContext& getCameraContext() { return s_CameraContext; }

protected:
	// Default render callback switches materials on nodes to render their depth.
	class ShadowRenderCallback : public irr::scene::ILightManager
	{
	public:
		// Set isOrthographic to true when rendering shadows with orthographic projection,
		// and false when rendering with perspective projection. For the former, pixel 
		// depth is rendered, and for the latter, pixel distance from camera center is
		// rendered.
		ShadowRenderCallback(MaterialLibrary &materialLib, bool isOrthographic = false);

		virtual void OnPreRender(irr::core::array<irr::scene::ISceneNode*> &lightList);
		virtual void OnPostRender(void);
		virtual void OnRenderPassPreRender(irr::scene::E_SCENE_NODE_RENDER_PASS renderPass);
		virtual void OnRenderPassPostRender(irr::scene::E_SCENE_NODE_RENDER_PASS renderPass);
		virtual void OnNodePreRender(irr::scene::ISceneNode* node);
		virtual void OnNodePostRender(irr::scene::ISceneNode* node);

	protected:
		MaterialTypeSwitcher m_MatSwitcher;
		irr::scene::E_SCENE_NODE_RENDER_PASS m_CurRenderPass;
		MaterialLibrary &m_MaterialLib;
		bool m_IsOrtho;

		// Store the material type IDs so we don't have to look them up on every frame
		irr::video::E_MATERIAL_TYPE m_DepthMat, m_DistMat, m_NullMat;
	};

	class ShadowCameraContext : public ICameraContext
	{
	public:
		virtual irr::scene::ICameraSceneNode* getCameraNode() override { return m_CameraNode; }
		virtual void setCameraNode(irr::scene::ICameraSceneNode *node);
		
		virtual irr::f32 getCameraMinDist() override { return m_CameraMinDist; }
		virtual irr::f32 getCameraMaxDist() override { return m_CameraMaxDist; }

	protected:
		irr::scene::ICameraSceneNode *m_CameraNode;
		irr::f32 m_CameraMinDist, m_CameraMaxDist;
	};

	irr::scene::ISceneManager *m_SceneManager;
	irr::video::IVideoDriver *m_VideoDriver;
	MaterialLibrary &m_MaterialLib;
	irr::scene::ICameraSceneNode *m_ShadowCamera;

	// The materials that we added to the MaterialLib are shared amongst all shadow renderers,
	// so the input contexts are shared as well.
	static ShadowCameraContext s_CameraContext;
	static PSBoxBlur::BLUR_PARAMS s_BoxBlurParams;
	static ShaderEffectRenderer s_EffectRenderer;
};

#endif