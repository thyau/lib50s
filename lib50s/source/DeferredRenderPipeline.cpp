#include <assert.h>
#include <cfloat>

#include "..\\include\\DeferredRenderPipeline.hpp"
#include "..\\include\\ShaderInclude.hpp"
#include "..\\include\\WindowsConsole.hpp"

/*
	DeferredRenderPipeline.cpp
	Copyright (c) [ORGNAME] 2014
	Author: Timothy Yau
*/

using namespace std;

using namespace irr;
using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;

DeferredRenderPipeline::DeferredRenderPipeline(IVideoDriver *videoDriver)
	: BaseRenderPipeline(videoDriver), m_RenderCallback(*this), m_LightSphere(NULL), m_EffectRenderer(videoDriver),
	m_ShadowMapAlloc(videoDriver), m_CubeShadowRenderer(videoDriver, m_MaterialLib), m_SpotShadowRenderer(videoDriver, m_MaterialLib),
	m_CascadedShadowRenderer(videoDriver, m_MaterialLib, std::vector<float>(1, 1.0f) /* Too bad we don't have C++11 uniform initialization*/)
{
	// TESTING
	std::vector<float> cascadeSizes;
	/*cascadeSizes.push_back(12.0f);
	cascadeSizes.push_back(24.0f);
	cascadeSizes.push_back(72.0f);*/
	cascadeSizes.push_back(6.0f);
	cascadeSizes.push_back(12.0f);
	cascadeSizes.push_back(36.0f);
	//cascadeSizes.push_back(125.0f);
	m_CascadedShadowRenderer.setCascadeSizes(cascadeSizes);
}


DeferredRenderPipeline::~DeferredRenderPipeline(void)
{
}

RESULT DeferredRenderPipeline::init()
{
	RESULT res = R_OK;

	if ((res = allocateGBuffer(m_VideoDriver->getCurrentRenderTargetSize())) != R_OK)
	{
		PRINT("DeferredRenderPipeline::init(): Failed to allocate the G-Buffer" << endl, RED);
		return res;
	}

	if ((res = allocateFrameBuffers(m_VideoDriver->getCurrentRenderTargetSize())) != R_OK)
	{
		PRINT("DeferredRenderPipeline::init(): Failed to allocate the frame buffer" << endl, RED);
		return res;
	}

	if ((res = loadMaterials()) != R_OK)
		return res;


	return res;
}

void DeferredRenderPipeline::run()
{
	if (m_SceneManager)
	{
		m_SceneManager->setLightManager(&m_RenderCallback);
		m_SceneManager->drawAll();
		m_SceneManager->setLightManager(NULL);
	}

	lightingPass();
	postProcessPass();

	doSetFinalRenderTarget(false, false);

	//// Debug: draw the G-Buffer
	//recti bufRect = recti(vector2di(0, 0), m_GBuffer[0].RenderTexture->getSize());
	//recti bufRectSmall;
	//dimension2du bufDim;
	//for(irr::u32 i= 0; i < m_GBuffer.size(); i++)
 //   {
	//	bufDim = m_GBuffer[i].RenderTexture->getSize();
	//	bufRectSmall = recti(vector2di((int)(bufDim.Width/4.0*i), 0), vector2di((int)(bufDim.Width/4.0*(i+1)), (int)(bufDim.Height/4.0)));
	//	m_VideoDriver->draw2DImage(m_GBuffer[i].RenderTexture, bufRectSmall, bufRect);
	//}

	//// Debug: draw shadow map
	//ITexture *smTex = m_CascadedShadowRenderer.getRenderedTexture();
	//if (smTex)
	//{
	//	bufRect = recti(vector2di(0, 0), smTex->getSize());
	//	bufRectSmall = recti(vector2di((int)(bufDim.Width/4.0*3), 0), vector2di((int)(bufDim.Width/4.0*3 + bufDim.Height/4.0), (int)(bufDim.Height/4.0)));
	//	m_VideoDriver->draw2DImage(smTex, bufRectSmall, bufRect);
	//}
}

void DeferredRenderPipeline::setSceneManager(ISceneManager *sceneManager)
{
	if (m_SceneManager) {
		removeLightMeshes();
		m_SceneManager->setLightManager(NULL);
	}

	BaseRenderPipeline::setSceneManager(sceneManager);

	createLightMeshes(sceneManager);
}

