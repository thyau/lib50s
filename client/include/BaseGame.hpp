#ifndef __BASEGAME_HPP__
#define __BASEGAME_HPP__

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Jon Schiavo
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Serves as a basic implementation of the IGame interface, though still abstract.
 * Contains data members and implements methods common to different game types,
 * to limit code duplication
 */

#include "IGUIEnvironment.h"

#include "..\\include\\IGame.hpp"

/******************************************************
* Class  : BaseGame (ABSTRACT)
* Purpose: Basic functionality common across game types
* Notes  :
******************************************************/
class BaseGame : public IGame
{
public:
								BaseGame				(const GAME_INITIALIZER &init);
	virtual						~BaseGame				();

protected:
	// Protected Members
	irr::IrrlichtDevice*		m_Device;
	irr::video::IVideoDriver*	m_VideoDriver;
	irr::scene::ISceneManager   *m_SceneManager, *m_DebugSceneManager;
	irr::gui::IGUIEnvironment*	m_GUIEnvironment;
	irr::io::IFileSystem*		m_FileSystem;

	EntityFactory*				m_EntFactory;
	ComponentFactory*			m_CompFactory;
};

#endif