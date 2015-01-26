#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Jon Schiavo
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Contains common includes and macro definitions used throughout other source files.
 */

#include <Common.hpp>
#include <Scene.hpp>
#include <EntityFactory.hpp>

#include "..\\include\\IrrlichtEventPipe.hpp"
#include "..\\include\\ContactScreen.hpp"

/******************************************************
* Class  : Client
* Purpose: Handles all application startup, execution, and shutdown
* Notes  : 
******************************************************/
class Client
{
public:
	// Constructor and Destructor
						Client					();
						~Client					();

	// Public Methods
	void				setArgs					(long argc, char **argv);
	bool				init					();
	long				go						();
	bool				shutdown				();

private:
	// Private Methods
	RESULT				launchContactScreen		();
	RESULT				launchOfflineGame		();
	RESULT				endCurrentGame			();
	
	// Private Members
	bool				m_Started;

	irr::IrrlichtDevice*		m_Device;
	irr::video::IVideoDriver*	m_VideoDriver;
	irr::scene::ISceneManager	*m_SceneManager, *m_DebugSceneManager;
	irr::gui::IGUIEnvironment*	m_GUIEnvironment;
	irr::io::IFileSystem*		m_FileSystem;
	irr::ITimer*				m_Timer;

	IrrlichtEventPipe*			m_EventPipe;

	irr::u32					m_LastTick;
	unsigned int				m_MaxFrameRate;
	
	IGame::Ptr					m_CurrentGame;
};

#endif