RESULT DeferredRenderPipeline::allocateGBuffer(dimension2du dim)
{
	ITexture *tex;

	tex = m_VideoDriver->addRenderTargetTexture(dim, "gbuffer_color", ECF_A8R8G8B8);
	if (!tex) { return R_E_FAIL; }
	m_GBuffer.push_back(IRenderTarget(tex));

	tex = m_VideoDriver->addRenderTargetTexture(dim, "gbuffer_normal", ECF_A16B16G16R16F);
	//tex = m_VideoDriver->addRenderTargetTexture(dim, "gbuffer_normal", ECF_A8R8G8B8);
	if (!tex) { return R_E_FAIL; }
	m_GBuffer.push_back(IRenderTarget(tex));

	tex = m_VideoDriver->addRenderTargetTexture(dim, "gbuffer_depth", ECF_G16R16F);
	if (!tex) { return R_E_FAIL; }
	m_GBuffer.push_back(IRenderTarget(tex));

	return R_OK;
}

RESULT DeferredRenderPipeline::allocateFrameBuffers(dimension2du dim)
{
	ITexture *tex;

	//TODO: Additional render targets for, e.g., bloom effect?

	tex = m_VideoDriver->addRenderTargetTexture(dim, "hdr_frame_buffer", ECF_A16B16G16R16F);
	//tex = m_VideoDriver->addTexture(dim, "hdr_frame_buffer", ECF_A16B16G16R16F);
	if (!tex) { return R_E_FAIL; }
	m_FrameBuffers.push_back(IRenderTarget(tex));

	PRINT("HDR frame buffer has mip maps? " << tex->hasMipMaps() << endl, WHITE);
	int mipDataSize = 0;
	int maxDim = max(dim.Width, dim.Height);
	int pixSize = 8; // Bytes, from texture format above
	while (maxDim >= 1)
	{
		maxDim = maxDim >> 1;
		mipDataSize += maxDim * maxDim;
	}
	PRINT("Allocating mip map data of " << dec << mipDataSize << " bytes" <<endl, WHITE);
	byte* mipData = new byte[pixSize * mipDataSize];
	tex->regenerateMipMapLevels(mipData);
	PRINT("HDR frame after regenerateMipMapLevels has mip maps? " << tex->hasMipMaps() << endl, WHITE);

	return R_OK;
}

