#ifndef __DEFERREDRENDERPIPELINE_HPP__
#define __DEFERREDRENDERPIPELINE_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Deferred-rendering pipeline.
 */

#include "..\\include\\BaseRenderPipeline.hpp"
#include "..\\include\\MaterialLibrary.hpp"
#include "..\\include\\ILightContext.hpp"
#include "..\\include\\MaterialTypeSwitcher.hpp"
#include "..\\include\\ShadowMapAllocator.hpp"
#include "..\\include\\CubeShadowMapRenderer.hpp"
#include "..\\include\\SpotShadowMapRenderer.hpp"
#include "..\\include\\CascadedShadowMapRenderer.hpp"

class DeferredRenderPipeline : public BaseRenderPipeline
{
public:
	DeferredRenderPipeline(IVideoDriver *videoDriver);
	virtual ~DeferredRenderPipeline(void);

	virtual RESULT	init();
	virtual void	run();
	virtual void	setSceneManager(ISceneManager *sceneManager);

protected:
	// Contains format definition of the G-Buffer
	virtual RESULT	allocateGBuffer(dimension2du dim);
	// Contains format definition of the frame buffers
	virtual RESULT	allocateFrameBuffers(dimension2du dim);
	// Contains definitions of all materials
	virtual RESULT	loadMaterials();
	// Create mesh nodes that we render to run the lighting shaders over an area of the screen
	virtual void	createLightMeshes(ISceneManager *mgr);
	virtual void	removeLightMeshes();
	// Set the material texture layers on the given node to be our GBuffer textures
	virtual void	setGBufferTextures(ISceneNode *node);

	virtual void	onPreRender(core::array<ISceneNode*> &lightList);
	virtual void	onPostRender(void);
	virtual void	onRenderPassPreRender(E_SCENE_NODE_RENDER_PASS renderPass);
	virtual void	onRenderPassPostRender(E_SCENE_NODE_RENDER_PASS renderPass);
	virtual void	onNodePreRender(ISceneNode* node);
	virtual void	onNodePostRender(ISceneNode* node);

	virtual void	lightingPass();
	virtual void	postProcessPass();

	virtual void	updateFullscreenBillboard(const ICameraSceneNode *camera);

	// Calculate distance at which light intensity drops down to specified threshold
	// Returns FLT_MAX if light has infinite extinction radius
	static float    computeExtinctionRadius(const SLight &lightData, float threshold);

	// We hook into the scene rendering process with the light manager interface, and
	// render our lights at specific points.
	friend class SceneRenderCallback;
	class SceneRenderCallback : public ILightManager, public ILightContext
	{
	public:
		SceneRenderCallback(DeferredRenderPipeline &d) : m_DeferredPipeline(d)
		{
		}

		//! Called after the scene's light list has been built, but before rendering has begun.
		/** As actual device/hardware lights are not created until the
		ESNRP_LIGHT render pass, this provides an opportunity for the
		light manager to trim or re-order the light list, before any
		device/hardware lights have actually been created.
		\param lightList: the Scene Manager's light list, which
		the light manager may modify. This reference will remain valid
		until OnPostRender().
		*/
		virtual void OnPreRender(core::array<ISceneNode*> &lightList) { m_DeferredPipeline.onPreRender(lightList); }

		//! Called after the last scene node is rendered.
		/** After this call returns, the lightList passed to OnPreRender() becomes invalid. */
		virtual void OnPostRender(void) { m_DeferredPipeline.onPostRender(); }

		//! Called before a render pass begins
		/** \param renderPass: the render pass that's about to begin */
		virtual void OnRenderPassPreRender(E_SCENE_NODE_RENDER_PASS renderPass) { m_DeferredPipeline.onRenderPassPreRender(renderPass); }

		//! Called after the render pass specified in OnRenderPassPreRender() ends
		/** \param[in] renderPass: the render pass that has finished */
		virtual void OnRenderPassPostRender(E_SCENE_NODE_RENDER_PASS renderPass) { m_DeferredPipeline.onRenderPassPostRender(renderPass); }

		//! Called before the given scene node is rendered
		/** \param[in] node: the scene node that's about to be rendered */
		virtual void OnNodePreRender(ISceneNode* node) { m_DeferredPipeline.onNodePreRender(node); }

		//! Called after the the node specified in OnNodePreRender() has been rendered
		/** \param[in] node: the scene node that has just been rendered */
		virtual void OnNodePostRender(ISceneNode* node) { m_DeferredPipeline.onNodePostRender(node); }

		virtual inline const SLight* getLightData() { return m_LightData; }
		virtual inline void setLightData(const SLight *lightData) { m_LightData = lightData; }

		//virtual inline const SViewFrustum* getViewFrustum() { return m_ViewFrustum; }
		//virtual inline void SViewFrustum* setViewFrustum(const SViewFrustum *frustum) { m_ViewFrustum = frustum; }

		virtual inline const vector3df& getLightEyePos() { return m_LightEyePos; }
		virtual inline void setLightEyePos(const vector3df &pos) { m_LightEyePos = pos; }

		virtual const matrix4& getRenderViewMatrixInverse() { return m_ViewMatrixInverse; }
		virtual void setRenderViewMatrixInverse(const matrix4& mat) { m_ViewMatrixInverse = mat; }

		//virtual inline const vector3df& getViewAbsPos() { return m_ViewAbsPos; }
		//virtual inline void setViewAbsPos(const vector3df &pos) { m_ViewAbsPos = pos; }

	protected:
		DeferredRenderPipeline &m_DeferredPipeline;
		const SLight *m_LightData;
		vector3df m_LightEyePos;
		matrix4 m_ViewMatrixInverse;
	};

	irr::core::array<IRenderTarget> m_GBuffer;
	irr::core::array<IRenderTarget> m_FrameBuffers;

	// State variables used during rendering process
	SceneRenderCallback m_RenderCallback;
	core::array<ISceneNode*> m_LightList;
	E_SCENE_NODE_RENDER_PASS m_CurRenderPass;
	core::array<ISceneNode*> m_TransNodeList; // list of transparent nodes in rendering order
	MaterialTypeSwitcher m_MatSwitcher;

	// Mesh objects for rendering lights
	IMeshSceneNode* m_LightSphere;
	IBillboardSceneNode *m_LightBillboard, *m_LightFullscreenBillboard;

	ShaderEffectRenderer m_EffectRenderer;

	// Shadow renderering objects
	ShadowMapAllocator m_ShadowMapAlloc;
	CubeShadowMapRenderer m_CubeShadowRenderer;
	SpotShadowMapRenderer m_SpotShadowRenderer;
	float m_ShadowCascadeSizes[1];
	CascadedShadowMapRenderer m_CascadedShadowRenderer;
};

#endif