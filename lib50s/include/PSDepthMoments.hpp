#ifndef __PSDEPTHMOMENTS_HPP__
#define __PSDEPTHMOMENTS_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Renders depth and squared depth. Depth is measured along the camera 
 * viewing direction, and ranges from 0 on the near plane to 1 on the 
 * far plane. 
 * Note that a bias value should be added back to the depth before use
 * (see shader::getFPBias).
 */

#include "..\\include\\BasePixelShader.hpp"

class PSDepthMoments : public BasePixelShader
{
public:
	virtual std::string	buildSource() override;
};

#endif