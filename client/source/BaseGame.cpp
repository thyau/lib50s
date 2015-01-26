/*
	BaseGame.cpp
	Copyright (c) [ORGNAME] 2013
	Author: Jon Schiavo
*/

#include "..\\include\\BaseGame.hpp"

BaseGame::BaseGame(const GAME_INITIALIZER &init)
{
	// Assign pointers and increment reference count
	m_Device = init.device;
	m_Device->grab();
	m_VideoDriver = init.videoDriver;
	m_VideoDriver->grab();
	m_SceneManager = init.sceneManager;
	m_SceneManager->grab();
	m_DebugSceneManager = init.debugSceneManager;
	if (m_DebugSceneManager)
		m_DebugSceneManager->grab();
	m_GUIEnvironment = init.guiEnv;
	m_GUIEnvironment->grab();
	m_FileSystem = init.fileSystem;
	m_FileSystem->grab();

	m_EntFactory = new EntityFactory(m_SceneManager);
	m_CompFactory = new ComponentFactory(m_SceneManager);
}

BaseGame::~BaseGame()
{
	// Drop pointers
	SAFE_DROP(m_Device);
	SAFE_DROP(m_VideoDriver);
	SAFE_DROP(m_SceneManager);
	SAFE_DROP(m_GUIEnvironment);
	SAFE_DROP(m_FileSystem);

	SAFE_DELETE(m_EntFactory);
}