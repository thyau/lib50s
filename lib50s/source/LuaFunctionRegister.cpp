///*
//	LuaFunctionRegister.cpp
//	Copyright (c) [ORGNAME] 2013
//	Author: Zackery Perryman
//*/
//#include "..\\include\\LuaFunctionRegister.hpp"
//
//
//
//RegisterWrapper::RegisterWrapper(lua_State *L, EntityFactory *entityFactory, Scene *scene)
//{
//	this->m_Lua = L;
//	this->m_EntityFactory = entityFactory;
//	this->m_Scene = scene;
//}
//
//EntityFactory* RegisterWrapper::m_EntityFactory = NULL;
//Scene*		   RegisterWrapper::m_Scene	= NULL;
//
////Mass registration of all Functions in LuaBindings
//void RegisterWrapper::RegisterFunctions()
//{
//	lua_register(this->m_Lua,"_C_Vector3",LuaBindings::_LUAC_Vector3_Create);
//	lua_register(this->m_Lua,"_C_Vector3_GetX",LuaBindings::_LUAC_Vector3_GetX);
//	lua_register(this->m_Lua,"_C_Vector3_GetY",LuaBindings::_LUAC_Vector3_GetY);
//	lua_register(this->m_Lua,"_C_Vector3_GetZ",LuaBindings::_LUAC_Vector3_GetZ);
//	lua_register(this->m_Lua,"_C_Vector3_Print",LuaBindings::_LUAC_Vector3_Print);
//	lua_register(this->m_Lua,"_C_Vector3_Delete",LuaBindings::_LUAC_Vector3_Delete);
//
//	lua_register(this->m_Lua,"_C_Entity_Create",LuaBindings::_LUAC_Entity_Create);
//	lua_register(this->m_Lua,"_C_Entity_GetPosition",LuaBindings::_LUAC_Entity_GetPosition);
//	lua_register(this->m_Lua,"_C_Entity_SetPosition",LuaBindings::_LUAC_Entity_SetPosition);
//	lua_register(this->m_Lua,"_C_Entity_GetRotation",LuaBindings::_LUAC_Entity_GetRotation);
//	lua_register(this->m_Lua,"_C_Entity_SetRotation",LuaBindings::_LUAC_Entity_SetRotation);
//
//	lua_register(this->m_Lua,"include",LuaBindings::_LUAC_Include);
//}
////Lua binding to create a vector object.
//int LuaBindings::_LUAC_Vector3_Create(lua_State *L)
//{
//	luaL_checktype(L,1,LUA_TNUMBER);
//	double z = irr::f32(lua_tonumber(L,1));
//	luaL_checktype(L,2,LUA_TNUMBER);
//	double y = irr::f32(lua_tonumber(L,2));
//	luaL_checktype(L,3,LUA_TNUMBER);
//	double x = irr::f32(lua_tonumber(L,3));
//
//	vector3df *vec = new vector3df(x,y,z);
//	lua_pushlightuserdata(L,vec);
//
//	return 1;
//}
////Push Vector's X value to Lua
//int LuaBindings::_LUAC_Vector3_GetX(lua_State *L)
//{
//	luaL_checktype(L,1,LUA_TLIGHTUSERDATA);
//	vector3df *vec = (vector3df*)(lua_touserdata(L,1));
//	lua_pushnumber(L,vec->X);
//	return 1;
//}
////Push Vector's Y value to Lua
//int LuaBindings::_LUAC_Vector3_GetY(lua_State *L)
//{
//	luaL_checktype(L,1,LUA_TLIGHTUSERDATA);
//	vector3df *vec = (vector3df*)(lua_touserdata(L,1));
//	lua_pushnumber(L,vec->Y);
//	return 1;
//}
////Push Vector's Z value to Lua
//int LuaBindings::_LUAC_Vector3_GetZ(lua_State *L)
//{
//	luaL_checktype(L,1,LUA_TLIGHTUSERDATA);
//	vector3df *vec = (vector3df*)(lua_touserdata(L,1));
//	lua_pushnumber(L,vec->Z);
//	return 1;
//}
////Print Vector's X,Y, and Z values to console via cout
//int LuaBindings::_LUAC_Vector3_Print(lua_State *L)
//{
//	luaL_checktype(L,1,LUA_TLIGHTUSERDATA);
//	vector3df *vec = (vector3df*)(lua_touserdata(L,1));
//	std::cout << "X;Y;Z" << std::endl;
//	std::cout << vec->X << ";" << vec->Y << ";" << vec->Z << std::endl;
//	return 0;
//}
////Cleanup Vector's memory in the heap
//int LuaBindings::_LUAC_Vector3_Delete(lua_State *L)
//{
//	luaL_checktype(L,1,LUA_TLIGHTUSERDATA);
//	vector3df *vec = (vector3df*)(lua_touserdata(L,1));
//	delete vec;
//	return 0;
//}
////Set Vector's values
//int LuaBindings::_LUAC_Vector3_Set(lua_State *L)
//{
//	luaL_checktype(L,1,LUA_TLIGHTUSERDATA);
//	vector3df *vec = (vector3df*)(lua_touserdata(L,1));
//	luaL_checktype(L,2,LUA_TNUMBER);
//	float X = lua_tonumber(L,2);
//	luaL_checktype(L,3,LUA_TNUMBER);
//	float Y = lua_tonumber(L,3);
//	luaL_checktype(L,4,LUA_TNUMBER);
//	float Z = lua_tonumber(L,4);
//	
//	vec->set(X,Y,Z);
//
//	return 0;
//}
//
//int LuaBindings::_LUAC_Entity_Create(lua_State *L)
//{
//	//ENTITY_SPAWN_DATA data("LUA_ENTITY",ZERO_VECTOR,ZERO_VECTOR,NULL,NULL);
//	//data.compScript = std::vector<COMPONENT_SCRIPT_SPAWN_DATA>();
//	//data.compScript.push_back(COMPONENT_SCRIPT_SPAWN_DATA(L));
//
//	//ComponentMesh::Ptr meshData = ComponentMesh::Ptr(new ComponentMesh());
//	
//
//	Entity::Ptr entity = RegisterWrapper::getEntityFactory()->makeEntity();
//	//entity->addComponent(meshData);
//	RegisterWrapper::getScene()->addEntity(entity);
//
//	lua_pushlightuserdata(L,entity.get());
//
//	return 1;
//}
//
//int LuaBindings::_LUAC_Entity_GetPosition(lua_State *L)
//{
//	luaL_checktype(L,1,LUA_TLIGHTUSERDATA);
//	Entity *ent = (Entity*)lua_touserdata(L,1);
//
//	irr::core::vector3df vec = ent->getPosition();
//
//	lua_pushnumber(L,vec.X);
//	lua_pushnumber(L,vec.Y);
//	lua_pushnumber(L,vec.Z);
//
//	return 3;
//}
//
//int LuaBindings::_LUAC_Entity_SetPosition(lua_State *L)
//{
//	luaL_checktype(L,1,LUA_TLIGHTUSERDATA);
//	Entity *entity = (Entity*)lua_touserdata(L,1);
//	luaL_checktype(L,2,LUA_TNUMBER);
//	float X = lua_tonumber(L,2);
//	luaL_checktype(L,3,LUA_TNUMBER);
//	float Y = lua_tonumber(L,3);
//	luaL_checktype(L,4,LUA_TNUMBER);
//	float Z = lua_tonumber(L,4);
//
//	entity->setPosition(vector3df(X,Y,Z));
//
//	return 0;
//}
//
//int LuaBindings::_LUAC_Entity_GetRotation(lua_State *L)
//{
//	luaL_checktype(L,1,LUA_TLIGHTUSERDATA);
//	Entity *ent = (Entity*)lua_touserdata(L,1);
//
//	irr::core::vector3df vec = ent->getRotation();
//
//	lua_pushnumber(L,vec.X);
//	lua_pushnumber(L,vec.Y);
//	lua_pushnumber(L,vec.Z);
//
//	return 3;
//}
//
//int LuaBindings::_LUAC_Entity_SetRotation(lua_State *L)
//{
//	luaL_checktype(L,1,LUA_TLIGHTUSERDATA);
//	Entity *entity = (Entity*)lua_touserdata(L,1);
//	luaL_checktype(L,2,LUA_TNUMBER);
//	float X = lua_tonumber(L,2);
//	luaL_checktype(L,3,LUA_TNUMBER);
//	float Y = lua_tonumber(L,3);
//	luaL_checktype(L,4,LUA_TNUMBER);
//	float Z = lua_tonumber(L,4);
//
//	entity->setRotation(vector3df(X,Y,Z));
//
//	return 0;
//}
//
//int LuaBindings::_LUAC_Include(lua_State *L)
//{
//	const char* filePath = lua_tostring(L,1);
//	luaL_dofile(L,filePath);
//
//	return 0;
//}