RESULT DeferredRenderPipeline::loadMaterials()
{
	MaterialLibrary::MATERIAL_DEFINITION def;
	def.gpuShadingLanguage = EGSL_DEFAULT;
	def.constantSetCallback = NULL;

	/***** Materials for objects *****/

	// Dummy material that does nothing. See onNodePreRender()
	def.vertexShader = IVertexShader::Ptr(new VSNull());
	def.pixelShader = IPixelShader::Ptr(new PSNull());
	def.geometryShader = NULL;
	def.baseMaterial = EMT_SOLID;
	def.materialName = "null";

	if (m_MaterialLib.addMaterial(def) < 0) { return R_E_FAIL; }


	// A basic material with a single texture, interpolated normal, and uniform specular
	def.vertexShader = IVertexShader::Ptr(new VSNormal());
	def.pixelShader = IPixelShader::Ptr(new PSDiffuse());
	def.geometryShader = NULL;
	def.baseMaterial = EMT_SOLID;
	def.materialName = "d_solid";

	if (m_MaterialLib.addMaterial(def) < 0) { return R_E_FAIL; }


	// Normal-mapped material with a single texture and uniform specular
	def.vertexShader = IVertexShader::Ptr(new VSTangent());
	def.pixelShader = IPixelShader::Ptr(new PSNormalMap());
	def.geometryShader = NULL;
	def.baseMaterial = EMT_SOLID;
	def.materialName = "d_solid_normal";

	if (m_MaterialLib.addMaterial(def) < 0) { return R_E_FAIL; }


	// Normal- and specular-mapped material with a single texture
	def.vertexShader = IVertexShader::Ptr(new VSTangent());
	def.pixelShader = IPixelShader::Ptr(new PSSpecularMap());
	def.geometryShader = NULL;
	def.baseMaterial = EMT_SOLID;
	def.materialName = "d_solid_normal_spec";

	if (m_MaterialLib.addMaterial(def) < 0) { return R_E_FAIL; }



	/***** Materials for lights *****/

	// Point (omni-directional) light using Blinn-Phong shading model
	def.vertexShader = IVertexShader::Ptr(new VSBasic());
	def.pixelShader = IPixelShader::Ptr(new PSLPoint(m_RenderCallback, NULL));
	def.geometryShader = NULL;
	def.baseMaterial = EMT_TRANSPARENT_ADD_COLOR;
	def.materialName = "dl_point";

	if (m_MaterialLib.addMaterial(def) < 0) { return R_E_FAIL; }

	// Same as above but with shadows
	def.vertexShader = IVertexShader::Ptr(new VSBasic());
	def.pixelShader = IPixelShader::Ptr(new PSLPoint(m_RenderCallback, &m_CubeShadowRenderer));
	def.geometryShader = NULL;
	def.baseMaterial = EMT_TRANSPARENT_ADD_COLOR;
	def.materialName = "dl_point_shadow";

	if (m_MaterialLib.addMaterial(def) < 0) { return R_E_FAIL; }


	// Spot (cone) light using Blinn-Phong shading model
	def.vertexShader = IVertexShader::Ptr(new VSBasic());
	def.pixelShader = IPixelShader::Ptr(new PSLSpot(m_RenderCallback, NULL));
	def.geometryShader = NULL;
	def.baseMaterial = EMT_TRANSPARENT_ADD_COLOR;
	def.materialName = "dl_spot";

	if (m_MaterialLib.addMaterial(def) < 0) { return R_E_FAIL; }

	// Same as above but with shadows
	def.vertexShader = IVertexShader::Ptr(new VSBasic());
	def.pixelShader = IPixelShader::Ptr(new PSLSpot(m_RenderCallback, &m_SpotShadowRenderer));
	def.geometryShader = NULL;
	def.baseMaterial = EMT_TRANSPARENT_ADD_COLOR;
	def.materialName = "dl_spot_shadow";

	if (m_MaterialLib.addMaterial(def) < 0) { return R_E_FAIL; }


	// Directional light using Blinn-Phong shading model
	def.vertexShader = IVertexShader::Ptr(new VSBasic());
	def.pixelShader = IPixelShader::Ptr(new PSLDirectional(m_RenderCallback, NULL));
	def.geometryShader = NULL;
	def.baseMaterial = EMT_TRANSPARENT_ADD_COLOR;
	def.materialName = "dl_dir";

	if (m_MaterialLib.addMaterial(def) < 0) { return R_E_FAIL; }

	// Same as above but with shadows
	def.vertexShader = IVertexShader::Ptr(new VSBasic());
	def.pixelShader = IPixelShader::Ptr(new PSLDirectional(m_RenderCallback, &m_CascadedShadowRenderer));
	def.geometryShader = NULL;
	def.baseMaterial = EMT_TRANSPARENT_ADD_COLOR;
	def.materialName = "dl_dir_shadow";

	if (m_MaterialLib.addMaterial(def) < 0) { return R_E_FAIL; }

	return R_OK;
}

void DeferredRenderPipeline::createLightMeshes(ISceneManager *mgr)
{
	m_LightSphere = mgr->addSphereSceneNode(1.0, 12);
	m_LightSphere->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, false);
	m_LightSphere->setMaterialFlag(irr::video::EMF_FRONT_FACE_CULLING, true);
	m_LightSphere->setMaterialFlag(irr::video::EMF_ZWRITE_ENABLE, false);
	m_LightSphere->setMaterialFlag(irr::video::EMF_ZBUFFER, false);
	m_LightSphere->setVisible(false);

	// Light contributions are added together
	// Need to set this despite EMT_TRANSPARENT_ADD_COLOR base material, see http://irrlicht.sourceforge.net/forum/viewtopic.php?f=4&t=48998
	m_LightSphere->getMaterial(0).BlendOperation = EBO_ADD;

	setGBufferTextures(m_LightSphere);


	m_LightBillboard = mgr->addBillboardSceneNode();
	m_LightBillboard->setSize(dimension2df(1.0f, 1.0f));
	m_LightBillboard->setMaterialFlag(irr::video::EMF_ZWRITE_ENABLE, false);
	m_LightBillboard->setMaterialFlag(irr::video::EMF_ZBUFFER, false);
	m_LightBillboard->setVisible(false);
	m_LightBillboard->setAutomaticCulling(false);

	m_LightBillboard->getMaterial(0).BlendOperation = EBO_ADD;

	setGBufferTextures(m_LightBillboard);


	// Billboard will fill the entire screen. Effect is like the full-screen quad rendered by 
	// ShaderEffectRenderer, but in world coordinates.
	m_LightFullscreenBillboard = mgr->addBillboardSceneNode();
	m_LightFullscreenBillboard->setMaterialFlag(irr::video::EMF_ZWRITE_ENABLE, false);
	m_LightFullscreenBillboard->setMaterialFlag(irr::video::EMF_ZBUFFER, false);
	m_LightFullscreenBillboard->setVisible(false);

	m_LightFullscreenBillboard->getMaterial(0).BlendOperation = EBO_ADD;

	setGBufferTextures(m_LightFullscreenBillboard);
}

