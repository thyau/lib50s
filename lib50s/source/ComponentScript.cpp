/*
	ComponentScript.cpp
	Copyright (c) [ORGNAME] 2013
	Author: Zackery Perryman
*/
#include "..\\include\\ComponentScript.hpp"
#include "..\\include\\BaseComponent.hpp"
#include "..\\include\\Entity.hpp"

using namespace irr::core;
using namespace irr::io;

ComponentScript::ComponentScript()
{

}

ComponentScript::~ComponentScript()
{

}

//void ComponentScript::initializeLuaSide()
//{
//	//lua_getglobal(this->m_State,"_L_RegisterComponent");
//	//lua_pushlightuserdata(this->m_State,this);
//	//if(! lua_pcall(this->m_State,1,0,NULL) == LUA_OK)
//	//{
//	//	std::cout << "Error attempting to initialize a component's Lua state" << std::endl;
//	//	std::cout << lua_tostring(this->m_State,1) << std::endl;
//	//}
//	
//}

void ComponentScript::update(float delta)
{
	//lua_getglobal(this->m_State,"Components");
	//lua_rawgetp(this->m_State,-1,this);
	//lua_getfield(this->m_State,-1,"onUpdate");
	//lua_pushvalue(this->m_State,-2);
	//lua_pushnumber(this->m_State,delta);
	//
	//lua_pcall(this->m_State,2,0,NULL);
}

//void ComponentScript::sig_Moved(const vector3df &pos)
//{
//
//}
//
//void ComponentScript::sig_Rotated(const vector3df &rot)
//{
//
//}

void ComponentScript::sig_SceneChange(const std::shared_ptr<Scene> &oldScene, const std::shared_ptr<Scene> newScene)
{

}

//void ComponentScript::setLuaState(lua_State *L)
//{
//	this->m_State = L;
//}

void ComponentScript::onAttach(const std::shared_ptr<Entity> &parent)
{
	//BaseComponent::onAttach(parent);

	////m_Connections[ENT_SIGNAL_UPDATE] = m_Parent->subscribeUpdate(WO_CALL_FIRST, boost::bind(&ComponentScript::sig_Update, this, _1));

	//lua_getglobal(this->m_State,"Components");
	//lua_rawgetp(this->m_State,-1,this);
	//lua_getfield(this->m_State,-1,"onAttach");
	//lua_pushvalue(this->m_State,-2);
	//lua_pushlightuserdata(this->m_State,parent.get());

	//lua_pcall(this->m_State,2,0,NULL);
}

void ComponentScript::onDetach(const std::shared_ptr<Entity> &parent)
{
	BaseComponent::onDetach(parent);

}

//void ComponentScript::SpawnData::serializeAttributes(IAttributes* out, SAttributeReadWriteOptions* options) const
//{
//	//TODO implement
//}
//	
//void ComponentScript::SpawnData::deserializeAttributes(IAttributes* in, SAttributeReadWriteOptions* options)
//{
//	//TODO implement
//}