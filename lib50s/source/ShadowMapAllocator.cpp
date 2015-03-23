#include <assert.h>
#include <string>

#include "irrTypes.h"

#include "..\include\ShadowMapAllocator.hpp"

using namespace std;

using namespace irr;
using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;

ShadowMapAllocator::ShadowMapAllocator(IVideoDriver *videoDriver)
	: m_VideoDriver(videoDriver), m_ShadowFormat(ECF_G32R32F /*ECF_G16R16F*/)
{
	m_VideoDriver->grab();

	// TODO: this implementation is a stub.
	dimension2du shadowSize(1024, 512);
	m_FlatCubeTexture = m_VideoDriver->addRenderTargetTexture(shadowSize, "flat_cube_shadow_map", m_ShadowFormat);
	m_FlatCubeTexture2 = m_VideoDriver->addRenderTargetTexture(shadowSize, "flat_cube_shadow_map2", m_ShadowFormat);

	shadowSize = dimension2du(512, 512);
	m_SingleViewTexture = m_VideoDriver->addRenderTargetTexture(shadowSize, "single_view_shadow_map", m_ShadowFormat);
	m_SingleViewTexture2 = m_VideoDriver->addRenderTargetTexture(shadowSize, "single_view_shadow_map2", m_ShadowFormat);
}


ShadowMapAllocator::~ShadowMapAllocator(void)
{
	m_VideoDriver->drop();
}

void ShadowMapAllocator::allocateTextures(const std::vector<SViewFrustum> &frustums, SHADOW_MAP_ALLOC_STRATEGY strategy,
		std::vector<ITexture*> &outTextures, std::vector<recti> &outRegions, bool allocateSecondaryTexture, unsigned int numRegions)
{
	outTextures.clear();
	outRegions.clear();

	static unsigned int cascadeTexAllocated = 0;

	switch(strategy)
	{
	case SHADOW_MAP_ALLOC_SINGLE:
		{
		//outTextures.push_back(m_FlatCubeTexture);
		//if (allocateSecondaryTexture)
		//	outTextures.push_back(m_FlatCubeTexture2);

		//// Re-use the flat cube texture, but keep a square aspect ratio
		//u32 regionSize = m_FlatCubeTexture->getSize().Height;
		//outRegions.push_back(recti(0, 0, regionSize, regionSize));


			// Workaround: For some reason setting the viewport doesn't work in SpotShadowMapRenderer, so use a separate square texture
			outTextures.push_back(m_SingleViewTexture);
			if (allocateSecondaryTexture)
				outTextures.push_back(m_SingleViewTexture2);

			u32 regionSize = m_SingleViewTexture->getSize().Height;
			outRegions.push_back(recti(0, 0, regionSize, regionSize));

			break;
		}
	case SHADOW_MAP_ALLOC_FLAT_CUBE:
		{
			// TODO: this implementation is a stub. Be more cleverer and find a suitable resolution depending on view frustums and distance from camera.
			outTextures.push_back(m_FlatCubeTexture);
			if (allocateSecondaryTexture)
				outTextures.push_back(m_FlatCubeTexture2);

			// Sub-divide into regions like this:
			// -----------------
			// | 1 | 3 | 5 | x |
			// -----------------
			// | 2 | 4 | 6 | x |
			// -----------------
			// It's allocated in a 2:1 aspect ratio to keep power-of-two dimensions.
			u32 regionSize = m_FlatCubeTexture->getSize().Height / 2;
			outRegions.push_back(recti(0, 0, regionSize, regionSize));
			outRegions.push_back(recti(0, regionSize, regionSize, 2*regionSize));
			outRegions.push_back(recti(regionSize, 0, 2*regionSize, regionSize));
			outRegions.push_back(recti(regionSize, regionSize, 2*regionSize, 2*regionSize));
			outRegions.push_back(recti(2*regionSize, 0, 3*regionSize, regionSize));
			outRegions.push_back(recti(2*regionSize, regionSize, 3*regionSize, 2*regionSize));
			break;
		}
	case SHADOW_MAP_ALLOC_CASCADE:
		{
			assert(numRegions > 0 && "Invalid number of cascaded shadow maps requested.");

			// We will allocate memory in one of these aspect ratios, finding one that will
			// fit the requested number of regions with minimum wastage.
			dimension2du aspectRatios[] = { dimension2du(1,1), dimension2du(2,1) };
			dimension2du allocSize = dimension2du(0,0);

			for (unsigned int i = 0; i < sizeof(aspectRatios)/sizeof(aspectRatios[0]); i++)
			{
				const dimension2du &aspect = aspectRatios[i];

				unsigned int j = 1;
				while ((j * aspect.Width * j * aspect.Height) < numRegions)	{ j++; }

				if ((allocSize.Width == 0) || 
					(allocSize.Width * allocSize.Height) > (j * aspect.Width * j * aspect.Height))
				{
					allocSize.Width = j * aspect.Width;
					allocSize.Height = j * aspect.Height;
				}
			}

			// Decide on the pixel dimensions of each region
			// These numbers should ideally be powers of 2
			unsigned int regionMinSize = 512;
			unsigned int texturePrefSize = 2048; 

			// Try to scale up region size so that both texture dimensions are close to texturePrefSize
			unsigned int regionSize = regionMinSize;
			while ((2 * regionSize * allocSize.Width <= texturePrefSize) &&
				   (2 * regionSize * allocSize.Height <= texturePrefSize))
			{
				regionSize *= 2;
			}

			dimension2du textureSize = dimension2du(regionSize * allocSize.Width, regionSize * allocSize.Height);

			// Try to find an existing texture that fits our requirements
			unsigned int numTex = allocateSecondaryTexture? 2 : 1;
			// Specify exact texture size to avoid potential mistakes in caller code
			// TODO: possibly save memory in the future by only specifying minimum size
			// Took this out so that cascade textures are not shared, and must be deallocated by the caller
			//findSuitableTextures(m_CascadeTextures, outTextures, numTex, textureSize, textureSize);

			while (outTextures.size() < numTex)
			{
				ITexture *tex = m_VideoDriver->addRenderTargetTexture(textureSize, 
					("cascade_shadow_map_" + to_string(cascadeTexAllocated++)).c_str(), m_ShadowFormat);
				outTextures.push_back(tex);
			}

			// Assign regions
			for (unsigned int i = 0; i < allocSize.Width; i++)
			{
				for (unsigned int j = 0; j < allocSize.Height; j++)
				{
					outRegions.push_back(recti(i * regionSize, j * regionSize, (i+1) * regionSize, (j+1) * regionSize));
					if (outRegions.size() >= numRegions) { break; }
				}
			}

			break;
		}
	default:
		assert(false && "Unknown shadow map allocation strategy.");
	}
}