void DeferredRenderPipeline::removeLightMeshes()
{
	if (m_LightSphere)
		m_LightSphere->remove();

	if (m_LightFullscreenBillboard)
		m_LightFullscreenBillboard->remove();
}

void DeferredRenderPipeline::setGBufferTextures(ISceneNode *node)
{
	for (u32 i = 0; i < m_GBuffer.size(); i++)
	{
		node->setMaterialTexture(i, m_GBuffer[i].RenderTexture);
	}
}

void DeferredRenderPipeline::onPreRender(core::array<ISceneNode*> &lightList)
{
	// TODO: sort by dist and remove faraway lights

	m_LightList = lightList;
	m_TransNodeList.clear();
}

void DeferredRenderPipeline::onPostRender(void)
{
}

void DeferredRenderPipeline::onRenderPassPreRender(E_SCENE_NODE_RENDER_PASS renderPass)
{
	m_CurRenderPass = renderPass;

	if (m_CurRenderPass == ESNRP_SOLID)
	{
		m_VideoDriver->setRenderTarget(m_GBuffer);
	}
}

void DeferredRenderPipeline::onRenderPassPostRender(E_SCENE_NODE_RENDER_PASS renderPass)
{
	m_CurRenderPass = renderPass;
}

void DeferredRenderPipeline::onNodePreRender(ISceneNode* node)
{
	if (m_CurRenderPass == ESNRP_TRANSPARENT)
	{
		// This is a bad hack to disable the transparent rendering pass in CSceneManager.
		// I couldn't find any other way to prevent transparent objects from being rendered
		// while keeping them set to visible so the scene manager still performs position
		// updates and culling on them.
		// TODO: request a feature from Irrlicht to ask the scene manager to render a specific pass

		// TODO: Can we use the override material?? See COpenGLDriver::runOcclusionQuery

		m_MatSwitcher.switchMaterialTypes(node, m_MaterialLib["null"]);
		m_TransNodeList.push_back(node);
	}
}

void DeferredRenderPipeline::onNodePostRender(ISceneNode* node)
{
	if (m_CurRenderPass == ESNRP_TRANSPARENT)
	{
		m_MatSwitcher.restoreMaterialTypes(node);
	}
}

