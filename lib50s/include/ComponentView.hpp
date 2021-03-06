#ifndef __COMPONENTVIEW_HPP__
#define __COMPONENTVIEW_HPP__

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Jon Schiavo
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Gives an object and camera scene node
 */

#include "..\\include\\ComponentWithSceneNode.hpp"
#include "..\\include\\ComponentSpawnData.hpp"

/*
 * STRUCTURE: COMPONENT_VIEW_SPAWN_DATA
 * For use in ENTITY_SPAWN_DATA.  Holds initial information for creating a camera component
 */
//typedef struct COMPONENT_VIEW_SPAWN_DATA
//{
//	vector3df posOffset;
//	vector3df lookAt;
//	bool active;
//
//	COMPONENT_VIEW_SPAWN_DATA() :
//		posOffset(ZERO_VECTOR),
//		lookAt(-UNIT_Z),
//		active(false)
//	{ }
//
//	COMPONENT_VIEW_SPAWN_DATA(const vector3df &pos, const vector3df &look, bool activate) :
//		posOffset(pos),
//		lookAt(look),
//		active(activate)
//	{ }
//
//} COMPONENT_VIEW_SPAWN_DATA;

/******************************************************
* Class  : ComponentView
* Purpose: Wrapper for an object holding a camera
* Notes  :
******************************************************/
class ComponentView : public ComponentWithSceneNode
{
public:
							ComponentView			();
	virtual					~ComponentView			();

	// Public Methods
	inline COMPONENT_TYPE	getType					() const override { return COMPONENT_VIEW; }

	// Only allow camera nodes
	ISceneNode*				setNode					(ICameraSceneNode* cam) { return this->setNodeImpl(cam); }

	void					makeActive				(ISceneManager* smgr);

	class SpawnData : public ComponentSpawnData
	{
	public:
		vector3df posOffset;
		vector3df lookAt;
		bool active;

		SpawnData() : posOffset(ZERO_VECTOR), lookAt(ZERO_VECTOR), active(false) { }
		SpawnData(const vector3df &pos, const vector3df &look, bool activate)
			: posOffset(pos), lookAt(look), active(activate) { }

		virtual COMPONENT_TYPE getSpawnType() const override { return COMPONENT_VIEW; }

		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);
	};

protected:
	// Protected Methods
	virtual void			onAttach				(const std::shared_ptr<Entity> &parent) override;
	virtual void			onDetach				(const std::shared_ptr<Entity> &parent) override;
};

#endif