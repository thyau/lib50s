#include "..\\include\\BaseShadowMapRenderer.hpp"
#include "..\\include\\ShaderInclude.hpp"

/*
	BaseShadowMapRenderer.cpp
	Copyright (c) [ORGNAME] 2014
	Author: Timothy Yau
*/


BaseShadowMapRenderer::ShadowCameraContext BaseShadowMapRenderer::s_CameraContext;
PSBoxBlur::BLUR_PARAMS BaseShadowMapRenderer::s_BoxBlurParams;
ShaderEffectRenderer BaseShadowMapRenderer::s_EffectRenderer(NULL);

BaseShadowMapRenderer::BaseShadowMapRenderer(IVideoDriver *videoDriver, MaterialLibrary &materialLib)
	: m_SceneManager(NULL), m_VideoDriver(videoDriver), m_MaterialLib(materialLib), m_ShadowCamera(NULL)
{
	m_VideoDriver->grab();

	s_EffectRenderer.setVideoDriver(videoDriver);

	// Set up shadow materials
	MaterialLibrary::MATERIAL_DEFINITION def;
	def.gpuShadingLanguage = EGSL_DEFAULT;
	def.constantSetCallback = NULL;

	if (m_MaterialLib["depth"] < 0)
	{
		def.vertexShader = IVertexShader::Ptr(new VSBasic());
		def.pixelShader = IPixelShader::Ptr(new PSDepthMoments());
		def.geometryShader = NULL;
		def.baseMaterial = EMT_SOLID;
		def.materialName = "depth";

		if (m_MaterialLib.addMaterial(def) < 0)
		{
			PRINT("BaseShadowMapRenderer::constructor: failed to create depth material." << endl, RED);
		}
	}

	if (m_MaterialLib["dist"] < 0)
	{
		def.vertexShader = IVertexShader::Ptr(new VSBasic());
		def.pixelShader = IPixelShader::Ptr(new PSDistMoments(s_CameraContext));
		def.geometryShader = NULL;
		def.baseMaterial = EMT_SOLID;
		def.materialName = "dist";

		if (m_MaterialLib.addMaterial(def) < 0)
		{
			PRINT("BaseShadowMapRenderer::constructor: failed to create dist material." << endl, RED);
		}
	}

	if (m_MaterialLib["shadow_blur"] < 0)
	{
		def.vertexShader = IVertexShader::Ptr(new VSNormCoords());
		def.pixelShader = IPixelShader::Ptr(new PSBoxBlur(s_BoxBlurParams, &s_EffectRenderer));
		def.geometryShader = NULL;
		def.baseMaterial = EMT_SOLID;
		def.materialName = "shadow_blur";

		if (m_MaterialLib.addMaterial(def) < 0)
		{
			PRINT("BaseShadowMapRenderer::constructor: failed to create depth material." << endl, RED);
		}
	}
}

BaseShadowMapRenderer::~BaseShadowMapRenderer()
{
	if (m_ShadowCamera)
		m_ShadowCamera->remove();

	if (m_SceneManager)
		m_SceneManager->drop();

	m_VideoDriver->drop();
}

void BaseShadowMapRenderer::setSceneManager(ISceneManager *sceneManager)
{
	if (sceneManager != m_SceneManager)
	{
		if (m_ShadowCamera)
		{
			m_ShadowCamera->remove();
			m_ShadowCamera = NULL;
		}

		if (m_SceneManager)
			m_SceneManager->drop();

		m_SceneManager = sceneManager;

		if (m_SceneManager)
		{
			m_SceneManager->grab();
			m_ShadowCamera = m_SceneManager->addCameraSceneNode(m_SceneManager->getRootSceneNode(), ZERO_VECTOR, ZERO_VECTOR, -1, false);
		}
	}
}

BaseShadowMapRenderer::ShadowRenderCallback::ShadowRenderCallback(MaterialLibrary &materialLib, bool isOrthographic)
	: m_CurRenderPass(ESNRP_NONE), m_MaterialLib(materialLib), m_IsOrtho(isOrthographic)
{
	m_DistMat = m_MaterialLib["dist"];
	m_DepthMat = m_MaterialLib["depth"];
	m_NullMat = m_MaterialLib["null"];
}

void BaseShadowMapRenderer::ShadowRenderCallback::OnPreRender(core::array<ISceneNode*> &lightList)
{
}

void BaseShadowMapRenderer::ShadowRenderCallback::OnPostRender(void)
{
}

void BaseShadowMapRenderer::ShadowRenderCallback::OnRenderPassPreRender(E_SCENE_NODE_RENDER_PASS renderPass)
{
	m_CurRenderPass = renderPass;
}

void BaseShadowMapRenderer::ShadowRenderCallback::OnRenderPassPostRender(E_SCENE_NODE_RENDER_PASS renderPass)
{
}

void BaseShadowMapRenderer::ShadowRenderCallback::OnNodePreRender(ISceneNode* node)
{
	if (m_CurRenderPass == ESNRP_SOLID)
	{
		//m_MatSwitcher.switchMaterialTypes(node, m_MaterialLib["depth"]);

		if (m_IsOrtho)
		{
			m_MatSwitcher.switchMaterialTypes(node, m_DepthMat);
			// TESTING
			//m_MatSwitcher.switchMaterialTypes(node, m_MaterialLib["d_solid"]);
		}
		else
		{
			m_MatSwitcher.switchMaterialTypes(node, m_DistMat);
		}
		//m_MatSwitcher.switchMaterialTypes(node, m_MaterialLib["null"]);
		//m_MatSwitcher.switchMaterialTypes(node, m_MaterialLib["d_solid"]);
	}
	else if (m_CurRenderPass == ESNRP_TRANSPARENT)
	{
		// TODO: make a proper material for rendering depth with alpha
		m_MatSwitcher.switchMaterialTypes(node, m_NullMat);
	}
}

void BaseShadowMapRenderer::ShadowRenderCallback::OnNodePostRender(ISceneNode* node)
{
	if ((m_CurRenderPass == ESNRP_SOLID) || (m_CurRenderPass == ESNRP_TRANSPARENT))
	{
		m_MatSwitcher.restoreMaterialTypes(node);
	}
}

void BaseShadowMapRenderer::ShadowCameraContext::setCameraNode(ICameraSceneNode *node)
{
	m_CameraNode = node;

	if (m_CameraNode)
	{
		m_CameraMinDist = m_CameraNode->getNearValue();

		// Compute max distance to outer corner of view frustum
		irr::core::vector3df farRightPoint;
		farRightPoint.Z = m_CameraNode->getFarValue();
		farRightPoint.Y = farRightPoint.Z * std::tan(m_CameraNode->getFOV() / 2.0f);
		farRightPoint.X = farRightPoint.Y * m_CameraNode->getAspectRatio();

		//PRINT("Far right point is " << farRightPoint.X << ", " << farRightPoint.Y << ", " << farRightPoint.Z << endl, WHITE);

		m_CameraMaxDist = farRightPoint.getLength();
	}
	else 
	{
		m_CameraMinDist = m_CameraMaxDist = 0;
	}
}