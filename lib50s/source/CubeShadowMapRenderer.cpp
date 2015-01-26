#include "..\\include\\CubeShadowMapRenderer.hpp"
#include "..\\include\\PostProcessTechniques.hpp"

/*
	CubeShadowMapRenderer.cpp
	Copyright (c) [ORGNAME] 2014
	Author: Timothy Yau
*/

CubeShadowMapRenderer::CubeShadowMapRenderer(IVideoDriver *videoDriver, MaterialLibrary &materialLib, float margin, float zNear, float zFar)
	: BaseShadowMapRenderer(videoDriver, materialLib), m_RenderCallback(materialLib), m_Margin(margin), m_ShadowTexture(NULL)
{
	m_AspectRatio = 1.0f;
	m_ZNear = zNear;
	m_ZFar = zFar;
	// Leave m_Margin percent of pixels around the edges
	m_Fovy = 2.0f * atan(1.0f / (1 - m_Margin));

	//m_ProjectionMatrix.buildProjectionMatrixPerspectiveFovLH(m_Fovy, m_AspectRatio, m_ZNear, m_ZFar);

	m_ViewProjMatrices.resize(6);
}


CubeShadowMapRenderer::~CubeShadowMapRenderer(void)
{
}

void CubeShadowMapRenderer::setSceneManager(ISceneManager *sceneManager)
{
	BaseShadowMapRenderer::setSceneManager(sceneManager);

	m_ShadowCamera->setFOV(m_Fovy);
	m_ShadowCamera->setAspectRatio(m_AspectRatio);
	m_ShadowCamera->setNearValue(m_ZNear);
	m_ShadowCamera->setFarValue(m_ZFar);
}

void CubeShadowMapRenderer::renderShadows(ILightSceneNode *lightNode, ShadowMapAllocator &alloc)
{
	if (!m_SceneManager)
		return;

	s_CameraContext.setCameraNode(m_ShadowCamera);

	std::vector<SViewFrustum> frustums;
	// TODO: actually build the shadow map viewing frustums for cleverer texture allocation
	std::vector<ITexture*> textures;
	std::vector<recti> regions;

	alloc.allocateTextures(frustums, SHADOW_MAP_ALLOC_FLAT_CUBE, textures, regions, true);
	//assert(textures.size() == 2 && regions.size() == 6 && "Shadow map allocator returned an unexpected number of textures/regions.");
	m_ShadowTexture = textures[0];
	alloc.calcNormalizedRegions(textures[0], regions, m_TexOffsets, m_TexScales);

	vector3df viewDirs[] = {
		vector3df(1, 0, 0),
		vector3df(0, 1, 0),
		vector3df(0, 0, 1),
		vector3df(-1, 0, 0),
		vector3df(0, -1, 0),
		vector3df(0, 0, -1)
	};

	vector3df lightPos = lightNode->getAbsolutePosition();
	ICameraSceneNode *activeCam = m_SceneManager->getActiveCamera();
	matrix4 activeCamViewInverse;
	activeCam->getViewMatrix().getInverse(activeCamViewInverse);

	
	m_SceneManager->setLightManager(&m_RenderCallback);
	
	// Offset no longer needed with VSM shadows
	//// Polygon offset is a slope-based depth bias to increase the shadow map depth
	//// values slightly. This prevents surface acne from self-shadowing.
	//SOverrideMaterial &overrideMaterial = m_VideoDriver->getOverrideMaterial();
	//overrideMaterial.Material.PolygonOffsetFactor = 1;
	//overrideMaterial.Material.PolygonOffsetDirection = EPO_BACK;
	//overrideMaterial.EnablePasses = ESNRP_SOLID | ESNRP_TRANSPARENT;
	//overrideMaterial.EnableFlags = EMF_POLYGON_OFFSET;

	//if (m_VideoDriver->queryFeature(EVDF_POLYGON_OFFSET))
	//{
	//	PRINT("We have polygon offset feature", WHITE);
	//}
	//else
	//{
	//	PRINT("We don't have polygon offset feature", WHITE);
	//}

	// TODO: maybe adjust camera near/far depending on scene?
	//m_ShadowCamera->setProjectionMatrix(m_ProjectionMatrix);
	m_ShadowCamera->setPosition(lightPos);
	m_SceneManager->setActiveCamera(m_ShadowCamera);
	
	m_VideoDriver->setRenderTarget(m_ShadowTexture, true, true, SColor(0, 255, 255, 0));
	
	for (int i = 0; i < 6; i++)
	{
		m_VideoDriver->setViewPort(regions[i]);
		m_ShadowCamera->setTarget(lightPos + viewDirs[i]);

		m_SceneManager->drawAll();

		m_ViewProjMatrices[i] = m_ShadowCamera->getProjectionMatrix() * m_ShadowCamera->getViewMatrix() * activeCamViewInverse;
		//m_ViewProjMatrices[i] = m_ShadowCamera->getProjectionMatrix() * m_ShadowCamera->getViewMatrix();
		//m_ViewProjMatrices[i] = activeCamViewInverse;
	}

	
	s_EffectRenderer.setPingPongRender(textures[0], textures[1]);
	// TODO: make blur width a parameter
	PostProcessTechniques::pixelBoxBlur(s_BoxBlurParams, s_EffectRenderer, m_MaterialLib["shadow_blur"], 5);
	m_ShadowTexture = s_EffectRenderer.getLastRenderTexture();
	
	// Restore the previous state
	m_SceneManager->setActiveCamera(activeCam);


	//overrideMaterial.Material.PolygonOffsetFactor = 0;
	//overrideMaterial.EnablePasses = 0;
	//overrideMaterial.EnableFlags = 0;

	m_SceneManager->setLightManager(NULL);
}