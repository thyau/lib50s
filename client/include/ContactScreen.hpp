#ifndef __CONTACTSCREEN_HPP__
#define __CONTACTSCREEN_HPP__

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Jon Schiavo
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * The initial screen the players sees when starting the application.
 * This is an implementation of the IGame interface, even though no
 * actual "play" occurs in here
 */

#include "IAttributes.h"
#include "IGUIEnvironment.h"
#include "ILightSceneNode.h"
#include "ISceneUserDataSerializer.h"
#include "SColor.h"

#include "ScriptManager.hpp"
#include "LuaFunctionRegister.hpp"
#include "IRenderPipeline.hpp"

#include "..\\include\\BaseGame.hpp"
#include "..\\include\\PIDControl.hpp"
#include "..\\include\\DebugScene.hpp"

/******************************************************
* Class  : ContactScreen
* Purpose: The main menu loop
* Notes  :
******************************************************/
class ContactScreen : public BaseGame, public irr::scene::ISceneUserDataSerializer
{
public:
							ContactScreen			(const GAME_INITIALIZER &init);
	virtual					~ContactScreen			();
	
	// Public Methods
	RESULT					init					() override;
	void					frame					(float delta) override;
	void					debugFrame				(float delta) override;
	void					updateGUI				(float delta) override;

	RESULT					save					() override;
	RESULT					load					() override;

	// Implemented methods from ISceneUserDataSerializer
	virtual void					OnCreateNode	(irr::scene::ISceneNode* node);
	virtual void					OnReadUserData	(irr::scene::ISceneNode* forSceneNode, irr::io::IAttributes* userData);
	virtual irr::io::IAttributes*	createUserData	(irr::scene::ISceneNode* forSceneNode);

protected:
	// Protected Members

	// Various test functions
	void createDesertBackdrop(void);
	void createPlaneBackdrop(void);

	void createOriginMarker(void);
	void createSunlight(void);
	
	void createSerializationTestScene(void);

	void createCompoundShapeTestScene1(void);
	Entity::Ptr createTetrisShape(int which);
	Entity::Ptr createTetrisBlock(float blkSz, int len);

	void createCompoundShapeTestScene2(void);
	// Just a test function!
	//void setAllNodeMaterialFlags(ISceneNode *node, E_MATERIAL_FLAG flag, bool newValue);
	void createCompoundShapeTestScene3(void);

	void createCompoundShapeTestScene4(void);

	void createCompoundShapeTestScene5(void);

	void createShaderTestScene1(void);
	void createShaderTestScene2(void);
	void createShaderTestScene3(void);
	void createShaderTestScene4(void);
	void createShaderTestScene5(void); // a sphere and two cubes
	void createShaderTestScene6(void);
	void createShaderTestScene7(void); // rows and rows of cubes

	void createHDRTestScene(void);

	void createZombieRuinsScene(void);
	void createScifiRoomScene(void);

	void createCharacterCollisionModel(void);



	// Misc functions
	void printSubtreeTypes(irr::scene::ISceneNode *node, int level = 0);


	Scene::Ptr				m_Scene;
	DebugScene::Ptr			m_DebugScene;
	//ScriptManager::Ptr		m_ScriptManager;
	//RegisterWrapper::Ptr    m_RegisterWrapper;
	IRenderPipeline::Ptr	m_RenderPipeline;

	// Test variables
	long					m_CharacterSphereId, m_CharacterViewId, m_CharacterBodyId;
	PIDControl				m_SpherePIDControl;
	Entity::Ptr				m_TestEntity1, m_TestEntity2;
	std::vector<Entity::Ptr> m_TestWheels;

	int m_CurrentMillis;
	int m_PeriodMillis;
	float m_MinBrightness;
	float m_MaxBrightness;
	float m_AmbientFactor;
	irr::video::SColorf m_FlashingLightColor;
	irr::scene::ILightSceneNode *m_FlashingLight;
};

#endif