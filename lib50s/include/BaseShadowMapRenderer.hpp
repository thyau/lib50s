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
	BaseShadowMapRenderer(IVideoDriver *videoDriver, MaterialLibrary &materialLib);
	virtual ~BaseShadowMapRenderer();

	virtual void setSceneManager(ISceneManager *sceneManager) override;

	// Mainly for shaders to access the camera parameters used to render the shadow map
	virtual ICameraContext& getCameraContext() { return s_CameraContext; }

protected:
	// Default render callback switches materials on nodes to render their depth.
	class ShadowRenderCallback : public ILightManager
	{
	public:
		// Set isOrthographic to true when rendering shadows with orthographic projection,
		// and false when rendering with perspective projection. For the former, pixel 
		// depth is rendered, and for the latter, pixel distance from camera center is
		// rendered.
		ShadowRenderCallback(MaterialLibrary &materialLib, bool isOrthographic = false);

		virtual void OnPreRender(core::array<ISceneNode*> &lightList);
		virtual void OnPostRender(void);
		virtual void OnRenderPassPreRender(E_SCENE_NODE_RENDER_PASS renderPass);
		virtual void OnRenderPassPostRender(E_SCENE_NODE_RENDER_PASS renderPass);
		virtual void OnNodePreRender(ISceneNode* node);
		virtual void OnNodePostRender(ISceneNode* node);

	protected:
		MaterialTypeSwitcher m_MatSwitcher;
		E_SCENE_NODE_RENDER_PASS m_CurRenderPass;
		MaterialLibrary &m_MaterialLib;
		bool m_IsOrtho;

		// Store the material type IDs so we don't have to look them up on every frame
		E_MATERIAL_TYPE m_DepthMat, m_DistMat, m_NullMat;
	};

	class ShadowCameraContext : public ICameraContext
	{
	public:
		virtual ICameraSceneNode* getCameraNode() override { return m_CameraNode; }
		virtual void setCameraNode(ICameraSceneNode *node);
		
		virtual f32 getCameraMinDist() override { return m_CameraMinDist; }
		virtual f32 getCameraMaxDist() override { return m_CameraMaxDist; }

	protected:
		ICameraSceneNode *m_CameraNode;
		f32 m_CameraMinDist, m_CameraMaxDist;
	};

	ISceneManager *m_SceneManager;
	IVideoDriver *m_VideoDriver;
	MaterialLibrary &m_MaterialLib;
	ICameraSceneNode *m_ShadowCamera;

	// The materials that we added to the MaterialLib are shared amongst all shadow renderers,
	// so the input contexts are shared as well.
	static ShadowCameraContext s_CameraContext;
	static PSBoxBlur::BLUR_PARAMS s_BoxBlurParams;
	static ShaderEffectRenderer s_EffectRenderer;
};

#endif