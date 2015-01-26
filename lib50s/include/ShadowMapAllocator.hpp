#ifndef __SHADOWMAPALLOCATOR_HPP__
#define __SHADOWMAPALLOCATOR_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Allocates textures for shadow mapping, dynamically adjusting texture
 * sizes and sub-regions as needed.
 */

#include "..\\include\\Common.hpp"

enum SHADOW_MAP_ALLOC_STRATEGY {
	SHADOW_MAP_ALLOC_SINGLE,
	SHADOW_MAP_ALLOC_FLAT_CUBE, // Single (flat) texture divided up into 6 cube faces
	// Single texture divided into a user-specified number of square regions.
	// Textures allocated using this strategy are NOT shared, and must be deallocated
	// by the caller.
	SHADOW_MAP_ALLOC_CASCADE 
};

class ShadowMapAllocator
{
public:
	ShadowMapAllocator(IVideoDriver *videoDriver);
	virtual ~ShadowMapAllocator(void);

	// Note: the same textures may be reused when called repeatedly.
	// If allocSecondaryTexture is true, then outTextures contains twice the number of textures. 
	// The second set is in the same order as the first set.
	// Interpretation of outTextures and outRegions depends on allocation strategy (e.g. for strategies
	// that allocate a single texture, all regions are specified within that texture).
	// outRegions gives pixel coordinates of each region.
	// numRegions is the number of requested regions, see SHADOW_MAP_ALLOC_STRATEGY to see when this applies
	virtual void allocateTextures(const std::vector<SViewFrustum> &frustums, SHADOW_MAP_ALLOC_STRATEGY strategy,
		std::vector<ITexture*> &outTextures, std::vector<recti> &outRegions, bool allocSecondaryTexture = false,
		unsigned int numRegions = 0);

	// Convert pixel coordinates into normalized coordinates.
	// Regions are interpreted in terms of a single texture.
	static void calcNormalizedRegions(const ITexture* texture, const std::vector<recti> &regions, 
		std::vector<vector2df> &outTexOffsets, std::vector<vector2df> &outTexScales);

protected:

	virtual void findSuitableTextures(const std::vector<ITexture*> &texturePool, std::vector<ITexture*> &out,
		unsigned int numWanted, dimension2du minDim, dimension2du exactDim = dimension2du(0,0));

	IVideoDriver *m_VideoDriver;
	ECOLOR_FORMAT m_ShadowFormat;

	ITexture *m_FlatCubeTexture, *m_FlatCubeTexture2;
	ITexture *m_SingleViewTexture, *m_SingleViewTexture2;
	//std::vector<ITexture*> m_CascadeTextures;
};

#endif