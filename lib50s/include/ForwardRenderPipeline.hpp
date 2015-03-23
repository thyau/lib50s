#ifndef __FORWARDRENDERPIPELINE_HPP__
#define __FORWARDRENDERPIPELINE_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * The default forward-rendering pipeline.
 */

#include "IVideoDriver.h"

#include "baserenderpipeline.hpp"

class ForwardRenderPipeline : public BaseRenderPipeline
{
public:
	ForwardRenderPipeline(irr::video::IVideoDriver *videoDriver);
	virtual ~ForwardRenderPipeline(void);

	virtual RESULT	init	() override;
	virtual void	run		() override;
};

#endif