void DeferredRenderPipeline::lightingPass()
{
	// All solid (non-transparent) nodes have been rendered into G-Buffer, now apply lighting
	//m_RenderCallback.setViewFrustum(m_SceneManager->getActiveCamera()->getViewFrustum());
	//m_RenderCallback.setViewAbsPos(m_SceneManager->getActiveCamera()->getAbsolutePosition());

	// Determines extinction radius, the distance to which lights will be rendered
	float intensityThreshold = 1.0f / 255;

	m_CubeShadowRenderer.setSceneManager(m_SceneManager);
	m_SpotShadowRenderer.setSceneManager(m_SceneManager);
	m_CascadedShadowRenderer.setSceneManager(m_SceneManager);

	doSetFinalRenderTarget();
	//m_VideoDriver->setRenderTarget(m_VideoDriver->findTexture("hdr_frame_buffer"), true, true);

	E_MATERIAL_TYPE pointLightMat = m_MaterialLib["dl_point"];
	E_MATERIAL_TYPE pointLightShadowMat = m_MaterialLib["dl_point_shadow"];
	E_MATERIAL_TYPE spotLightMat = m_MaterialLib["dl_spot"];
	E_MATERIAL_TYPE spotLightShadowMat = m_MaterialLib["dl_spot_shadow"];
	E_MATERIAL_TYPE dirLightMat = m_MaterialLib["dl_dir"];
	E_MATERIAL_TYPE dirLightShadowMat = m_MaterialLib["dl_dir_shadow"];

	matrix4 viewMatrix = m_VideoDriver->getTransform(ETS_VIEW);
	matrix4 viewMatrixInverse;
	viewMatrix.getInverse(viewMatrixInverse);
	m_RenderCallback.setRenderViewMatrixInverse(viewMatrixInverse);

	updateFullscreenBillboard(m_SceneManager->getActiveCamera());

	//PRINT("Rendering " << m_LightList.size() << " lights" << endl, WHITE);
	for (u32 i = 0; i < m_LightList.size(); i++)
	{
		// THIS COULD BE BAD. We can't be sure that the nodes in the light list are actually ILightSceneNode's
		ILightSceneNode *lightNode = (ILightSceneNode*)m_LightList[i];
		const SLight &lightData = lightNode->getLightData();

		vector3df lightEyePos;
		viewMatrix.transformVect(lightEyePos, lightNode->getAbsolutePosition());
		m_RenderCallback.setLightEyePos(lightEyePos);
		m_RenderCallback.setLightData(&lightData);

		ISceneNode *lightMeshNode;

		if ((lightData.Type == irr::video::ELT_POINT) || (lightData.Type == irr::video::ELT_SPOT))
		{
			float extRadius = computeExtinctionRadius(lightData, intensityThreshold);
			float lightDist = lightEyePos.getLength();
			// Using inequality sqrt(a + b) <= sqrt(a) + sqrt(b) for speed
			float margin = m_LightFullscreenBillboard->getSize().Width + m_LightFullscreenBillboard->getSize().Height + 
				m_SceneManager->getActiveCamera()->getNearValue();

			if (lightDist > (extRadius + margin))
			{
				// If distance to light is far enough, use a billboard instead to avoid the back of the light sphere
				// getting clipped by the far plane
				// "far enough" means that the billboard placed at the light position won't still be on-screen when
				// it gets clipped by the near plane as the camera rotates
				lightMeshNode = m_LightBillboard;
				m_LightBillboard->setSize(dimension2df(extRadius * 2, extRadius * 2));

				lightMeshNode->setPosition(lightNode->getAbsolutePosition());
				lightMeshNode->updateAbsolutePosition();
			}
			else if (lightDist + extRadius > m_SceneManager->getActiveCamera()->getFarValue())
			{
				// Light radius is too big, and is in danger of being clipped by the far plane
				// Conservatively switch to full-screen billboard
				lightMeshNode = m_LightFullscreenBillboard;
			}
			else
			{
				lightMeshNode = m_LightSphere;
				
				lightMeshNode->setPosition(lightNode->getAbsolutePosition());
				// Need to setScale before updateAbsolutePosition to get proper updated size
				lightMeshNode->setScale(irr::core::vector3df(extRadius));
				lightMeshNode->updateAbsolutePosition();

				// Let's not worry about near plane clipping, since the camera must be close to the
				// edge of the sphere for this to happen, which means the light intensity is very low
				// anyway.
			}
		}
		else
		{
			lightMeshNode = m_LightFullscreenBillboard;
		}

		if(lightData.Type == irr::video::ELT_POINT)
		{
			//lightMeshNode = m_LightBillboard;

			//lightMeshNode->setPosition(lightNode->getAbsolutePosition());
			////TODO: calculate the proper extinction radius from falloff values
			//// Need to setScale before updateAbsolutePosition to get proper updated size
			////lightMeshNode->setScale(irr::core::vector3df(lightData.Radius*10.0f));
			//lightMeshNode->updateAbsolutePosition();

			//lightMeshNode = m_LightFullscreenBillboard;

			if (lightData.CastShadows)
			{
				m_CubeShadowRenderer.renderShadows(lightNode, m_ShadowMapAlloc);
				lightMeshNode->setMaterialTexture(3, m_CubeShadowRenderer.getRenderedTexture());
				lightMeshNode->setMaterialType(pointLightShadowMat);
			}
			else 
			{
				lightMeshNode->setMaterialType(pointLightMat);
			}
		}
		else if (lightData.Type == irr::video::ELT_SPOT)
		{
			// TODO: Make a more efficient mesh for spot lights
			//lightMeshNode = m_LightSphere;

			if (lightData.CastShadows)
			{
				m_SpotShadowRenderer.renderShadows(lightNode, m_ShadowMapAlloc);
				lightMeshNode->setMaterialTexture(3, m_SpotShadowRenderer.getRenderedTexture());
				lightMeshNode->setMaterialType(spotLightShadowMat);
			}
			else
			{
				lightMeshNode->setMaterialType(spotLightMat);
			}
		}
		else if (lightData.Type == irr::video::ELT_DIRECTIONAL)
		{
			// testing
			//m_LightFullscreenBillboard->setColor(SColor(20, 0, 255, 0));

			if (lightData.CastShadows)
			{
				m_CascadedShadowRenderer.renderShadows(lightNode, m_ShadowMapAlloc);
				lightMeshNode->setMaterialTexture(3, m_CascadedShadowRenderer.getRenderedTexture());
				lightMeshNode->setMaterialType(dirLightShadowMat);
			}
			else
			{
				lightMeshNode->setMaterialType(dirLightMat);
			}
		}


		if (lightData.CastShadows)
		{
			// Reset the camera projection and view matrices after rendering shadows
			// This is necessary to correctly compute pixel position from G-Buffer depth
			m_SceneManager->getActiveCamera()->render();

			// Set render target back, since it may have been changed during shadow rendering
			//m_VideoDriver->setRenderTarget(m_VideoDriver->findTexture("hdr_frame_buffer"), false, false);
			doSetFinalRenderTarget();
		}

		lightMeshNode->render();

		// I tried rendering a full-screen quad instead of a sphere, but it doesn't work (need eye coords) and it's not faster
		//m_EffectRenderer.render(m_LightSphere->getMaterial(0).MaterialType, 3);
	}
}