void ShadowMapAllocator::calcNormalizedRegions(const ITexture* texture, const std::vector<recti> &regions, 
	std::vector<vector2df> &outTexOffsets, std::vector<vector2df> &outTexScales)
{
	outTexOffsets.clear();
	outTexScales.clear();

	dimension2du texSize = texture->getSize();

	for (unsigned int i = 0; i < regions.size(); i++)
	{
		recti region = regions[i];

		outTexScales.push_back(vector2df((float)region.getWidth() / (float)texSize.Width, 
			(float)region.getHeight() / (float)texSize.Height));
		// regions is measured in pixels, with origin in upper left corner
		// normalized offsets is measured with origin in bottom left corner
		outTexOffsets.push_back(vector2df((float)region.UpperLeftCorner.X / (float)texSize.Width, 
			(float)(texSize.Height - region.UpperLeftCorner.Y - region.getHeight()) / (float)texSize.Height));
	}
}

void ShadowMapAllocator::findSuitableTextures(const std::vector<ITexture*> &texturePool, std::vector<ITexture*> &out,
											  unsigned int numWanted, dimension2du minDim, dimension2du exactDim)
{
	unsigned int numAdded = 0;

	for (unsigned int i = 0; i < texturePool.size(); i++)
	{
		ITexture* tex = texturePool[i];

		if (tex->getSize().Width >= minDim.Width && tex->getSize().Height >= minDim.Height)
		{
			if (exactDim.Width == 0 || 
				((exactDim.Width == tex->getSize().Width) && (exactDim.Height == tex->getSize().Height)))
			{
				out.push_back(tex);
				if (numAdded >= numWanted) { break; }
			}
		}
	}
}