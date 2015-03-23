#include "..\\include\\SpotShadowMapRenderer.hpp"
#include "..\\include\\PostProcessTechniques.hpp"

/*
	SpotShadowMapRenderer.cpp
	Copyright (c) [ORGNAME] 2014
	Author: Timothy Yau
*/

using namespace irr::core;
using namespace irr::video;
using namespace irr::scene;

SpotShadowMapRenderer::SpotShadowMapRenderer(IVideoDriver *videoDriver, MaterialLibrary &materialLib, float margin, float zNear, float zFar)
	: BaseShadowMapRenderer(videoDriver, materialLib), m_RenderCallback(materialLib), m_Margin(margin), m_ZNear(zNear), m_ZFar(zFar), m_OuterAngle(-1), m_ShadowTexture(NULL)
{
}


SpotShadowMapRenderer::~SpotShadowMapRenderer(void)
{
}

void SpotShadowMapRenderer::setSceneManager(ISceneManager *sceneManager)
{
	BaseShadowMapRenderer::setSceneManager(sceneManager);

	m_ShadowCamera->setNearValue(m_ZNear);
	m_ShadowCamera->setFarValue(m_ZFar);
	m_ShadowCamera->setAspectRatio(1.0f);
}

void SpotShadowMapRenderer::renderShadows(ILightSceneNode *lightNode, ShadowMapAllocator &alloc)
{
	if (!m_SceneManager)
		return;

	if (lightNode->getLightData().OuterCone != m_OuterAngle)
	{
		m_OuterAngle = lightNode->getLightData().OuterCone;
		float fov = 2.0f * atan(tan(m_OuterAngle) / (1 - m_Margin));
		m_ShadowCamera->setFOV(fov);
	}

	s_CameraContext.setCameraNode(m_ShadowCamera);

	std::vector<SViewFrustum> frustums;
	std::vector<ITexture*> textures;
	std::vector<recti> regions;
	frustums.push_back(*m_ShadowCamera->getViewFrustum());
	
	alloc.allocateTextures(frustums, SHADOW_MAP_ALLOC_SINGLE, textures, regions, true);
	//assert(textures.size() == 2 && regions.size() == 1 && "Shadow map allocator returned an unexpected number of textures/regions.");
	m_ShadowTexture = textures[0];
	const dimension2du &shadowTexSize = m_ShadowTexture->getSize();

	m_TexScale = vector2df((float)regions[0].getWidth() / (float)shadowTexSize.Width, (float)regions[0].getHeight() / (float)shadowTexSize.Height);
	m_TexOffset = vector2df((float)regions[0].UpperLeftCorner.X / (float)shadowTexSize.Width, 1.0f - (float)regions[0].LowerRightCorner.Y / (float)shadowTexSize.Height);

	vector3df lightPos = lightNode->getAbsolutePosition();
	vector3df lightDir = lightNode->getLightData().Direction;
	ICameraSceneNode *activeCam = m_SceneManager->getActiveCamera();
	matrix4 activeCamViewInverse;
	activeCam->getViewMatrix().getInverse(activeCamViewInverse);

	m_SceneManager->setLightManager(&m_RenderCallback);
	
	m_ShadowCamera->setPosition(lightPos);
	m_ShadowCamera->setTarget(lightPos + lightDir);
	m_SceneManager->setActiveCamera(m_ShadowCamera);
	
	m_VideoDriver->setRenderTarget(m_ShadowTexture, true, true, SColor(0, 255, 255, 0));
	m_VideoDriver->setViewPort(regions[0]); // TODO: Irrlicht bug?? This seems to have no effect here, but it works in CubeShadowMapRenderer

	m_SceneManager->drawAll();

	m_ViewProjMatrix = m_ShadowCamera->getProjectionMatrix() * m_ShadowCamera->getViewMatrix() * activeCamViewInverse;
	
	s_EffectRenderer.setPingPongRender(textures[0], textures[1]);
	// TODO: make blur width a parameter
	PostProcessTechniques::pixelBoxBlur(s_BoxBlurParams, s_EffectRenderer, m_MaterialLib["shadow_blur"], 2);
	m_ShadowTexture = s_EffectRenderer.getLastRenderTexture();
	
	// Restore the previous state
	m_SceneManager->setActiveCamera(activeCam);

	m_SceneManager->setLightManager(NULL);
}