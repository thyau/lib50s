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

#include "ICameraSceneNode.h"
#include "IMeshSceneNode.h"
#include "IBillboardSceneNode.h"
#include "ISceneNode.h"
#include "ISceneManager.h"
#include "ILightManager.h"
#include "IVideoDriver.h"
#include "dimension2d.h"
#include "irrArray.h"
#include "matrix4.h"
#include "vector3d.h"
#include "SLight.h"

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
	DeferredRenderPipeline(irr::video::IVideoDriver *videoDriver);
	virtual ~DeferredRenderPipeline(void);

	virtual RESULT	init();
	virtual void	run();
	virtual void	setSceneManager(irr::scene::ISceneManager *sceneManager);

protected:
	// Contains format definition of the G-Buffer
	virtual RESULT	allocateGBuffer(irr::core::dimension2du dim);
	// Contains format definition of the frame buffers
	virtual RESULT	allocateFrameBuffers(irr::core::dimension2du dim);
	// Contains definitions of all materials
	virtual RESULT	loadMaterials();
	// Create mesh nodes that we render to run the lighting shaders over an area of the screen
	virtual void	createLightMeshes(irr::scene::ISceneManager *mgr);
	virtual void	removeLightMeshes();
	// Set the material texture layers on the given node to be our GBuffer textures
	virtual void	setGBufferTextures(irr::scene::ISceneNode *node);

	virtual void	onPreRender(irr::core::array<irr::scene::ISceneNode*> &lightList);
	virtual void	onPostRender(void);
	virtual void	onRenderPassPreRender(irr::scene::E_SCENE_NODE_RENDER_PASS renderPass);
	virtual void	onRenderPassPostRender(irr::scene::E_SCENE_NODE_RENDER_PASS renderPass);
	virtual void	onNodePreRender(irr::scene::ISceneNode* node);
	virtual void	onNodePostRender(irr::scene::ISceneNode* node);

	virtual void	lightingPass();
	virtual void	postProcessPass();

	virtual void	updateFullscreenBillboard(const irr::scene::ICameraSceneNode *camera);

	// Calculate distance at which light intensity drops down to specified threshold
	// Returns FLT_MAX if light has infinite extinction radius
	static float    computeExtinctionRadius(const irr::video::SLight &lightData, float threshold);

	// We hook into the scene rendering process with the light manager interface, and
	// render our lights at specific points.
	friend class SceneRenderCallback;
	class SceneRenderCallback : public irr::scene::ILightManager, public ILightContext
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
		virtual void OnPreRender(irr::core::array<irr::scene::ISceneNode*> &lightList) { m_DeferredPipeline.onPreRender(lightList); }

		//! Called after the last scene node is rendered.
		/** After this call returns, the lightList passed to OnPreRender() becomes invalid. */
		virtual void OnPostRender(void) { m_DeferredPipeline.onPostRender(); }

		//! Called before a render pass begins
		/** \param renderPass: the render pass that's about to begin */
		virtual void OnRenderPassPreRender(irr::scene::E_SCENE_NODE_RENDER_PASS renderPass) { m_DeferredPipeline.onRenderPassPreRender(renderPass); }

		//! Called after the render pass specified in OnRenderPassPreRender() ends
		/** \param[in] renderPass: the render pass that has finished */
		virtual void OnRenderPassPostRender(irr::scene::E_SCENE_NODE_RENDER_PASS renderPass) { m_DeferredPipeline.onRenderPassPostRender(renderPass); }

		//! Called before the given scene node is rendered
		/** \param[in] node: the scene node that's about to be rendered */
		virtual void OnNodePreRender(irr::scene::ISceneNode* node) { m_DeferredPipeline.onNodePreRender(node); }

		//! Called after the the node specified in OnNodePreRender() has been rendered
		/** \param[in] node: the scene node that has just been rendered */
		virtual void OnNodePostRender(irr::scene::ISceneNode* node) { m_DeferredPipeline.onNodePostRender(node); }

		virtual inline const irr::video::SLight* getLightData() { return m_LightData; }
		virtual inline void setLightData(const irr::video::SLight *lightData) { m_LightData = lightData; }

		//virtual inline const SViewFrustum* getViewFrustum() { return m_ViewFrustum; }
		//virtual inline void SViewFrustum* setViewFrustum(const SViewFrustum *frustum) { m_ViewFrustum = frustum; }

		virtual inline const irr::core::vector3df& getLightEyePos() { return m_LightEyePos; }
		virtual inline void setLightEyePos(const irr::core::vector3df &pos) { m_LightEyePos = pos; }

		virtual const irr::core::matrix4& getRenderViewMatrixInverse() { return m_ViewMatrixInverse; }
		virtual void setRenderViewMatrixInverse(const irr::core::matrix4& mat) { m_ViewMatrixInverse = mat; }

		//virtual inline const vector3df& getViewAbsPos() { return m_ViewAbsPos; }
		//virtual inline void setViewAbsPos(const vector3df &pos) { m_ViewAbsPos = pos; }

	protected:
		DeferredRenderPipeline &m_DeferredPipeline;
		const irr::video::SLight *m_LightData;
		irr::core::vector3df m_LightEyePos;
		irr::core::matrix4 m_ViewMatrixInverse;
	};

	irr::core::array<irr::video::IRenderTarget> m_GBuffer;
	irr::core::array<irr::video::IRenderTarget> m_FrameBuffers;

	// State variables used during rendering process
	SceneRenderCallback m_RenderCallback;
	irr::core::array<irr::scene::ISceneNode*> m_LightList;
	irr::scene::E_SCENE_NODE_RENDER_PASS m_CurRenderPass;
	irr::core::array<irr::scene::ISceneNode*> m_TransNodeList; // list of transparent nodes in rendering order
	MaterialTypeSwitcher m_MatSwitcher;

	// Mesh objects for rendering lights
	irr::scene::IMeshSceneNode* m_LightSphere;
	irr::scene::IBillboardSceneNode *m_LightBillboard, *m_LightFullscreenBillboard;

	ShaderEffectRenderer m_EffectRenderer;

	// Shadow renderering objects
	ShadowMapAllocator m_ShadowMapAlloc;
	CubeShadowMapRenderer m_CubeShadowRenderer;
	SpotShadowMapRenderer m_SpotShadowRenderer;
	float m_ShadowCascadeSizes[1];
	CascadedShadowMapRenderer m_CascadedShadowRenderer;
};

#endif