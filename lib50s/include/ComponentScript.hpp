#ifndef _SCRIPTCOMPONENT_HPP_
#define _SCRIPTCOMPONENT_HPP_

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Zackery Perryman
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Component based container for Lua scripts that allows for scripts to interact with the parent entity.
 */

#include <memory>

#include "..\\include\\Common.hpp"
#include "..\\include\\BaseComponent.hpp"
#include "..\\include\\EntitySignal.hpp"
#include "..\\include\\IComponent.hpp"
#include "..\\include\\ComponentSpawnData.hpp"

//#include <lua.hpp>

typedef struct COMPONENT_SCRIPT_SPAWN_DATA
{
	//lua_State* state;

	//COMPONENT_SCRIPT_SPAWN_DATA(lua_State* state) { this->state = state; };

} COMPONENT_SCRIPT_SPAWN_DATA;

class ComponentScript : public BaseComponent , public std::enable_shared_from_this<ComponentScript>
{
public:
	typedef std::shared_ptr<ComponentScript> Ptr;
	typedef std::weak_ptr<ComponentScript>  wPtr;

	//Public Methods
								ComponentScript		();
	virtual				       ~ComponentScript		();

	virtual inline bool			allowMultiple		(){ return true; }

	virtual void				update				(float delta) override;

	//virtual void				sig_Moved			(const irr::core::vector3df& pos) override;
	//virtual void				sig_Rotated			(const irr::core::vector3df& rot) override;
	virtual void				sig_SceneChange		(const std::shared_ptr<Scene> &oldScene, const std::shared_ptr<Scene> newScene);

	inline COMPONENT_TYPE		getType				() const override { return COMPONENT_SCRIPT; }

	//inline lua_State*			getLuaState         () { return this->m_State; }
	//void						initializeLuaSide	();
	//void						setLuaState			(lua_State* state);

	inline ComponentScript::Ptr getSharedPointer() { return shared_from_this(); }

	class SpawnData : public ComponentSpawnData
	{
	public:
		SpawnData() { }

		virtual COMPONENT_TYPE getSpawnType() const override { return COMPONENT_SCRIPT; }

		virtual void serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options = 0) const;
		virtual void deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options = 0);

		// TODO attributes to load lua script
	};

protected:
	//Protected Methods
	virtual void				onAttach			(const std::shared_ptr<Entity> &parent) override;
	virtual void				onDetach			(const std::shared_ptr<Entity> &parent) override;

	void						moveScene			(const std::shared_ptr<Scene> &oldScene, const std::shared_ptr<Scene> newScene);
	
	
	////Protected Members
	//lua_State*					m_State;
};

#endif