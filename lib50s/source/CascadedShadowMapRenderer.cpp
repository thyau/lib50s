#include "..\\include\\CascadedShadowMapRenderer.hpp"
#include "..\\include\\PostProcessTechniques.hpp"

// Testing
#include <thread>

/*
	CascadedShadowMapRenderer.cpp
	Copyright (c) [ORGNAME] 2014
	Author: Timothy Yau
*/

using namespace std;

CascadedShadowMapRenderer::CascadedShadowMapRenderer(IVideoDriver *videoDriver, MaterialLibrary &materialLib,  const std::vector<float> &cascadeSizes)
	: BaseShadowMapRenderer(videoDriver, materialLib), m_RenderCallback(materialLib, true), m_ShadowTexture(NULL)
{
	setCascadeSizes(cascadeSizes);
}

CascadedShadowMapRenderer::~CascadedShadowMapRenderer(void)
{
	for (auto it = m_CascadeTextures.begin(); it != m_CascadeTextures.end(); it++)
	{
		(*it)->drop();
	}
}

void CascadedShadowMapRenderer::setCascadeSizes(const std::vector<float> &cascadeSizes)
{ 
	assert(cascadeSizes.size() <= sizeof(m_CascadeUpdateCount) * 8 && "Too many shadow cascades, recompile with larger update counter.");

	m_CascadeSizes = cascadeSizes; 
	m_ViewProjMatrices.resize(m_CascadeSizes.size());
	m_ShadowViewProjMatrices.resize(m_CascadeSizes.size());
	m_CascadeUpdateCount = 0;

	if (m_CascadeTextures.size() > 0)
	{
		for (auto it = m_CascadeTextures.begin(); it != m_CascadeTextures.end(); it++)
		{
			(*it)->drop();
		}

		m_CascadeTextures.clear();
	}
}

