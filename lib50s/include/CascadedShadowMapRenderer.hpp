#ifndef __CASCADEDSHADOWMAPRENDERER_HPP__
#define __CASCADEDSHADOWMAPRENDERER_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Renders cascaded shadow maps. 
 */

#include <vector>
#include <cfloat>

#include "..\\include\\Common.hpp"
#include "..\\include\\BaseShadowMapRenderer.hpp"

class CascadedShadowMapRenderer : public BaseShadowMapRenderer
{
public:
	// cascadeSizes specifies the dimensions of each shadow map in world units, in order of increasing size.
	// Each shadow map will be a square, and will not be scaled in order to avoid jitter in the shadows.
	// This means that depending on the camera field of view, it may not be possible for the shadow maps
	// to cover the view frustum to a reasonable depth. Therefore the cascade sizes should be chosen
	// according to the field of view.
	CascadedShadowMapRenderer(IVideoDriver *videoDriver, MaterialLibrary &materialLib, const std::vector<float> &cascadeSizes);
	virtual ~CascadedShadowMapRenderer(void);

	virtual const std::vector<float>& getCascadeSizes() { return m_CascadeSizes; }
	virtual void setCascadeSizes(const std::vector<float> &cascadeSizes);

	virtual void renderShadows(ILightSceneNode *lightNode, ShadowMapAllocator &alloc) override;

	// Only valid after a call to renderShadows().
	virtual ITexture* getRenderedTexture() { return m_ShadowTexture; }
	// Only valid after a call to renderShadows().
	virtual const std::vector<matrix4>& getViewProjectionMatrices() { return m_ViewProjMatrices; }

	// Get X/Y offsets for sampling from the texture
	virtual const std::vector<vector2df>& getTexOffsets() { return m_TexOffsets; }
	// Get X/Y scales for sampling from the texture
	virtual const std::vector<vector2df>& getTexScales() { return m_TexScales; }

protected:
	// Fit orthographic projection cameras for the shadow map cascades to the active camera view frustum
	// using the currently set m_CascadeSizes. shadowMapDims gives the pixel dimensions of each cascade
	// shadow map, corresponding to m_CascadeSizes.
	// rotation specifies the rotation of all shadow cameras.
	// Note that outCamPositions is still in orthographic projection space to facilitate use with fitCameraNearFar
	virtual void fitCascadeCameras(ISceneManager *smgr, std::vector<recti> shadowMapDims, const vector3df rotation, 
		const std::vector<float>& camSizes, std::vector<vector3df>& outCamPositions);

	virtual float findExtremalPoints(const std::vector<vector2df> &vec, float desiredRange, vector2df &outMin, vector2df &outMax);
	virtual float computePointsRange(const std::vector<vector2df> &vec, float scale);

	// inoutCamPositions camera position X/Y should be set in the orthographic projection space (i.e. after applying rotation),
	// and the Z coordinate will be adjusted. Then the camera positions will be rotated back into world space.
	virtual void fitCameraNearFar(ISceneManager *smgr, const vector3df rotation, std::vector<vector3df>& inoutCamPositions, 
		const std::vector<float>& camSizes, std::vector<float> &outZNear, std::vector<float> &outZFar);

	// Intersect a parallelogram with the axis-aligned rectangle given by (xMin,yMin), (xMax,yMax), and
	// find the minimum and maximum z-values of all points within the intersection area.
	// The parallelogram is defined by points pts[0..3] which are joined by edges in that order
	virtual void parallelogramIntersectDepthRange(float xMin, float xMax, float yMin, float yMax, 
		const vector3df (&pts)[4], float &outMinDepth, float &outMaxDepth);

	static inline bool xLineIntersect(float x, float yMin, float yMax, const vector3df &p1, const vector3df &p2, float &interpZ)
	{
		if ((p1.X < x) != (p2.X < x))
		{
			float alpha = (x - p1.X) / (p2.X - p1.X);
			float iY = p1.Y + alpha * (p2.Y - p1.Y);

			if ((iY >= yMin) && (iY <= yMax))
			{
				interpZ = p1.Z + alpha * (p2.Z - p1.Z);
				return true;
			}
		}

		return false;
	}

	static inline bool yLineIntersect(float y, float xMin, float xMax, const vector3df &p1, const vector3df &p2, float &interpZ)
	{
		if ((p1.Y < y) != (p2.Y < y))
		{
			float alpha = (y - p1.Y) / (p2.Y - p1.Y);
			float iX = p1.X + alpha * (p2.X - p1.X);

			if ((iX >= xMin) && (iX <= xMax))
			{
				interpZ = p1.Z + alpha * (p2.Z - p1.Z);
				return true;
			}
		}

		return false;
	}

	// matrix4::buildProjectionMatrixOrthoLH seems wrong for OpenGL
	// See http://www.songho.ca/opengl/gl_projectionmatrix.html
	static inline void myMakeOrthoProj(matrix4& mat, float width, float height, float zNear, float zFar)
	{
		mat[0] = (2/width);
		mat[1] = 0;
		mat[2] = 0;
		mat[3] = 0;

		mat[4] = 0;
		mat[5] = (2/height);
		mat[6] = 0;
		mat[7] = 0;

		mat[8] = 0;
		mat[9] = 0;
		mat[10] = (2/(zFar-zNear));
		mat[11] = 0;

		mat[12] = 0;
		mat[13] = 0;
		mat[14] = (-(zFar+zNear)/(zFar-zNear));
		mat[15] = 1;
	}

	ShadowRenderCallback m_RenderCallback;
	
	std::vector<float> m_CascadeSizes;
	std::vector<float> m_CascadeSizesFull; // Expanded to render out onto margins (see m_CascadeRegions[Inner])
	// First cascade will be updated every two frames, 2nd cascade every four frames, 3rd every eight frames etc.
	unsigned short m_CascadeUpdateCount;

	ITexture *m_ShadowTexture; // The final shadow output texture
	std::vector<ITexture*> m_CascadeTextures; // All the textures that we need
	std::vector<recti> m_CascadeRegions;
	std::vector<matrix4> m_ViewProjMatrices;
	std::vector<matrix4> m_ShadowViewProjMatrices;

	// For shaders to sample from the shadow map
	std::vector<vector2df> m_TexOffsets, m_TexScales;
};

#endif