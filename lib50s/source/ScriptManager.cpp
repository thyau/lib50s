/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Zackery Perryman
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Utility class for creating a lua_State and loading Lua files.
 */

#include "..//include//ScriptManager.hpp"
#include <iostream>

ScriptManager::ScriptManager()
{
	this->m_Lua = luaL_newstate();
	this->m_FilePaths = std::list<boost::filesystem::path>();

	luaL_openlibs(this->m_Lua);
}

void ScriptManager::loadScripts()
{
	for(boost::filesystem::recursive_directory_iterator end, itr = boost::filesystem::recursive_directory_iterator("data/scripts");
		itr != end;
		itr++)
	{
		if(boost::filesystem::is_regular_file((*itr)) && (*itr).path().extension().string() == ".lua")
		{
		this->m_FilePaths.push_back((*itr));
		}
	}
}

void ScriptManager::executeScripts()
{
	for(std::list<boost::filesystem::path>::iterator itr = this->m_FilePaths.begin();
		itr != this->m_FilePaths.end();
		itr++)
	{
		luaL_dofile(this->m_Lua,(*itr).string().c_str());
	}
}

void ScriptManager::executeScript(std::string filePath)
{
	std::cout << "Executing file: " << filePath << std::endl;
	luaL_dofile(this->m_Lua,filePath.c_str());
}