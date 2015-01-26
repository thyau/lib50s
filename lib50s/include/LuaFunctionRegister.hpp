#ifndef _LUAFUNCTIONREGISTER_HPP_
#define _LUAFUNCTIONREGISTER_HPP_
/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Zackery Perryman
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Present C functions to lua to allow Lua to interact with Entities and Components
 */

#include <lua.hpp>

#include "..\\include\\common.hpp"
#include "..\\include\\EntityFactory.hpp"
#include "..\\include\\Scene.hpp"

class RegisterWrapper
{
public:
	typedef std::shared_ptr<RegisterWrapper> Ptr;
	typedef std::weak_ptr<RegisterWrapper> wPtr;

	RegisterWrapper(lua_State *L, EntityFactory *entityFactory, Scene *scene);
	void RegisterFunctions();
	static EntityFactory* getEntityFactory() { return m_EntityFactory; }
	static Scene*		  getScene() { return m_Scene; }
protected:
	lua_State *m_Lua;
	static EntityFactory *m_EntityFactory;
	static Scene		 *m_Scene;
};

namespace LuaBindings
{

int _LUAC_Vector3_Create(lua_State *L);
int _LUAC_Vector3_GetX(lua_State *L);
int _LUAC_Vector3_GetY(lua_State *L);
int _LUAC_Vector3_GetZ(lua_State *L);
int _LUAC_Vector3_Set(lua_State *L);
int _LUAC_Vector3_Print(lua_State *L);
int _LUAC_Vector3_Delete(lua_State *L);

int _LUAC_Entity_Create(lua_State *L);
int _LUAC_Entity_SetPosition(lua_State *L);
int _LUAC_Entity_GetPosition(lua_State *L);
int _LUAC_Entity_SetRotation(lua_State *L);
int _LUAC_Entity_GetRotation(lua_State *L);

int _LUAC_Include(lua_State *L);
}
#endif