void CascadedShadowMapRenderer::renderShadows(ILightSceneNode *lightNode, ShadowMapAllocator &alloc)
{
	if (!m_SceneManager)
		return;

	s_CameraContext.setCameraNode(m_ShadowCamera);

	int cascadeUpdateIdx;

	std::vector<SViewFrustum> frustums;
	frustums.push_back(*m_ShadowCamera->getViewFrustum()); // TODO: This is kinda pointless right now
	
	if (m_CascadeTextures.size() == 0)
	{
		alloc.allocateTextures(frustums, SHADOW_MAP_ALLOC_CASCADE, m_CascadeTextures, m_CascadeRegions, true, m_CascadeSizes.size());
		m_ShadowTexture = m_CascadeTextures[0];
	
		// Add a margin around each region to prevent artifacts
		int margin = 7; // pixels
		std::vector<recti> cascadeRegionsInner = m_CascadeRegions;
		m_CascadeSizesFull = m_CascadeSizes;
		for (unsigned int i = 0; i < m_CascadeRegions.size(); i++)
		{
			cascadeRegionsInner[i].UpperLeftCorner += margin;
			cascadeRegionsInner[i].LowerRightCorner -= margin;
			m_CascadeSizesFull[i] += (m_CascadeSizes[i] / cascadeRegionsInner[i].getWidth()) * margin * 2;
		}

		ShadowMapAllocator::calcNormalizedRegions(m_CascadeTextures[0], cascadeRegionsInner, m_TexOffsets, m_TexScales);

		// Update all cameras
		cascadeUpdateIdx = -1;
	}
	else
	{
		// Decide which cascade to update on this frame
		cascadeUpdateIdx = 0;
		m_CascadeUpdateCount++;

		if (m_CascadeUpdateCount == 0)
		{
			// Counter wrapped around to zero
			m_CascadeUpdateCount++;
			// Avoid updating cascade 0 three times in a row
			cascadeUpdateIdx = (m_CascadeSizes.size() > 1)? 1 : 0;
		}
		else {
			// Find lowest bit set. Wish we had ffs() from POSIX
			while (((0x1 << cascadeUpdateIdx) & m_CascadeUpdateCount) == 0)
			{
				cascadeUpdateIdx++;
			}

			if (cascadeUpdateIdx >= (int)m_CascadeSizes.size())
			{
				// Avoid updating cascade 0 three times in a row
				// NOTE: this came about partially because Irrlicht has a bug where,
				// if we render the same region twice, the second setViewport() has 
				// no effect and it renders over the entire texture instead.
				cascadeUpdateIdx = (m_CascadeSizes.size() > 1)? 1 : 0;
			}
		}
	}

	vector3df lightDir = lightNode->getLightData().Direction;
	ICameraSceneNode *activeCam = m_SceneManager->getActiveCamera();
	matrix4 activeCamViewInverse;
	activeCam->getViewMatrix().getInverse(activeCamViewInverse);

	m_SceneManager->setLightManager(&m_RenderCallback);

	m_ShadowCamera->bindTargetAndRotation(true);
	m_ShadowCamera->updateAbsolutePosition();
	m_ShadowCamera->setTarget(m_ShadowCamera->getAbsolutePosition() + lightDir);
	m_ShadowCamera->updateAbsolutePosition();
	vector3df shadowRot = m_ShadowCamera->getAbsoluteTransformation().getRotationDegrees();

	std::vector<vector3df> shadowCamPositions;
	std::vector<float> zNearValues, zFarValues;

	// Fit all camera positions together because they influence each other
	fitCascadeCameras(m_SceneManager, m_CascadeRegions, shadowRot, m_CascadeSizesFull, shadowCamPositions);

	// Only fit near/far with the camera(s) being updated this frame
	if (cascadeUpdateIdx < 0)
	{
		fitCameraNearFar(m_SceneManager, shadowRot, shadowCamPositions, m_CascadeSizesFull, zNearValues, zFarValues);

		m_VideoDriver->setRenderTarget(m_ShadowTexture, true, true, SColor(0, 255, 255, 0));
	}
	else
	{
		shadowCamPositions[0] = shadowCamPositions[cascadeUpdateIdx];
		shadowCamPositions.resize(1);
		std::vector<float> cascadeSizesTmp(1, m_CascadeSizesFull[cascadeUpdateIdx]);

		// TODO: this is still too slow. Need to spread it out across frames...
		fitCameraNearFar(m_SceneManager, shadowRot, shadowCamPositions, cascadeSizesTmp, zNearValues, zFarValues);

		m_VideoDriver->setRenderTarget(m_ShadowTexture, false, true, SColor(0, 255, 255, 0));
		m_VideoDriver->draw2DRectangle(SColor(255, 255, 255, 0), m_CascadeRegions[cascadeUpdateIdx]);
	}

	m_SceneManager->setActiveCamera(m_ShadowCamera);

	// Irrlicht has an object culling bug when using orthographic cameras. Objects that should still be in the
	// viewing frustum get culled, resulting in shadows that appear and disappear. This is a temporary workaround,
	// still doesn't fully fix the issue though. See hack with near/far bias in fitCascadeCameras().
	m_ShadowCamera->setAutomaticCulling(false);

	//// Temporary polygon offset, until we implement VSM
	//SOverrideMaterial &overrideMaterial = m_VideoDriver->getOverrideMaterial();
	//overrideMaterial.Material.PolygonOffsetFactor = 1;
	//overrideMaterial.Material.PolygonOffsetDirection = EPO_BACK;
	//overrideMaterial.EnablePasses = ESNRP_SOLID | ESNRP_TRANSPARENT;
	//overrideMaterial.EnableFlags = EMF_POLYGON_OFFSET;

	matrix4 shadowProjMatFull;
	for (unsigned int i = 0; i < m_CascadeSizes.size(); i++)
	{
		unsigned int updatedIdx;
		if (cascadeUpdateIdx < 0)
		{
			updatedIdx = i;
		}
		else if (cascadeUpdateIdx == i)
		{
			updatedIdx = 0;
		}
		else {
			// Still need to update if active camera moves
			m_ViewProjMatrices[i] = m_ShadowViewProjMatrices[i] * activeCamViewInverse;
			continue;
		}

		myMakeOrthoProj(shadowProjMatFull, m_CascadeSizesFull[i], m_CascadeSizesFull[i], zNearValues[updatedIdx], zFarValues[updatedIdx]);
		myMakeOrthoProj(m_ShadowViewProjMatrices[i], m_CascadeSizes[i], m_CascadeSizes[i], zNearValues[updatedIdx], zFarValues[updatedIdx]);

		//shadowProjMat.buildProjectionMatrixOrthoLH(m_CascadeSizes[i], m_CascadeSizes[i], zNearValues[i], zFarValues[i]);
		//shadowProjMat.buildProjectionMatrixOrthoLH(m_CascadeSizes[i], m_CascadeSizes[i], 0.1f, 100.0f);
		
		m_ShadowCamera->setProjectionMatrix(shadowProjMatFull, true);
		m_ShadowCamera->setPosition(shadowCamPositions[updatedIdx]);
		m_ShadowCamera->updateAbsolutePosition();
		// Need to set target again after updating position
		m_ShadowCamera->setTarget(m_ShadowCamera->getAbsolutePosition() + lightDir);

		m_VideoDriver->setViewPort(recti(0, 0, 1, 1)); // HACK/WORKAROUND: Irrlicht bug when setting the same viewport twice
		m_VideoDriver->setViewPort(m_CascadeRegions[i]);

		m_SceneManager->drawAll();

		m_ShadowViewProjMatrices[i] *= m_ShadowCamera->getViewMatrix();
		m_ViewProjMatrices[i] = m_ShadowViewProjMatrices[i] * activeCamViewInverse;

		//PRINT("Shadow map rendered for cascade " << i << endl, WHITE);
		//this_thread::sleep_for(chrono::seconds(2));
		s_EffectRenderer.setPingPongRender(m_CascadeTextures[0], m_CascadeTextures[1], m_CascadeRegions[i], m_CascadeRegions[i]);
		s_EffectRenderer.setClearBackBuffer(false);
		s_EffectRenderer.setClearZBuffer(true);
		//// TODO: make blur width a parameter
		// NOTE: We depend on the fact that the blur uses two passes so that the final output is on m_CascadeTextures[0]
		PostProcessTechniques::pixelBoxBlur(s_BoxBlurParams, s_EffectRenderer, m_MaterialLib["shadow_blur"], 7, 
			m_CascadeSizesFull[0] / m_CascadeSizesFull[i]);

		// TESTING
		//m_ShadowTexture = m_CascadeTextures[1];

		//PRINT("Blur rendered for cascade " << i << endl, WHITE);
		//this_thread::sleep_for(chrono::seconds(2));

		// TESTING
		//break;
	}

	// Restore the previous state
	m_SceneManager->setActiveCamera(activeCam);

	m_SceneManager->setLightManager(NULL);
}


