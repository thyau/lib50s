#ifndef __COMPONENT_SPAWN_DATA_HPP__
#define __COMPONENT_SPAWN_DATA_HPP__

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Base class to represent the attributes needed to spawn entity components. This is the primary way
 * to save and load components.
 */

#include "..\\include\\IComponent.hpp"

class ComponentSpawnData
{
public:
	ComponentSpawnData(void);
	virtual ~ComponentSpawnData(void);

	virtual COMPONENT_TYPE getSpawnType(void) const PURE;

	// These serialization functions mirror the ones from ISceneNode
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const PURE;
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0) PURE;
};

#endif