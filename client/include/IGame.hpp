#ifndef __IGAME_HPP__
#define __IGAME_HPP__

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Jon Schiavo
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * This interface represents the current main process of the application.
 * It does not necessarily have to be a "game" per-se, but rather anything
 * that controls the main loop.  For example, the contact screen is also
 * a "game" because it has a scene and processes input.
 */

#include <Common.hpp>
#include <Scene.hpp>
#include <EntityFactory.hpp>
#include <ComponentFactory.hpp>

/*
 * STRUCTURE: GAME_INITIALIZER
 * Contains data needed to initialize a new game class.  Exists for the
 * sake of limiting the number of constructor arguments necessary.
 */
typedef struct GAME_INITIALIZER
{
	irr::IrrlichtDevice* device;
	irr::video::IVideoDriver* videoDriver;
	irr::scene::ISceneManager* sceneManager;
	// May be NULL. Rendered after sceneManager with the same camera settings (an active camera must be set)
	irr::scene::ISceneManager* debugSceneManager; 
	irr::gui::IGUIEnvironment* guiEnv;
	irr::io::IFileSystem* fileSystem;
} GAME_INITIALIZER;

/******************************************************
* Class  : IGame (ABSTRACT)
* Purpose: Interface for all application loops
* Notes  :
******************************************************/
class IGame
{
public:
	typedef std::shared_ptr<IGame> Ptr;
	typedef std::weak_ptr<IGame> wPtr;

	// Public Methods
	virtual RESULT				init					() PURE;

	// Called between a video driver beginScene() and endScene(), and responsible
	// for drawing the next frame
	virtual void				frame					(float delta) PURE;
	virtual void				debugFrame				(float delta) PURE;
	virtual void				updateGUI				(float delta) PURE;

	virtual RESULT				save					() PURE;
	virtual RESULT				load					() PURE;
};

#endif