void CascadedShadowMapRenderer::fitCascadeCameras(ISceneManager *smgr, std::vector<recti> shadowMapDims, const vector3df rotation, 
												  const std::vector<float>& camSizes, std::vector<vector3df>& outCamPositions)
{
	// Algorithm: 
	// 1. Project the four points of the view frustum near plane using orthographic projection
	// 2. Project the camera center point
	// 3. For each cascade, maximize the depth along the viewing frustum that can be covered.
	//    Depth is measured along the viewing direction 
	//     3a. Divide the 2D vectors from the camera center to each of the four points by the 
	//         near plane distance. Since orthographic projection is an affine transformation,
	//         this gives the derivative of 2D point positions w.r.t. view frustum depth.
	//     3b. Compute the view frustum depth such that the bounding box of the first four
	//         points and the points extended along the vector directions has maximal dimension
	//         equal to the cascade shadow map size.
	//     3c. Consider the four extended points as if they were the new near plane and repeat
	//         step 3 for the next cascade.
	//
	// Subtleties:
	// margins, overlap, rounding camera positions to nearest texel

	ICameraSceneNode *activeCam = smgr->getActiveCamera();
	vector3df activeCamPos = activeCam->getAbsolutePosition();
	const SViewFrustum *viewFrustum = activeCam->getViewFrustum();

	matrix4 lightMatrix;
	lightMatrix.makeIdentity();
	lightMatrix.setInverseRotationDegrees(rotation);

	std::vector<vector3df> anchorPts;
	vector3df tmp;
	lightMatrix.rotateVect(tmp, viewFrustum->getNearLeftUp()); anchorPts.push_back(tmp);
	lightMatrix.rotateVect(tmp, viewFrustum->getNearRightUp()); anchorPts.push_back(tmp);
	lightMatrix.rotateVect(tmp, viewFrustum->getNearRightDown()); anchorPts.push_back(tmp);
	lightMatrix.rotateVect(tmp, viewFrustum->getNearLeftDown()); anchorPts.push_back(tmp);

	std::vector<vector3df> edgeDirs;
	vector3df activeCamPosRotated;
	float znear = activeCam->getNearValue();
	lightMatrix.rotateVect(activeCamPosRotated, activeCamPos);
	for (int i = 0; i < 4; i++)
		edgeDirs.push_back((anchorPts[i] - activeCamPosRotated) / znear);

	std::vector<vector2df> xDirs, yDirs;
	vector2df xDirMin, xDirMax, yDirMin, yDirMax;

	for (int i = 0; i < 4; i++)
	{
		xDirs.push_back(vector2df(anchorPts[i].X, edgeDirs[i].X));
		yDirs.push_back(vector2df(anchorPts[i].Y, edgeDirs[i].Y));
	}

	float scale = FLT_MAX;
	for (unsigned int i = 0; i < camSizes.size(); i++)
	{
		float xTexelSize = camSizes[i] / shadowMapDims[i].getWidth();
		float yTexelSize = camSizes[i] / shadowMapDims[i].getHeight();

		// Make it one texel smaller in each dimension so that we can snap the camera to the 
		// texel grid and still have it cover all the points
		scale = min(findExtremalPoints(xDirs, camSizes[i] - xTexelSize, xDirMin, xDirMax), 
			findExtremalPoints(yDirs, camSizes[i] - yTexelSize, yDirMin, yDirMax));

		if (scale == 0.0f)
		{
			// Means we can't fit all the anchor points
			// Make a best effort for this cascade
			assert(false && "Can't fit all anchor points. Not implemented.");
		}
		else if (scale == FLT_MAX)
		{
			// Means we can fit the entire frustum to infinite depth
			// Shouldn't happen unless active camera is using orthographic projection
			assert(false && "Not implemented");
		}
		else
		{
			float xMin, yMin, xMax, yMax;
			xMin = xDirMin.X + scale * xDirMin.Y;
			yMin = yDirMin.X + scale * yDirMin.Y;
			xMax = xDirMax.X + scale * xDirMax.Y;
			yMax = yDirMax.X + scale * yDirMax.Y;

			// TODO: need to choose Z properly
			vector3df camPos = vector3df((xMin + xMax)/2.0f, (yMin + yMax)/2.0f, 
				activeCamPosRotated.Z);

			// Snap shadow camera positions to texel grid to avoid jittering when the
			// active camera moves
			camPos.X = floor(camPos.X / xTexelSize) * xTexelSize;
			camPos.Y = floor(camPos.Y / yTexelSize) * yTexelSize;

			outCamPositions.push_back(camPos);

			for (int j = 0; j < 4; j++)
			{
				xDirs[j].X = xDirs[j].X + scale * xDirs[j].Y;
				yDirs[j].X = yDirs[j].X + scale * yDirs[j].Y;
			}
		}
	}
}