void DeferredRenderPipeline::postProcessPass()
{
	// TODO: apply post-processing effects

}

void DeferredRenderPipeline::updateFullscreenBillboard(const ICameraSceneNode *camera)
{
	// Set up the billboard on the near plane
	const SViewFrustum *viewFrustum = camera->getViewFrustum();
	vector3df leftUp = viewFrustum->getNearLeftUp();
	vector3df rightUp = viewFrustum->getNearRightUp();
	vector3df rightDown = viewFrustum->getNearRightDown();

	vector3df center = (leftUp + rightDown) / 2.0f;
	vector3df right = (rightUp + rightDown) / 2.0f;
	vector3df top = (leftUp + rightUp) / 2.0f;

	m_LightFullscreenBillboard->setPosition(center);
	m_LightFullscreenBillboard->setSize(dimension2df((right - center).getLength() * 2.0f,
		(top - center).getLength() * 2.0f));
	m_LightFullscreenBillboard->updateAbsolutePosition();
}

float DeferredRenderPipeline::computeExtinctionRadius(const SLight &lightData, float threshold)
{
	// Luminance from RGB
	// TODO: ask for these coefficients to be put into Irrlicht?
	float diffInt = 0.2126f * lightData.DiffuseColor.r + 0.7152f * lightData.DiffuseColor.g + 0.0722f * lightData.DiffuseColor.b;
	//float specInt = 0.2126f * lightData.SpecularColor.r + 0.7152f * lightData.SpecularColor.g + 0.0722f * lightData.SpecularColor.b;

	//float maxInt = max(diffInt, specInt);
	float maxInt = diffInt;

	float tol = 1.0e-5f;
	assert(threshold > tol && "Too low threshold for light extinction can cause numerical issues");

	// See shader::lightAttenuationFactor for equation
	float a = lightData.Attenuation.Z;
	float b = lightData.Attenuation.Y;
	float c = lightData.Attenuation.X;
	float dist = FLT_MAX;

	if (abs(a) < tol)
	{
		if (b > tol)
		{
			// Possibly negative if c is too big, meaning that the light intensity is 
			// always below threshold. In this case we will return 0.
			dist = ((maxInt / threshold) - c) / b;
		}
		// else b is either zero, in which case light has infinite range, or b is negative,
		// in which case intensity goes to inf and then negative as distance increases.
		// In both cases we return FLT_MAX.
	}
	else if (a > tol)
	{
		float discrim = b * b - 4 * a * (c - maxInt / threshold);

		if (discrim < 0)
		{
			// No zero crossing; light intensity is always below threshold
			dist = 0;
		}
		else
		{
			// Possibly negative, zero will be returned
			dist = (-b + sqrt(discrim)) / (2 * a);
		}
	}
	// else a is negative, in which case intensity goes to inf and then negative as distance
	// increases. That doesn't make sense, so we will just return FLT_MAX.

	// Negative values generally mean light intensity is below threshold everywhere, just
	// return 0 instead.
	return max(0, dist);
}