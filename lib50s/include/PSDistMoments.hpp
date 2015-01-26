#ifndef __PSDISTMOMENTS_HPP__
#define __PSDISTMOMENTS_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Computes the eye-space distance and distance squared of each pixel to
 * the center of projection. Distances are rescaled to the range [0,1]
 * spanning the smallest and largest possible distances on the near and
 * far planes; the latter is not the same as the far plane depth.
 */

#include "..\\include\\BasePixelShader.hpp"
#include "..\\include\\ICameraContext.hpp"

class PSDistMoments : public BasePixelShader
{
public:
	PSDistMoments(ICameraContext &cameraContext) : m_CameraContext(cameraContext) {	}

	virtual std::string	buildSource() override;
	IShaderConstantSetCallBack* getDefaultCallbackObject() override
	{
		return new Callback(m_CameraContext);
	}

protected:
	class Callback : public IShaderConstantSetCallBack
	{
	public:
		Callback(ICameraContext &cameraContext) : m_CameraContext(cameraContext) {	}
		virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData) override;
	protected:
		ICameraContext &m_CameraContext;
	};

	ICameraContext &m_CameraContext;
};

#endif