float CascadedShadowMapRenderer::findExtremalPoints(const std::vector<vector2df> &inVec, float desiredRange, vector2df &outMin, vector2df &outMax)
{
	const float tol = 1.0e-6f;

	std::vector<vector2df> vec = inVec;
	sort(vec.begin(), vec.end());

	if (computePointsRange(inVec, 0.0f) >= desiredRange)
	{
		outMin = vec[0];
		outMax = vec[vec.size() - 1];
		return 0.0f;
	}

	int minIdx = 0;

	for (unsigned int i = 1; i < vec.size(); i++)
	{
		if ((vec[i].Y >= -tol) || (vec[minIdx].Y <= vec[i].Y + tol))
			continue;

		float crossingPt = (vec[minIdx].X - vec[i].X) / (vec[i].Y - min(0, vec[minIdx].Y));
		// TODO: can avoid computePointsRange if crossingPt is less than previous crossingPt
		float range = computePointsRange(vec, crossingPt);

		if (range < desiredRange)
		{
			minIdx = i;
		}
	}

	outMin = vec[minIdx];

	float minScale, prevMinScale;
	minScale = prevMinScale = FLT_MAX;

	if (abs(vec[minIdx].Y) > tol)
	{
		minScale = prevMinScale = desiredRange / abs(vec[minIdx].Y);
		outMax = vec[minIdx];
	}

	for (unsigned int i = 0; i < vec.size(); i++)
	{
		if ((i == minIdx) || (abs(vec[minIdx].Y) <= tol && abs(vec[i].Y) <= tol))
			continue;

		if (vec[minIdx].Y > tol)
		{
			// We know vec[minIdx].X <= vec[i].X because vec[minIdx] gives minimum
			if (vec[i].Y > tol)
			{
				minScale = min(minScale, (desiredRange - (vec[i].X - vec[minIdx].X)) / vec[i].Y);
			}
		}
		else if (vec[minIdx].Y < -tol)
		{
			if (vec[i].Y <= tol)
			{
				if (vec[minIdx].X < vec[i].X)
				{
					minScale = min(minScale, (desiredRange - (vec[i].X - vec[minIdx].X)) / -vec[minIdx].Y);
				}
			}
			else // vec[i].Y > tol
			{
				if (vec[minIdx].X < vec[i].X)
				{
					minScale = min(minScale, (desiredRange - (vec[i].X - vec[minIdx].X)) / (vec[i].Y - vec[minIdx].Y));
				}
				else 
				{
					float crossingPt = (vec[minIdx].X - vec[i].X) / vec[i].Y;

					if (crossingPt < minScale)
					{
						minScale = min(minScale, crossingPt + (desiredRange - crossingPt * -vec[minIdx].Y) / (vec[i].Y - vec[minIdx].Y));
					}
				}
			}
		}
		else // -tol <= vec[minIdx].Y <= tol
		{
			// Impossible for vec[i].Y < tol, because we know that vec[minIdx] gives minimum
			// Impossible for vec[i].X < vec[minIdx].X for the same reason
			minScale = min(minScale, (desiredRange - (vec[i].X - vec[minIdx].X)) / vec[i].Y);
		}

		if (minScale != prevMinScale)
		{
			outMax = vec[i];
			prevMinScale = minScale;
		}
	}

	return minScale;
}

