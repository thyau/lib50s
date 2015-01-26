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

#include "baserenderpipeline.hpp"

class ForwardRenderPipeline : public BaseRenderPipeline
{
public:
	ForwardRenderPipeline(IVideoDriver *videoDriver);
	virtual ~ForwardRenderPipeline(void);

	virtual RESULT	init	() override;
	virtual void	run		() override;
};

#endif