float CascadedShadowMapRenderer::computePointsRange(const std::vector<vector2df> &vec, float scale)
{
	float minPos, maxPos;
	minPos = maxPos = vec[0].X;

	for (unsigned int i = 0; i < vec.size(); i++)
	{
		float tmp = vec[i].X + scale * vec[i].Y;
		minPos = min(minPos, min(vec[i].X, tmp));
		maxPos = max(maxPos, max(vec[i].X, tmp));
	}

	return maxPos - minPos;
}

void CascadedShadowMapRenderer::fitCameraNearFar(ISceneManager *smgr, const vector3df rotation, std::vector<vector3df>& inoutCamPositions, 
												const std::vector<float>& camSizes, std::vector<float> &outZNear, std::vector<float> &outZFar)
{
	// Compute near/far planes for each camera
	// Algorithm:
	// 1. Get AABB bounding boxes for all objects in scene
	// 2. Project the AABB corner points using orthographic projection
	// 3. Construct the 6 bounding box faces from the points
	// 4. Intersect the projected face quads with the camera frustum (in 2D)
	//    4a. Walk along the points of the quad and find crossing points with
	//        camera frustum bounds.
	//    4b. Also determine whether each of the camera frustum corners lies
	//        within the quad.
	// 5. Find min/max Z within the intersection area
	//    5a. Linear interpolation of crossing points (see 4a)
	//    5b. Project frustum corner points (see 4b) onto plane of the quad

	// Orthographic projection is an affine transformation
	//  => convex shape remains convex
	//  => interior angles are all less than pi/2

	outZNear.resize(camSizes.size());
	outZFar.resize(camSizes.size());

	for (unsigned int i = 0; i < camSizes.size(); i++)
	{
		outZNear[i] = FLT_MAX;
		outZFar[i] = -FLT_MAX;
	}

	matrix4 lightMatrix;
	lightMatrix.makeIdentity();
	lightMatrix.setInverseRotationDegrees(rotation);

	// See aabbox3df::getEdges() to see point ordering
	const unsigned int faceIdx[6][4] = {
		{0, 1, 3, 2},
		{0, 1, 5, 4},
		{0, 2, 6, 4},
		{7, 6, 4, 5},
		{7, 5, 1, 3},
		{7, 6, 2, 3}
	};

	// Depth-first traversal to find all top-level mesh nodes
	std::stack<const ISceneNode*> nodeList;
	nodeList.push(smgr->getRootSceneNode());

	vector3df corners[8];
	float maxCamSize = camSizes.back(), minCamSize = camSizes.front();

	while (!nodeList.empty())
	{
		const ISceneNode *node = nodeList.top();
		nodeList.pop();
		ESCENE_NODE_TYPE type = node->getType();

		if (!node->isVisible())
			continue;

		if (type == ESNT_MESH || type == ESNT_ANIMATED_MESH ||
			type == ESNT_CUBE || type == ESNT_SPHERE ||
			type == ESNT_TEXT || type == ESNT_WATER_SURFACE ||
			type == ESNT_TERRAIN)
		{
			aabbox3df bbox = node->getTransformedBoundingBox();
			vector3df extent = bbox.getExtent();
			vector3df center;

			float maxDim = max(extent.X, max(extent.Y, extent.Z));
			
			// For performance reasons, we will compute intersection points with the shadow
			// camera viewport only if the object is very large compared to the viewport size.
			// Otherwise, we approximate it as a sphere large enough to contain a maxDim cube
			float radius;
			float camSizeFactor = 2.0f;
			if (maxDim <= camSizeFactor*maxCamSize)
			{
				radius = 0.866025f * maxDim;
				center = bbox.getCenter();
				lightMatrix.rotateVect(center);
			}

			if (maxDim > camSizeFactor*minCamSize)
			{
				bbox.getEdges(&corners[0]);

				for (int i = 0; i < 8; i++)
				{
					lightMatrix.rotateVect(corners[i]);
				}
			}


			for (unsigned int i = 0; i < camSizes.size(); i++)
			{
				float camSize = camSizes[i];
				float xMin, xMax, yMin, yMax, zMin, zMax;

				xMin = inoutCamPositions[i].X - camSize / 2.0f;
				xMax = inoutCamPositions[i].X + camSize / 2.0f;
				yMin = inoutCamPositions[i].Y - camSize / 2.0f;
				yMax = inoutCamPositions[i].Y + camSize / 2.0f;
				zMin = FLT_MAX;
				zMax = -FLT_MAX;

				bool isInView = false;

				if (maxDim > camSizeFactor*camSize)
				{
					vector3df subPts[4];

					for (unsigned int j = 0; j < 6; j++)
					{
						for (unsigned int k = 0; k < 4; k++)
							subPts[k] = corners[faceIdx[j][k]];

						float z1, z2;
						parallelogramIntersectDepthRange(xMin, xMax, yMin, yMax, subPts, z1, z2);
						zMin = min(z1, zMin);
						zMax = max(z2, zMax);
					}

					isInView = true;
				}
				else
				{
					isInView = !((center.X + radius) < xMin || (center.X - radius > xMax) || (center.Y + radius) < yMin || (center.Y - radius > yMax));

					if (isInView)
					{
						zMin = min(zMin, center.Z - radius);
						zMax = max(zMax, center.Z + radius);
					}
				}

				if (isInView)
				{
					outZNear[i] = min(outZNear[i], zMin);
					outZFar[i] = max(outZFar[i], zMax);
				}
			}
		}
		else
		{
			const core::list<ISceneNode*>& childList = node->getChildren();

			for (auto it = childList.begin(); it != childList.end(); it++)
			{
				nodeList.push(*it);
			}
		}
	}

	//fitCameraNearFar(smgr, rotation, outCamPositions, camSizes, outZNear, outZFar);

	for (unsigned int i = 0; i < camSizes.size(); i++)
	{
		//HACK. Is there a bug with object culling in Irrlicht when using orthographic projection?
		float bias = 30.0f;

		inoutCamPositions[i].Z = outZNear[i] - 1.0f - bias;
		outZNear[i] = 1.0f;
		outZFar[i] = outZFar[i] - inoutCamPositions[i].Z + bias;
		/*outCamPositions[i].Z = -50.1f;
		outZNear[i] = 0.1f;
		outZFar[i] = 100.0f;*/

		// Rotate back into world coordinate system
		lightMatrix.inverseRotateVect(inoutCamPositions[i]);
	}

	//PRINT("Processed " << bboxCount << " mesh nodes", WHITE);
}

void CascadedShadowMapRenderer::parallelogramIntersectDepthRange(float xMin, float xMax, float yMin, float yMax, 
																 const vector3df (&pts)[4], 
																 float &outMinDepth, float &outMaxDepth)
{
	//assert(pts.size() == 4 && "Expected 4 points defining parallelogram");

	// Compute all the intersection points of the parallelogram with the edges of the rectangle by 
	// walking the edges in order.

	outMinDepth = FLT_MAX;
	outMaxDepth = -FLT_MAX;
	for (unsigned int i = 0; i < 4; i++)
	{
		const vector3df &p1 = pts[i], &p2 = pts[(i+1) % 4];
		bool isInsideRect = false;

		if (p1.X >= xMin && p1.X <= xMax && p1.Y >= yMin && p1.Y <= yMax)
		{
			outMinDepth = min(outMinDepth, p1.Z);
			outMaxDepth = max(outMaxDepth, p1.Z);
			isInsideRect = true;
		}

		float interpZ;

		if (yLineIntersect(yMin, xMin, xMax, p1, p2, interpZ)) 
		{
			outMinDepth = min(outMinDepth, interpZ); outMaxDepth = max(outMaxDepth, interpZ);
		}

		if (xLineIntersect(xMin, yMin, yMax, p1, p2, interpZ))
		{
			outMinDepth = min(outMinDepth, interpZ);outMaxDepth = max(outMaxDepth, interpZ);
		}

		if (yLineIntersect(yMax, xMin, xMax, p1, p2, interpZ))
		{
			outMinDepth = min(outMinDepth, interpZ); outMaxDepth = max(outMaxDepth, interpZ);
		}

		if (xLineIntersect(xMax, yMin, yMax, p1, p2, interpZ))
		{
			outMinDepth = min(outMinDepth, interpZ); outMaxDepth = max(outMaxDepth, interpZ);
		}
	}

	//vector3df cornerPts[] = 
	//{
	//	vector3df(xMax, yMax, 0), vector3df(xMax, yMin, 0), vector3df(xMin, yMin, 0), vector3df(xMin, yMax, 0)
	//};

	float cornerPts[][2] = 
	{
		{xMax, yMax}, {xMax, yMin}, {xMin, yMin}, {xMin, yMax}
	};

	// Determine if corner points are within parallelogram
	// Specialized algorithm below
	vector2df v1 = vector2df(pts[0].X - pts[1].X, pts[0].Y - pts[1].Y);
	vector2df v2 = vector2df(pts[2].X - pts[1].X, pts[2].Y - pts[1].Y);
	
	// Parallelogram degenerates to a line
	if (abs(v1.X * v2.Y - v1.Y * v2.X) <= 1.0e-6f)
		return;

	float v2n = v2.getLength();
	vector2df v2u = v2 / v2n;
	vector2df v1p = v1 - v1.dotProduct(v2u) * v2u; // Get orthogonal basis
	float v1pn = v1p.getLength();
	vector2df v1pu = v1p / v1pn;

	vector3df planeNorm = pts[0] - pts[1];
	planeNorm = planeNorm.crossProduct(pts[2] - pts[1]);
	float planeDist = planeNorm.dotProduct(pts[0]);

	for (unsigned int i = 0; i < 4; i++)
	{
		vector2df p(cornerPts[i][0] - pts[1].X, cornerPts[i][1] - pts[1].Y);
		float c = p.dotProduct(v1pu);

		if (c >= 0 && c <= v1pn)
		{
			p = p - (c / v1pn) * v1;
			float d = p.dotProduct(v2u);

			if (d >= 0 && d <= v2n)
			{
				// Point is within parallelogram, project it along Z axis onto the polygon plane
				float z = (planeDist - planeNorm.X * cornerPts[i][0] - planeNorm.Y * cornerPts[i][1]) / planeNorm.Z;

				outMinDepth = min(outMinDepth, z);
				outMaxDepth = max(outMaxDepth, z);
			}
		}
	}
}