/*
	ContactScreen.cpp
	Copyright (c) [ORGNAME] 2013
	Author: Jon Schiavo
*/

#include <iostream>
#include <iomanip>

#include "ISceneNodeAnimatorCameraFPS.h"
#include "ITerrainSceneNode.h"
#include "CDynamicMeshBuffer.h"
#include "SMesh.h"

#include "BulletCompat.hpp"
#include "ForwardRenderPipeline.hpp"
#include "DeferredRenderPipeline.hpp"
#include "WindowsConsole.hpp"

#include "..\\include\\ContactScreen.hpp"

using namespace std;

using namespace irr;
using namespace irr::core;
using namespace irr::gui;
using namespace irr::scene;
using namespace irr::video;

ContactScreen::ContactScreen(const GAME_INITIALIZER &init) :
	BaseGame(init), m_SpherePIDControl(100.0f, 0.0f, 0.0f, (float)PI*2, 10.0f, -10.0f), m_FlashingLight(NULL)
{
}

ContactScreen::~ContactScreen()
{
}

RESULT ContactScreen::init()
{
	m_SceneManager->registerSceneNodeFactory(m_EntFactory);
	m_Scene = Scene::Ptr(new Scene());
	m_DebugScene = DebugScene::Ptr(new DebugScene(m_SceneManager, m_DebugSceneManager));

	//m_RenderPipeline = IRenderPipeline::Ptr(new ForwardRenderPipeline(m_VideoDriver));
	m_RenderPipeline = IRenderPipeline::Ptr(new DeferredRenderPipeline(m_VideoDriver));
	if (m_RenderPipeline->init() != R_OK)
	{
		PRINT("Contact screen failed to initialize rendering pipeline\n", RED);
		return R_E_FAIL;
	}
	m_RenderPipeline->setSceneManager(m_SceneManager);

	m_Device->getCursorControl()->setVisible(false);

	/***** Serialization test *****/

	//std::string filename = "serialization_test.irr";

	//createSerializationTestScene();

	//m_SceneManager->saveScene(filename.c_str(), this);

	////m_SceneManager->clear();
	////m_Scene->removeAllEntities();

	////m_SceneManager->loadScene(filename.c_str(), this);

	//// TODO not sure why the FPS camera is not receiving inputs after loading
	////PRINT("Active camera is " << m_SceneManager->getActiveCamera()->getName() << endl, WHITE);
	////m_SceneManager->setActiveCamera(m_SceneManager->getActiveCamera());


	/***** Physics compound shapes test *****/

	//createCompoundShapeTestScene1();
	//createCompoundShapeTestScene2();
	//createCompoundShapeTestScene3();
	//createCompoundShapeTestScene4();
	//createCompoundShapeTestScene5();


	/***** Shaders tests *****/

	//createShaderTestScene1();
	//createShaderTestScene2();
	//createShaderTestScene3();
	//createShaderTestScene4();
	//createShaderTestScene5();
	//createShaderTestScene6();
	//createShaderTestScene7();

	//createHDRTestScene();

	/***** Demo scenes *****/
	createZombieRuinsScene();
	//createScifiRoomScene();


	PRINT("Contact screen init done!\n", WHITE);

	return R_OK;
}

void ContactScreen::frame(float delta)
{
	//// Test stuff
	//Entity::Ptr sphere = m_Scene->getEntity(m_CharacterSphereId);
	//btRigidBody *body = (dynamic_pointer_cast<ComponentPhysical>(sphere->getComponent(COMPONENT_PHYSICAL)))->getBody();
	//btVector3 wVec = body->getAngularVelocity();
	//btVector3 axis = body->getWorldTransform().getBasis().getColumn(2);
	//float w = axis.dot(wVec);//wVec[2];
	//float controlVal = m_SpherePIDControl.step(w);

	////PRINT("Angular vel: " << wVec[0] << ", " << wVec[1] << ", " << wVec[2] << endl, WHITE);
	////PRINT("Angular vel: " << w << ", control: " << controlVal << endl, WHITE);


	////PRINT("Axis: " << axis[0] << ", " << axis[1] << ", " << axis[2] << endl, WHITE);

	//body->applyTorque(axis * controlVal);
	////body->applyTorque(btVector3(0,0,1)*controlVal);

	//// For 3rd person view!
	////Entity::Ptr view = m_Scene->getEntity(m_CharacterViewId);
	////vector3df target = view->getAbsPosition() + vector3df(1,0,0); //TODO: Doesn't work?? getAbsPosition() always seems to return zero, even if I add updateAbsolutePosition() inside Entity->getAbsPosition()
	//vector3df target = sphere->getPosition() + vector3df(0,1.5,0);
	////PRINT("Target: " << target.X << ", " << target.Y << ", " << target.Z << endl, WHITE);
	//m_SceneManager->getActiveCamera()->setTarget(target);

	//Entity::Ptr charBody = m_Scene->getEntity(m_CharacterBodyId);
	//body = (dynamic_pointer_cast<ComponentPhysical>(charBody->getComponent(COMPONENT_PHYSICAL)))->getBody();
	//body->setAngularVelocity(btVector3(0,(float)PI/6,0));
	////charBody->setRotation(charBody->getRotation() + vector3df(0,1.0f,0));

	//m_TestEntity1->setRotation(m_TestEntity1->getRotation() + vector3df(0, 0.2f, 0));
	//m_TestEntity2->setRotation(m_TestEntity2->getRotation() - vector3df(0, 0.2f, 0));

	//for (Entity::Ptr wheel : m_TestWheels)
	//{
	//	wheel->setRotation(wheel->getRotation() + vector3df(0.5f, 0, 0));
	//}

	if (m_FlashingLight)
	{
		m_CurrentMillis += (int)(delta * 1000);
		m_CurrentMillis = m_CurrentMillis % m_PeriodMillis;
		float factor = std::cos((float)m_CurrentMillis / (float)m_PeriodMillis * 2 * PI) * 0.5 + 0.5;
		factor = factor * (m_MaxBrightness - m_MinBrightness) + m_MinBrightness;
		m_FlashingLight->getLightData().DiffuseColor = SColorf(m_FlashingLightColor.r * factor, m_FlashingLightColor.g * factor, m_FlashingLightColor.b * factor);
		float ambientFactor = factor * m_AmbientFactor;
		m_FlashingLight->getLightData().AmbientColor = SColorf(m_FlashingLightColor.r * ambientFactor, m_FlashingLightColor.g * ambientFactor, m_FlashingLightColor.b * ambientFactor);
	}

	m_Scene->updateAll(delta);
	m_RenderPipeline->run();
}

void ContactScreen::debugFrame(float delta)
{
	m_DebugScene->updateScene();
	m_DebugSceneManager->drawAll();

	//// Draw irrRenderer multiple render targets
	//irr::core::recti gbuffRect= irr::core::recti(irr::core::vector2di(0, 0),m_Renderer->getMRT(0)->getSize());
	//for(irr::u32 i= 0; i < m_Renderer->getMRTCount(); i++)
	//{
	//	irr::core::recti gbuffRectSmall= irr::core::recti(irr::core::vector2di((int)(m_Renderer->getMRT(i)->getSize().Width/4.0f*i), 0), irr::core::vector2di((int)(m_Renderer->getMRT(i)->getSize().Width/4.0f*(i+1)), (int)(m_Renderer->getMRT(i)->getSize().Height/4.0f)));
	//	m_Device->getVideoDriver()->draw2DImage(m_Renderer->getMRT(i), gbuffRectSmall, gbuffRect);
	//}

	//m_Device->getVideoDriver()->setMaterial(video::SMaterial());
}

void ContactScreen::updateGUI(float delta)
{
	m_GUIEnvironment->drawAll();
}

RESULT ContactScreen::save()
{
	return R_E_NOTIMPL;
}

RESULT ContactScreen::load()
{
	return R_E_NOTIMPL;
}

void ContactScreen::OnCreateNode(ISceneNode* node)
{
	// TODO instantiate components now, as all of the node's children have been created
}
	
void ContactScreen::OnReadUserData(ISceneNode* forSceneNode, io::IAttributes* userData)
{
	// TODO load the data into ComponentSpawnData objects for later use, when the node's children have been created
}

io::IAttributes* ContactScreen::createUserData(ISceneNode* forSceneNode)
{
	// TODO get entity's components and tell them to write into ComponentSpawnData, then use the spawn data's serialization functions
	return m_FileSystem->createEmptyAttributes(m_VideoDriver);
}

void createCharacterCollisionModel()
{
	
}

void ContactScreen::createDesertBackdrop()
{
	///// SKY AND GROUND /////
	ISceneNode* skydome = m_SceneManager->addSkyDomeSceneNode(m_VideoDriver->getTexture("data\\assets\\RemusSkydomeTexturePack\\skydome.bmp"),
		16, 16, 1.0f, 1.0f);

	PRINT("Loading terrain...\n", WHITE);
	ITerrainSceneNode* terrain = m_SceneManager->addTerrainSceneNode("data\\assets\\terrain\\terrain-heightmap.bmp", 
		NULL, 
		-1, 
		vector3df(-15, -2, -30), 
		ZERO_VECTOR, 
		vector3df(0.5f, 0.05f, 0.5f), 
		video::SColor( 255, 255, 255, 255 ),
		5,
		scene::ETPS_17,
		4);

	if (terrain) {
		terrain->setMaterialFlag(video::EMF_LIGHTING, true);
		terrain->setMaterialFlag(video::EMF_BILINEAR_FILTER, false);
		terrain->setMaterialFlag(video::EMF_ANISOTROPIC_FILTER, true);
		terrain->setMaterialTexture(0, m_VideoDriver->getTexture("data\\assets\\terrain\\terrain-texture.jpg"));
		terrain->setMaterialTexture(1, m_VideoDriver->getTexture("data\\assets\\terrain\\detailmap3.jpg"));
		terrain->setMaterialType(video::EMT_DETAIL_MAP);

		terrain->scaleTexture(1.0f, 20.0f);

		scene::CDynamicMeshBuffer* buffer = new scene::CDynamicMeshBuffer(video::EVT_2TCOORDS, video::EIT_16BIT);
		terrain->getMeshBufferForLOD(*buffer, 0);
		SMesh irrTerrainMesh;
		irrTerrainMesh.addMeshBuffer(buffer);
		//video::S3DVertex2TCoords* data = (video::S3DVertex2TCoords*)buffer->getVertexBuffer().getData();

		PRINT("Loading terrain physics...\n", WHITE);
		btTriangleMesh *terrainMesh = Scene::convertToBtMesh(&irrTerrainMesh, terrain->getScale());
		btBvhTriangleMeshShape *terrainShape = new btBvhTriangleMeshShape(terrainMesh, true); // TODO: Leaked

		btRigidBody::btRigidBodyConstructionInfo rbInfoTerrain(0, NULL, terrainShape);
		rbInfoTerrain.m_startWorldTransform.setOrigin(btVec(terrain->getPosition()));
		btRigidBody *terrainBody = new btRigidBody(rbInfoTerrain); // TODO: Leaked
		terrainBody->setCollisionFlags(terrainBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
		terrainBody->setFriction(1.0f);

		// cleanup temporary buffer
		buffer->drop();

		m_Scene->getDynamicsWorld()->addCollisionObject(terrainBody);
	}

	//createOriginMarker();
}

void ContactScreen::createPlaneBackdrop()
{
	ISceneNode* skydome = m_SceneManager->addSkyDomeSceneNode(m_VideoDriver->getTexture("data\\assets\\RemusSkydomeTexturePack\\skydome.bmp"), 16, 16, 1.0f, 1.0f);

	Entity::Ptr e = m_EntFactory->makeEntity(m_SceneManager->getRootSceneNode());
	m_Scene->addEntity(e);

	ISceneNode *n = m_SceneManager->addCubeSceneNode(1.0f, e.get(), -1, ZERO_VECTOR, ZERO_VECTOR, vector3df(500, 1, 500));
	n->setMaterialTexture(0, m_VideoDriver->getTexture("data\\assets\\textures\\checker.bmp"));
	n->getMaterial(0).getTextureMatrix(0).setTextureScale(100,100);
	n->setMaterialFlag(EMF_LIGHTING, true);
	n->setName("plane_backdrop");

	ComponentCollider::SpawnData ccSpawn;
	ccSpawn.m_ShapeType = BroadphaseNativeTypes::BOX_SHAPE_PROXYTYPE;
	ccSpawn.m_BoundingBoxSize = btVec(n->getScale());
	ccSpawn.m_IsStatic = true;
	ccSpawn.m_Mass = 0;
	e->addComponent(m_CompFactory->createComponent(&ccSpawn));

	ComponentPhysical::SpawnData cpSpawn;
	cpSpawn.m_Friction = 0.1f;
	cpSpawn.m_RollingFriction = 0.01f;
	cpSpawn.m_HasContactResponse = true;
	cpSpawn.m_IsKinematic = false;
	cpSpawn.m_IsStatic = true;
	e->addComponent(m_CompFactory->createComponent(&cpSpawn));

	e->setPosition(vector3df(0, -0.5, 0));

	//createOriginMarker();
}

void ContactScreen::createOriginMarker()
{
	Entity::Ptr e = m_EntFactory->makeEntity(m_SceneManager->getRootSceneNode());
	m_Scene->addEntity(e);

	///// ORIGIN XYZ MARKERS /////
	ISceneNode *sNode = m_SceneManager->addCubeSceneNode(
		1.0f,
		e.get(),
		1,
		vector3df(0.5f,0,0),
		vector3df(0,0,0),
		vector3df(1.0f,0.1f,0.1f)
	);
	sNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	sNode->setName("origin_marker_x");

	m_SceneManager->addCubeSceneNode(
		1.0f,
		e.get(),
		2,
		vector3df(0,0.5f,0),
		vector3df(0,0,0),
		vector3df(0.1f,1.0f,0.1f)
	);
	sNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	sNode->setName("origin_marker_y");

	m_SceneManager->addCubeSceneNode(
		1.0f,
		e.get(),
		3,
		vector3df(0,0,0.5f),
		vector3df(0,0,0),
		vector3df(0.1f,0.1f,1.0f)
	);
	sNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	sNode->setName("origin_marker_z");
}

void ContactScreen::createSunlight()
{
	///// LIGHTS /////
	//m_SceneManager->addLightSceneNode(NULL, vector3df(100, 200, -100),SColorf(0.8f, 0.8f, 0.8f),100.0f);
	ILightSceneNode *light = m_SceneManager->addLightSceneNode(m_SceneManager->getRootSceneNode(), ZERO_VECTOR, SColorf(2.0f, 2.0f, 2.0f), 1000.0f);
	light->setLightType(ELT_DIRECTIONAL);
	light->setRotation(vector3df(45, 0, 0));
	//light->enableCastShadow(true);
	//m_SceneManager->setAmbientLight(SColorf(0.3f, 0.3f, 0.3f));

	// TEST
	light->setPosition(vector3df(100.0f, 1.0f, 0));
}

void ContactScreen::createSerializationTestScene() 
{
	createDesertBackdrop();
	createSunlight();

	// For mouse-controlled view!
	ICameraSceneNode* camera = m_SceneManager->addCameraSceneNodeFPS();
	camera->setPosition(vector3df(3, 0, 0));
	camera->setTarget(vector3df(3, 0, 1));
	//camera->setRotation(vector3df(0, 0, 0));


	/////// TEST OBJECTS /////
	//COMPONENT_MESH_SPAWN_DATA mesh;
	//mesh.meshType = COMPONENT_MESH_SPAWN_DATA::TYPE_PRIMITIVE_CUBE;
	//mesh.scale = vector3df(0.4f, 1.0f, 0.4f);
	//COMPONENT_PHYSICAL_SPAWN_DATA phys(BT_ZERO_VECTOR, BT_ZERO_VECTOR, 100.0); // Just a fudge for now (see below)
	//ENTITY_SPAWN_DATA spawn("test", vector3df(7,1.6f,5), ZERO_VECTOR, NULL, &phys);
	//spawn.compMesh.push_back(mesh);
	//Entity::Ptr e = m_EntFactory->makeEntity(spawn);
	//m_CharacterBodyId = e->getID();
	//(dynamic_pointer_cast<ComponentMesh>(e->getComponent(COMPONENT_MESH)))->getNode()->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);  // TODO: Getting node like this is rather painful...
	//dynamic_cast<IMeshSceneNode*>((dynamic_pointer_cast<ComponentMesh>(e->getComponent(COMPONENT_MESH)))->getNode())->addShadowVolumeSceneNode(); // We know it's an IMeshSceneNode, but gosh...
	Entity *e = dynamic_cast<Entity*>(m_SceneManager->addSceneNode(TYPE_NAME_ENTITY));
	e->setPosition(vector3df(7,1.6f,5));
	ISceneNode *charBody = m_SceneManager->addCubeSceneNode(1.0f, e, 1, ZERO_VECTOR, ZERO_VECTOR, vector3df(0.4f, 1.0f, 0.4f));
	charBody->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
	m_Scene->addEntity(e);

	//// TODO: move these shape parameters into entity factory and COMPONENT_PHYSICAL_SPAWN_DATA
	//ComponentPhysical::Ptr compPhys = dynamic_pointer_cast<ComponentPhysical>(e->getComponent(COMPONENT_PHYSICAL));
	//btRigidBody::btRigidBodyConstructionInfo rbInfoBox(1.0f, NULL, new btBoxShape(btVec(mesh.scale/2)), btVector3(1.0, 1.0, 1.0)); // Shape mem leaked
	//rbInfoBox.m_startWorldTransform.setOrigin(btVec(spawn.startPos));
	//btRigidBody* body = new btRigidBody(rbInfoBox); // TODO: Leaked
	//body->setAngularFactor(btVector3(0,0,0)); // We will handle all rotation un-physically
	//compPhys->setBody(body);
	
	//body->setAngularFactor(btVector3(0,0,0)); // We will handle all rotation un-physically

	ComponentCollider::SpawnData ccSpawn;
	ccSpawn.m_ShapeType = BroadphaseNativeTypes::BOX_SHAPE_PROXYTYPE;
	ccSpawn.m_BoundingBoxSize = btVec(charBody->getScale());
	ccSpawn.m_IsStatic = false; // Set to false just for testing
	ccSpawn.m_Mass = 1.0f;
	e->addComponent(m_CompFactory->createComponent(&ccSpawn));

	ComponentPhysical::SpawnData cpSpawn;
	cpSpawn.m_Friction = 0.1f;
	cpSpawn.m_RollingFriction = 0.1f;
	cpSpawn.m_HasContactResponse = true;
	cpSpawn.m_IsKinematic = false;
	cpSpawn.m_IsStatic = false;
	//ComponentPhysical::Ptr compPhys = dynamic_pointer_cast<ComponentPhysical>(m_CompFactory->createComponent(&compPhysSpawn));
	e->addComponent(m_CompFactory->createComponent(&cpSpawn));

	//COMPONENT_MESH_SPAWN_DATA mesh2;
	//ENTITY_SPAWN_DATA spawn2("test2", vector3df(7,0.3f,5), ZERO_VECTOR, NULL, &phys);
	//mesh2.meshType = COMPONENT_MESH_SPAWN_DATA::TYPE_PRIMITIVE_SPHERE;
	////mesh2.meshType = COMPONENT_MESH_SPAWN_DATA::TYPE_PRIMITIVE_CUBE;
	//mesh2.scale = vector3df(0.6f, 0.6f, 0.6f);
	//spawn2.compMesh.push_back(mesh2);
	//Entity::Ptr e4 = m_EntFactory->makeEntity(spawn2);
	//m_CharacterSphereId = e4->getID();
	//(dynamic_pointer_cast<ComponentMesh>(e4->getComponent(COMPONENT_MESH)))->getNode()->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
	//dynamic_cast<IMeshSceneNode*>((dynamic_pointer_cast<ComponentMesh>(e4->getComponent(COMPONENT_MESH)))->getNode())->addShadowVolumeSceneNode();
	////(dynamic_pointer_cast<ComponentMesh>(e4->getComponent(COMPONENT_MESH)))->getNode()->setMaterialFlag(EMF_GOURAUD_SHADING, true);
	e = dynamic_cast<Entity*>(m_SceneManager->addSceneNode(TYPE_NAME_ENTITY));
	e->setPosition(vector3df(7,0.3f,5));
	float sphereWheelRadius = 0.3f;
	charBody = m_SceneManager->addSphereSceneNode(sphereWheelRadius, 16, e, 2, ZERO_VECTOR, ZERO_VECTOR); 
	m_Scene->addEntity(e);

	//compPhys = dynamic_pointer_cast<ComponentPhysical>(e4->getComponent(COMPONENT_PHYSICAL));
	//btRigidBody::btRigidBodyConstructionInfo rbInfoSphere(1.0f, NULL, new btSphereShape(mesh2.scale.X/2), btVector3(1.0, 1.0, 1.0)); // Shape mem leaked
	////btRigidBody::btRigidBodyConstructionInfo rbInfoSphere(20, NULL, new btCylinderShapeZ(btVec(mesh2.scale/2)), btVector3(1.0, 1.0, 1.0)); // Shape mem leaked
	//rbInfoSphere.m_startWorldTransform.setOrigin(btVec(spawn2.startPos));
	//btRigidBody *bodySphere = new btRigidBody(rbInfoSphere); // TODO: Leaked
	//bodySphere->setFriction(3.0f);

	ccSpawn.m_ShapeType = BroadphaseNativeTypes::SPHERE_SHAPE_PROXYTYPE;
	ccSpawn.m_BoundingBoxSize = btVector3(sphereWheelRadius, sphereWheelRadius, sphereWheelRadius) * 2;
	ccSpawn.m_IsStatic = false; // Set to false just for testing
	e->addComponent(m_CompFactory->createComponent(&ccSpawn));

	e->addComponent(m_CompFactory->createComponent(&cpSpawn));

	//btHingeConstraint *hingeConst = new btHingeConstraint(*body, *bodySphere, btVector3(0,-1.0f,0), BT_ZERO_VECTOR, BT_UNIT_Z, BT_UNIT_Z);
	//m_Scene->getDynamicsWorld()->addConstraint(hingeConst);


	//// Make some stairs!
	//vector3df stairsStart(3, -0.7f, 5);
	//float stairsRise = 0.2f;
	//float stairsRun = 0.28f;
	//float stairsWidth = 1.0f;
	//int stairsNum = 10;

	//mesh.meshType = COMPONENT_MESH_SPAWN_DATA::TYPE_PRIMITIVE_CUBE;
	//mesh.scale = vector3df(stairsRun, stairsRise, stairsWidth);
	//spawn.compMesh.clear();
	//spawn.compMesh.push_back(mesh);
	//spawn.compPhys = &phys; // Fudge
	//btRigidBody::btRigidBodyConstructionInfo rbInfoStep(0, NULL, new btBoxShape(btVec(mesh.scale/2))); // Shape mem leaked
	//for (int i = 0; i < stairsNum; i++)
	//{
	//	spawn.startPos = stairsStart;
	//	spawn.startPos.X -= i * stairsRun;
	//	spawn.startPos.Y += i * stairsRise;

	//	rbInfoStep.m_startWorldTransform.setOrigin(btVec(spawn.startPos));
	//	btRigidBody *bodyStep = new btRigidBody(rbInfoStep); // TODO: Leaked
	//	bodyStep->setFriction(1.0f);

	//	Entity::Ptr e5 = m_EntFactory->makeEntity(spawn);
	//	compPhys = dynamic_pointer_cast<ComponentPhysical>(e5->getComponent(COMPONENT_PHYSICAL));
	//	compPhys->setBody(bodyStep);

	//	(dynamic_pointer_cast<ComponentMesh>(e5->getComponent(COMPONENT_MESH)))->getNode()->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
	//	dynamic_cast<IMeshSceneNode*>((dynamic_pointer_cast<ComponentMesh>(e5->getComponent(COMPONENT_MESH)))->getNode())->addShadowVolumeSceneNode();

	//	m_Scene->addEntity(e5);
	//}

	////body->applyForce(btVector3(0.0f, 0.0f, 100.0f), btVector3(0,0,0));
	////body->applyTorque(btVector3(0, 100.0f, 0));



	//// For 3rd person view!
	//COMPONENT_VIEW_SPAWN_DATA view(vector3df(3,1,0), vector3df(0,0,0), true);
	//spawn.name = "Test2";
	//spawn.startPos = vector3df(0,0,0);
	//spawn.compView = &view;
	//spawn.compPhys = NULL;
	//spawn.compMesh.clear();
	//Entity::Ptr e2 = m_EntFactory->makeEntity(spawn);
	//m_CharacterViewId = e2->getID();
	//e->addChild(e2.get());

	//m_Scene->addEntity(e);
	//// For 3rd person view!
	//m_Scene->addEntity(e2);
	//m_Scene->addEntity(e4);

	////spawn.compView = NULL;
	////spawn.startPos = vector3df(0,-3,0);
	////Entity::Ptr e3 = m_EntFactory->makeEntity(spawn);
	////// To test entity attachment (adding to scene to make sure it removes itself)
	////m_Scene->addEntity(e3);
	////e->addChild(e3);

	//spawn.compView = NULL;
	//spawn.startPos = vector3df(0,-3,0);
	////Entity::Ptr e3 = m_EntFactory->makeEntity(spawn);
	////// To test entity attachment (adding to scene to make sure it removes itself)
	////m_Scene->addEntity(e3);
	////e->addChild(e3);



	Entity::fireSubtreeUpdates(m_SceneManager->getRootSceneNode());


	///// SCRIPTING STUFF /////

	//m_ScriptManager = ScriptManager::Ptr(new ScriptManager());
	//m_RegisterWrapper = RegisterWrapper::Ptr(new RegisterWrapper(m_ScriptManager->getLuaState(),this->m_EntFactory,this->m_Scene.get()));
	//m_RegisterWrapper->RegisterFunctions();
	////m_ScriptManager->loadScripts();
	////m_ScriptManager->executeScripts();
	//
	//m_ScriptManager->executeScript("data\\scripts\\Bindings\\Bindings.lua");
	//
	////m_ScriptManager->executeScript("data\\heyoHiyo\\lua.lua");
}

void ContactScreen::createCompoundShapeTestScene1(void)
{
	createDesertBackdrop();
	createSunlight();

	m_DebugScene->setDrawBoundingBox(true);

	ICameraSceneNode* camera = m_SceneManager->addCameraSceneNodeFPS();
	camera->setPosition(vector3df(3, 1.5, -7));
	camera->setTarget(vector3df(3, 0, 1));

	Entity::Ptr e;
	e = createTetrisShape(3);
	e->setPosition(vector3df(2,0.5f,0));
	m_DebugScene->trackNodeSubtree(e.get());

	e = createTetrisShape(5);
	e->setRotation(vector3df(0, 0, -90.0f));
	e->setPosition(vector3df(2.4f,2.1f,0));
	m_DebugScene->trackNodeSubtree(e.get());

	e = createTetrisShape(6);
	//vector3df rot = e->getRelativeTransformation().getRotationDegrees();
	//PRINT("Before: " << rot.X << ", " << rot.Y << ", " << rot.Z << endl, WHITE);
	e->setPosition(vector3df(2,2.5f,0));
	e->setRotation(vector3df(0, 0, 90.0f));
	//rot = e->getRelativeTransformation().getRotationDegrees();
	//PRINT("After: " << rot.X << ", " << rot.Y << ", " << rot.Z << endl, WHITE);
	m_DebugScene->trackNodeSubtree(e.get());

	e = createTetrisShape(5);
	e->setRotation(vector3df(0, 0, -90.0f));
	e->setPosition(vector3df(2.4f,4.9f,0));
	m_DebugScene->trackNodeSubtree(e.get());

	e = createTetrisShape(2);
	e->setRotation(vector3df(0, 0, -90.0f));
	e->setPosition(vector3df(2.8f,6.1f,0));
	m_DebugScene->trackNodeSubtree(e.get());

	//for (int i = 0; i < 10; i++)
	//{
	//	e = createTetrisShape((i+1) % 7);
	//	e->setRotation(vector3df(34.0f*i, 248.0f*i, -21.0f*i));
	//	e->setPosition(vector3df(3.0f, 20.0f + 1.0f*i,0));
	//}

	Entity::fireSubtreeUpdates(m_SceneManager->getRootSceneNode());

}

Entity::Ptr ContactScreen::createTetrisShape(int which)
{
	Entity::Ptr base = m_EntFactory->makeEntity(m_SceneManager->getRootSceneNode());
	Entity::Ptr e, e2;
	m_Scene->addEntity(base);

	ComponentPhysical::SpawnData cpSpawn;
	cpSpawn.m_Friction = 0.5f;
	cpSpawn.m_RollingFriction = 0.05f;
	cpSpawn.m_HasContactResponse = true;
	cpSpawn.m_IsKinematic = false;
	cpSpawn.m_IsStatic = false;

	float blkSz = 0.4f;

	// The origin of the shape will be the center of the top-left square, whether it is empty or not!!

	switch(which)
	{
	case 0: // Straight shape
		e = createTetrisBlock(blkSz, 4);
		e->setPosition(vector3df(0, -1.5*blkSz, 0));
		base->addChild(e.get());
		break;
	case 1: // Backwards L shape
		e = createTetrisBlock(blkSz, 2);
		e->setPosition(vector3df(blkSz, -blkSz/2,0)); 
		e2 = createTetrisBlock(blkSz, 2);
		e2->setRotation(vector3df(0, 0, 90.0f));
		e2->setPosition(vector3df(-blkSz/2, -1.5*blkSz, 0));

		base->addChild(e.get());
		e->addChild(e2.get());
		break;
	case 2: // L shape
		e = createTetrisBlock(blkSz, 2);
		e->setPosition(vector3df(0, -blkSz/2,0)); 
		e2 = createTetrisBlock(blkSz, 2);
		e2->setRotation(vector3df(0, 0, 90.0f));
		e2->setPosition(vector3df(blkSz/2, -1.5*blkSz, 0));

		base->addChild(e.get());
		e->addChild(e2.get());
		break;
	case 3: // Cube shape
		e = createTetrisBlock(blkSz, 2);
		e->setPosition(vector3df(0,-blkSz/2,0));
		e2 = createTetrisBlock(blkSz, 2);
		e2->setPosition(vector3df(blkSz,0,0));

		base->addChild(e.get());
		e->addChild(e2.get());
		break;
	case 4: // Z shape
		e = createTetrisBlock(blkSz, 2);
		e->setRotation(vector3df(0,0,90.0f)); // Counter-clockwise rotation
		e->setPosition(vector3df(-blkSz/2, 0,0)); 
		e2 = createTetrisBlock(blkSz, 2);
		e2->setPosition(vector3df(-blkSz, -blkSz, 0));

		base->addChild(e.get());
		e->addChild(e2.get());
		break;
	case 5: // T-shape
		e = createTetrisBlock(blkSz, 1);
		e->setPosition(vector3df(blkSz, 0,0)); 
		e2 = createTetrisBlock(blkSz, 3);
		e2->setRotation(vector3df(0,0,90.0f)); // Counter-clockwise rotation
		e2->setPosition(vector3df(0, -blkSz, 0));

		base->addChild(e.get());
		e->addChild(e2.get());
		break;
	default: // Backwards Z shape
		e = createTetrisBlock(blkSz, 2);
		e->setRotation(vector3df(0,0,90.0f)); // Counter-clockwise rotation
		e->setPosition(vector3df(3*blkSz/2, 0, 0)); 
		e2 = createTetrisBlock(blkSz, 2);
		e2->setPosition(vector3df(-blkSz, blkSz, 0));

		base->addChild(e.get());
		e->addChild(e2.get());
		break;
	}

	base->addComponent(m_CompFactory->createComponent(&cpSpawn));

	return base;
}

Entity::Ptr ContactScreen::createTetrisBlock(float blkSz, int len)
{
	Entity::Ptr e = m_EntFactory->makeEntity(NULL);
	m_Scene->addEntity(e);

	ComponentCollider::SpawnData ccSpawn;
	ccSpawn.m_IsStatic = true;
	ccSpawn.m_ShapeType = BroadphaseNativeTypes::BOX_SHAPE_PROXYTYPE;
	ccSpawn.m_Mass = 0.2f * len;

	ISceneNode *pNode = m_SceneManager->addCubeSceneNode(1.0f, e.get(), 1, ZERO_VECTOR, ZERO_VECTOR, vector3df(blkSz, blkSz * len, blkSz));
	ccSpawn.m_BoundingBoxSize = btVec(pNode->getScale());
	e->addComponent(m_CompFactory->createComponent(&ccSpawn));

	return e;
}


void ContactScreen::createCompoundShapeTestScene2(void)
{
	createDesertBackdrop();
	createSunlight();

	ICameraSceneNode* camera = m_SceneManager->addCameraSceneNodeFPS();
	camera->setPosition(vector3df(4, 7, -7));
	camera->setTarget(vector3df(4, 0, 1));

	Entity::Ptr e = m_EntFactory->makeEntity(m_SceneManager->getRootSceneNode());
	m_Scene->addEntity(e);

	ComponentCollider::SpawnData ccSpawn;
	ccSpawn.m_IsStatic = false; // Just for testing
	ccSpawn.m_ShapeType = BroadphaseNativeTypes::BOX_SHAPE_PROXYTYPE;
	ccSpawn.m_Mass = 2.0f;

	ISceneNode *pNode = m_SceneManager->addCubeSceneNode(1.0f, e.get(), 1, ZERO_VECTOR, ZERO_VECTOR, vector3df(5, 0.5, 5));
	ccSpawn.m_BoundingBoxSize = btVec(pNode->getScale());
	e->addComponent(m_CompFactory->createComponent(&ccSpawn));

	e->setPosition(vector3df(4, 0.5, 0));

	ComponentPhysical::SpawnData cpSpawn;
	cpSpawn.m_Friction = 0.5f;
	cpSpawn.m_RollingFriction = 0.01f;
	cpSpawn.m_HasContactResponse = true;
	cpSpawn.m_IsKinematic = true;
	cpSpawn.m_IsStatic = false;

	e->addComponent(m_CompFactory->createComponent(&cpSpawn));

	Entity::Ptr e2 = m_EntFactory->makeEntity(e.get());
	pNode = m_SceneManager->addCubeSceneNode(1.0f, e2.get(), 1, ZERO_VECTOR, ZERO_VECTOR, vector3df(5, 0.5, 0.5));
	ccSpawn.m_BoundingBoxSize = btVec(pNode->getScale());
	e2->addComponent(m_CompFactory->createComponent(&ccSpawn));
	e2->setPosition(vector3df(0, 0.5f, 0));
	
	m_TestEntity1 = e2;

	e2 = m_EntFactory->makeEntity(e.get());
	pNode = m_SceneManager->addCubeSceneNode(1.0f, e2.get(), 1, ZERO_VECTOR, ZERO_VECTOR, vector3df(5, 0.5, 0.5));
	ccSpawn.m_BoundingBoxSize = btVec(pNode->getScale());
	e2->addComponent(m_CompFactory->createComponent(&ccSpawn));
	e2->setPosition(vector3df(0, 1.0f, 0));
	
	m_TestEntity2 = e2;

	e2 = m_EntFactory->makeEntity(e.get());
	pNode = m_SceneManager->addCubeSceneNode(1.0f, e2.get(), 1, ZERO_VECTOR, ZERO_VECTOR, vector3df(6, 2.0, 0.5));
	ccSpawn.m_BoundingBoxSize = btVec(pNode->getScale());
	e2->addComponent(m_CompFactory->createComponent(&ccSpawn));
	e2->setPosition(vector3df(0, 0.75, -2.75));

	e2 = m_EntFactory->makeEntity(e.get());
	pNode = m_SceneManager->addCubeSceneNode(1.0f, e2.get(), 1, ZERO_VECTOR, ZERO_VECTOR, vector3df(6, 2.0, 0.5));
	ccSpawn.m_BoundingBoxSize = btVec(pNode->getScale());
	e2->addComponent(m_CompFactory->createComponent(&ccSpawn));
	e2->setPosition(vector3df(0, 0.75, 2.75));

	e2 = m_EntFactory->makeEntity(e.get());
	pNode = m_SceneManager->addCubeSceneNode(1.0f, e2.get(), 1, ZERO_VECTOR, ZERO_VECTOR, vector3df(0.5, 2.0, 5));
	ccSpawn.m_BoundingBoxSize = btVec(pNode->getScale());
	e2->addComponent(m_CompFactory->createComponent(&ccSpawn));
	e2->setPosition(vector3df(-2.75, 0.75, 0));

	e2 = m_EntFactory->makeEntity(e.get());
	pNode = m_SceneManager->addCubeSceneNode(1.0f, e2.get(), 1, ZERO_VECTOR, ZERO_VECTOR, vector3df(0.5, 2.0, 5));
	ccSpawn.m_BoundingBoxSize = btVec(pNode->getScale());
	e2->addComponent(m_CompFactory->createComponent(&ccSpawn));
	e2->setPosition(vector3df(2.75, 0.75, 0));

	for (int i = 0; i < 10; i++)
	{
		e = createTetrisShape((i+1) % 7);
		e->setRotation(vector3df(34.0f*i, 248.0f*i, -21.0f*i));
		e->setPosition(vector3df(4.0f, 10.0f + 1.0f*i,0));
	}

	Entity::fireSubtreeUpdates(m_SceneManager->getRootSceneNode());
}

//void ContactScreen::setAllNodeMaterialFlags(ISceneNode *node, E_MATERIAL_FLAG flag, bool newValue)
//{
//	node->setMaterialFlag(flag, newValue);
//	PRINT("Setting flags for node " << hex << node << endl, WHITE);
//
//	const irr::core::list<ISceneNode*>& childList = node->getChildren();
//	for (irr::core::list<ISceneNode*>::ConstIterator it = childList.begin(); it != childList.end(); it++)
//	{
//		setAllNodeMaterialFlags(*it, flag, newValue);
//	}
//}

// This is a physics performance test
void ContactScreen::createCompoundShapeTestScene3(void)
{
	createPlaneBackdrop();
	createSunlight();

	ICameraSceneNode* camera = m_SceneManager->addCameraSceneNodeFPS();
	camera->setPosition(vector3df(0, 2, -10));
	camera->setTarget(vector3df(0, 2, 0));

	m_DebugScene->setDrawBoundingBox(true);
	m_DebugScene->setDrawPhysicsShape(true);

	Entity::Ptr e;

	for (int i = 0; i < 50; i++)
	{
		e = createTetrisShape((i+1) % 7);
		e->setRotation(vector3df(34.0f*i, 248.0f*i, -21.0f*i));
		e->setPosition(vector3df(0.0f, 10.0f + 2.0f*i,0));
		m_DebugScene->trackNodeSubtree(e.get());
	}

	Entity::fireSubtreeUpdates(m_SceneManager->getRootSceneNode());
}

void ContactScreen::createCompoundShapeTestScene4(void)
{
	createDesertBackdrop();
	createSunlight();

	ICameraSceneNode* camera = m_SceneManager->addCameraSceneNodeFPS();
	camera->setPosition(vector3df(2, 2, -8.5));
	camera->setTarget(vector3df(0, 2, 0));

	m_DebugScene->setDrawBoundingBox(false);
	m_DebugScene->setDrawPhysicsShape(true);

	ComponentCollider::SpawnData ccSpawn;

	ComponentPhysical::SpawnData cpSpawn;
	cpSpawn.m_Friction = 1.0f;
	cpSpawn.m_RollingFriction = 0.1f;
	cpSpawn.m_HasContactResponse = true;
	cpSpawn.m_IsKinematic = false;
	cpSpawn.m_IsStatic = false;

	Entity::Ptr e, e2, e3;
	IAnimatedMesh* mesh;
	IAnimatedMeshSceneNode* meshNode;

	// Tank body
	e = m_EntFactory->makeEntity(m_SceneManager->getRootSceneNode());
	m_Scene->addEntity(e);

	mesh = m_SceneManager->getMesh("data\\assets\\models\\AUSFB\\tank_base.3ds");
	meshNode = m_SceneManager->addAnimatedMeshSceneNode(mesh, e.get(), -1);

	e->addComponent(m_CompFactory->createComponent(&cpSpawn));


	// Collider for tank body
	e2 = m_EntFactory->makeEntity(e.get());
	m_Scene->addEntity(e2);

	// German Tiger II tank weighs 69.8 tonnes, or about 1000 humans
	ccSpawn.m_Mass = 1000.0f;
	ccSpawn.m_BoundingBoxSize = btVector3(3.5f, 1.5f, 6.8f);
	ccSpawn.m_ShapeType = BOX_SHAPE_PROXYTYPE;
	ccSpawn.m_IsStatic = true;

	e2->setPosition(vector3df(0, 1.25f, 0));
	e2->addComponent(m_CompFactory->createComponent(&ccSpawn));


	// Colliders for wheels!
	ccSpawn.m_Mass = 0;
	ccSpawn.m_IsStatic = false;
	ccSpawn.m_ShapeType = CYLINDER_SHAPE_PROXYTYPE;
	ccSpawn.m_Orientation = 0;
	ccSpawn.m_BoundingBoxSize = btVector3(0.8f, 0.8f, 0.8f);

	for (int i = 0; i < 5; i++)
	{
		e2 = m_EntFactory->makeEntity(e.get());
		m_Scene->addEntity(e2);
		e2->setPosition(vector3df(1.4f, 0.4f, 2.0f - i));
		e2->addComponent(m_CompFactory->createComponent(&ccSpawn));
		m_TestWheels.push_back(e2);

		e2 = m_EntFactory->makeEntity(e.get());
		m_Scene->addEntity(e2);
		e2->setPosition(vector3df(-1.4f, 0.4f, 2.0f - i));
		e2->addComponent(m_CompFactory->createComponent(&ccSpawn));
		m_TestWheels.push_back(e2);
	}

	// Two more for sprocket wheels up front
	e2 = m_EntFactory->makeEntity(e.get());
	m_Scene->addEntity(e2);
	e2->setPosition(vector3df(1.4f, 0.8f, 2.8f));
	e2->addComponent(m_CompFactory->createComponent(&ccSpawn));
	m_TestWheels.push_back(e2);

	e2 = m_EntFactory->makeEntity(e.get());
	m_Scene->addEntity(e2);
	e2->setPosition(vector3df(-1.4f, 0.8f, 2.8f));
	e2->addComponent(m_CompFactory->createComponent(&ccSpawn));
	m_TestWheels.push_back(e2);


	// Tank turret
	e2 = m_EntFactory->makeEntity(e.get());
	m_Scene->addEntity(e2);

	mesh = m_SceneManager->getMesh("data\\assets\\models\\AUSFB\\tank_turret.3ds");
	meshNode = m_SceneManager->addAnimatedMeshSceneNode(mesh, e2.get(), -1);

	m_TestEntity1 = e2;
	
	// Collider for tank turret
	e3 = m_EntFactory->makeEntity(e2.get());
	m_Scene->addEntity(e3);

	ccSpawn.m_Mass = 0;
	ccSpawn.m_IsStatic = false;
	ccSpawn.m_BoundingBoxSize = btVector3(3.0f, 1.0f, 4.0f);
	ccSpawn.m_ShapeType = BOX_SHAPE_PROXYTYPE;

	e3->setPosition(vector3df(0, 2.5f, -0.25f));
	e3->addComponent(m_CompFactory->createComponent(&ccSpawn));


	// Collider for the cannon
	e3 = m_EntFactory->makeEntity(e2.get());
	m_Scene->addEntity(e3);

	ccSpawn.m_Mass = 0;
	ccSpawn.m_IsStatic = false;
	ccSpawn.m_ShapeType = CYLINDER_SHAPE_PROXYTYPE;
	ccSpawn.m_Orientation = 2;
	ccSpawn.m_BoundingBoxSize = btVector3(0.4f, 0.4f, 5.3f);

	e3->setPosition(vector3df(0, 2.22f, 3.5f));
	e3->addComponent(m_CompFactory->createComponent(&ccSpawn));



	m_DebugScene->trackNodeSubtree(e.get());

	e->setPosition(vector3df(1, 10.0f, 1.5));
	e->setRotation(vector3df(0, 90, 0));


	// Make some tetris to knock over
	for (int i = 0; i < 5; i++)
	{
		e3 = createTetrisShape(3);
		e3->setPosition(vector3df(1.5, 0.1 + 0.81*i, -2));
		m_DebugScene->trackNodeSubtree(e3.get());
	}

	//m_SceneManager->addLightSceneNode(m_SceneManager->getRootSceneNode(), camera->getPosition(), SColorf(0.3f, 1.0f, 0.3f), 10.0f);
	///*ILightSceneNode *light = m_SceneManager->addLightSceneNode(m_SceneManager->getRootSceneNode(), ZERO_VECTOR, SColorf(2.0f, 2.0f, 2.0f), 0.0f);
	//light->setLightType(ELT_DIRECTIONAL);
	//light->setRotation(vector3df(45, 0, 0));
	//light->enableCastShadow(true);*/

	//m_SceneManager->setAmbientLight(SColorf(0, 0, 0));

}

void ContactScreen::createCompoundShapeTestScene5(void)
{
	createPlaneBackdrop();
	createSunlight();
	//createOriginMarker();

	ICameraSceneNode* camera = m_SceneManager->addCameraSceneNodeFPS();
	camera->setPosition(vector3df(0, 5, -10));
	camera->setTarget(vector3df(0, 5, 0));

	ComponentCollider::SpawnData ccSpawn;
	ccSpawn.m_Mass = 0;
	ccSpawn.m_IsStatic = false;
	ccSpawn.m_ShapeType = BOX_SHAPE_PROXYTYPE;

	ComponentPhysical::SpawnData cpSpawn;
	cpSpawn.m_Friction = 1.0f;
	cpSpawn.m_RollingFriction = 0.1f;
	cpSpawn.m_HasContactResponse = false;
	cpSpawn.m_IsKinematic = true;
	cpSpawn.m_IsStatic = false;

	Entity::Ptr e, e2;

	e = m_EntFactory->makeEntity(m_SceneManager->getRootSceneNode());
	m_Scene->addEntity(e);

	IAnimatedMesh* mesh = m_SceneManager->getMesh("data\\assets\\models\\ninja\\ninja.b3d");
    IAnimatedMeshSceneNode* node = m_SceneManager->addAnimatedMeshSceneNode(mesh, e.get());
	//node->setScale(vector3df(0.2f, 0.2f, 0.2f));
	//node->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
	//node->setMaterialFlag(EMF_LIGHTING, false);
    node->setMaterialTexture( 0, m_VideoDriver->getTexture("data\\assets\\models\\ninja\\nskinrd.jpg") );
	node->setAnimationSpeed(2);
	node->setDebugDataVisible(EDS_SKELETON);
	

	PRINT("=== Animated mesh has " << dec << node->getJointCount() << " joints" << endl, WHITE);

	////m_SceneManager->addSphereSceneNode(0.5f, 16, bone);
	//meshNode = m_SceneManager->addCubeSceneNode(1, bone, -1, ZERO_VECTOR, ZERO_VECTOR, vector3df(2, 0.02f, 0.02f));
	//manip->setVertexColors(meshNode->getMesh()->getMeshBuffer(0), SColor(0, 255, 0, 0));
	//meshNode = m_SceneManager->addCubeSceneNode(1, bone, -1, ZERO_VECTOR, ZERO_VECTOR, vector3df(0.02f, 2, 0.02f));
	//manip->setVertexColors(meshNode->getMesh()->getMeshBuffer(0), SColor(0, 0, 255, 0));
	//meshNode = m_SceneManager->addCubeSceneNode(1, bone, -1, ZERO_VECTOR, ZERO_VECTOR, vector3df(0.02f, 0.02f, 2));
	//manip->setVertexColors(meshNode->getMesh()->getMeshBuffer(0), SColor(0, 0, 0, 255));

	PRINT("=== Animated mesh structure:" << endl, WHITE);
	printSubtreeTypes(node);

	ISkinnedMesh *sMesh = (ISkinnedMesh*)(node->getMesh());
	const core::array<ISkinnedMesh::SJoint*> &jointList = sMesh->getAllJoints();
	core::array<SSkinMeshBuffer*>& sMeshBuffers = sMesh->getMeshBuffers();

	for (int i = 0; i < jointList.size(); i++)
	{
		ISkinnedMesh::SJoint *joint = jointList[i];
		IBoneSceneNode *bone = node->getJointNode(i); // Causes joint scene nodes to be added
		aabbox3df bBox(0, 0, 0, 0, 0, 0);

		//PRINT("Joint: " << joint->Name.c_str() << " has " << joint->AttachedMeshes.size() << " attached meshes, " << joint->Weights.size() << " weights" << endl, WHITE);
		for (int j = 0; j < joint->Weights.size(); j++)
		{
			SSkinMeshBuffer *sMeshBuffer = sMeshBuffers[joint->Weights[j].buffer_id];
			S3DVertex *v = sMeshBuffer->getVertex(joint->Weights[j].vertex_id);
			//PRINT("  " << j << " -> Buffer " << joint->Weights[j].buffer_id << ", vertex " << joint->Weights[j].vertex_id << ", strength " << joint->Weights[j].strength << " coord " << v->Pos.X << "," << v->Pos.Y << "," << v->Pos.Z << endl, WHITE);

			vector3df vertPos = v->Pos;
			joint->GlobalAnimatedMatrix.inverseTranslateVect(vertPos);
			joint->GlobalAnimatedMatrix.inverseRotateVect(vertPos);

			bBox.addInternalPoint(vertPos);
		}

		e2 = m_EntFactory->makeEntity(bone);
		e2->setPosition(bBox.getCenter());

		ccSpawn.m_BoundingBoxSize = btVec(bBox.getExtent());
		e2->addComponent(m_CompFactory->createComponent(&ccSpawn));
	}

	e->addComponent(m_CompFactory->createComponent(&cpSpawn));

	e->setPosition(vector3df(0, 0.4f, 0));
	e->setRotation(vector3df(0, 135, 0));

	m_DebugScene->setDrawBoundingBox(false);
	m_DebugScene->setDrawPhysicsShape(true);

	m_DebugScene->trackNodeSubtree(e.get());
}

void ContactScreen::createShaderTestScene1(void)
{
	const MaterialLibrary &matLib = m_RenderPipeline->getMaterialLibrary();

	createPlaneBackdrop();
	//createSunlight();
	//createOriginMarker();

	ICameraSceneNode* camera = m_SceneManager->addCameraSceneNodeFPS();
	camera->setPosition(vector3df(0, 3, -6));
	camera->setTarget(vector3df(0, 3, 0));
	//camera->bindTargetAndRotation(true);
	ISceneNodeAnimator *fpsAnimator = *(camera->getAnimators().begin());
	((ISceneNodeAnimatorCameraFPS*)fpsAnimator)->setMoveSpeed(2.0f/1000.0f);

	Entity::Ptr e;

	e = m_EntFactory->makeEntity(m_SceneManager->getRootSceneNode());
	m_Scene->addEntity(e);

	IAnimatedMesh* mesh = m_SceneManager->getMesh("data\\assets\\models\\ED-209\\ED-209.obj");
    IAnimatedMeshSceneNode* node = m_SceneManager->addAnimatedMeshSceneNode(mesh, e.get());

	//ISceneNode* node = m_SceneManager->addSphereSceneNode(1.0);
	//node->setMaterialTexture(0, m_VideoDriver->getTexture("data\\assets\\textures\\white.bmp"));
	//node->setPosition(vector3df(0, 2, 0));

	node->setMaterialType(matLib["d_solid"]);
	node->getMaterial(0).SpecularColor = SColor(0, 128, 0, 0);
	node->getMaterial(0).Shininess = 50.0f;

	e->setRotation(vector3df(0, 135, 0));

	// Add some point lights circling around
	for (int i = 0; i < 1; i++)
	{
		ILightSceneNode *light = m_SceneManager->addLightSceneNode(m_SceneManager->getRootSceneNode(), ZERO_VECTOR, SColorf(1.0f, 1.0f, 1.0f), 1.0f);
		light->setLightType(ELT_POINT);

		//light->setPosition(vector3df(0.0f, 0.0f, 0));
		ISceneNodeAnimator *animator = m_SceneManager->createFlyCircleAnimator(vector3df(0,3,0), 3.0f, 0.001f); // Leaked, should be drop()'d but too lazy
		light->addAnimator(animator);
		//light->setPosition(vector3df(2.0f, 3.0f, -2.0f));
	}
}

void ContactScreen::createShaderTestScene2(void)
{
	const MaterialLibrary &matLib = m_RenderPipeline->getMaterialLibrary();

	createPlaneBackdrop();

	ICameraSceneNode* camera = m_SceneManager->addCameraSceneNodeFPS();
	camera->setPosition(vector3df(0, 3, -6));
	camera->setTarget(vector3df(0, 3, 0));
	ISceneNodeAnimator *fpsAnimator = *(camera->getAnimators().begin());
	((ISceneNodeAnimatorCameraFPS*)fpsAnimator)->setMoveSpeed(2.0f/1000.0f);

	Entity::Ptr e;

	e = m_EntFactory->makeEntity(m_SceneManager->getRootSceneNode());
	m_Scene->addEntity(e);

	IAnimatedMesh* mesh = m_SceneManager->getMesh("data\\assets\\models\\ED-209\\ED-209.obj");
	IMesh* tangentMesh = m_SceneManager->getMeshManipulator()->createMeshWithTangents(mesh);
	IMeshSceneNode *node = m_SceneManager->addMeshSceneNode(tangentMesh, e.get());

	mesh->drop();
	tangentMesh->drop();


	//IMesh* tangentMesh = m_SceneManager->getMeshManipulator()->createMeshWithTangents(m_SceneManager->addSphereMesh("sphere", 1.0));
	//IMeshSceneNode *node = m_SceneManager->addMeshSceneNode(tangentMesh, e.get());
	//e->setPosition(vector3df(0,3,0));

	//mesh->drop();
	//tangentMesh->drop();

	node->setMaterialType(matLib["d_solid_normal"]);
	node->getMaterial(0).SpecularColor = SColor(0, 20, 0, 0);
	node->getMaterial(0).Shininess = 50.0f;
	node->setMaterialTexture(1, m_VideoDriver->getTexture("data\\assets\\models\\ED-209\\ED209_N.tga"));
	//node->setMaterialTexture(0, m_VideoDriver->getTexture("data\\assets\\textures\\white.bmp"));
	//node->setMaterialTexture(1, m_VideoDriver->getTexture("data\\assets\\textures\\blue.bmp"));

	e->setRotation(vector3df(0, 135, 0));

	// Add some point lights circling around
	for (int i = 0; i < 1; i++)
	{
		ILightSceneNode *light = m_SceneManager->addLightSceneNode(m_SceneManager->getRootSceneNode(), ZERO_VECTOR, SColorf(1.0f, 1.0f, 1.0f), 1.0f);
		light->setLightType(ELT_POINT);

		ISceneNodeAnimator *animator = m_SceneManager->createFlyCircleAnimator(vector3df(0,3,0), 3.0f, 0.001f); // Leaked, should be drop()'d but too lazy
		light->addAnimator(animator);
	}
}

void ContactScreen::createShaderTestScene3(void)
{
	const MaterialLibrary &matLib = m_RenderPipeline->getMaterialLibrary();

	createPlaneBackdrop();

	ICameraSceneNode* camera = m_SceneManager->addCameraSceneNodeFPS();
	camera->setPosition(vector3df(0, 3, -6));
	camera->setTarget(vector3df(0, 3, 0));
	ISceneNodeAnimator *fpsAnimator = *(camera->getAnimators().begin());
	((ISceneNodeAnimatorCameraFPS*)fpsAnimator)->setMoveSpeed(2.0f/1000.0f);

	Entity::Ptr e;

	e = m_EntFactory->makeEntity(m_SceneManager->getRootSceneNode());
	m_Scene->addEntity(e);

	IAnimatedMesh* mesh = m_SceneManager->getMesh("data\\assets\\models\\ED-209\\ED-209.obj");
	IMesh* tangentMesh = m_SceneManager->getMeshManipulator()->createMeshWithTangents(mesh);
	IMeshSceneNode *node = m_SceneManager->addMeshSceneNode(tangentMesh, e.get());

	mesh->drop();
	tangentMesh->drop();

	node->setMaterialType(matLib["d_solid_normal_spec"]);
	//node->setMaterialType(matLib["null"]);
	node->getMaterial(0).SpecularColor = SColor(0, 255, 0, 0);
	node->getMaterial(0).Shininess = 50.0f;
	node->setMaterialTexture(1, m_VideoDriver->getTexture("data\\assets\\models\\ED-209\\ED209_N.tga"));
	node->setMaterialTexture(2, m_VideoDriver->getTexture("data\\assets\\models\\ED-209\\ED209_S.tga"));

	e->setRotation(vector3df(0, 135, 0));

	// Add some point lights circling around
	for (int i = 0; i < 1; i++)
	{
		ILightSceneNode *light = m_SceneManager->addLightSceneNode(m_SceneManager->getRootSceneNode(), ZERO_VECTOR, SColorf(1.0f, 1.0f, 1.0f), 1.0f);
		light->setLightType(ELT_POINT);

		ISceneNodeAnimator *animator = m_SceneManager->createFlyCircleAnimator(vector3df(0,3,0), 3.0f, 0.001f); // Leaked, should be drop()'d but too lazy
		light->addAnimator(animator);
	}
}

void ContactScreen::createShaderTestScene4(void)
{
	const MaterialLibrary &matLib = m_RenderPipeline->getMaterialLibrary();

	createPlaneBackdrop();

	ICameraSceneNode* camera = m_SceneManager->addCameraSceneNodeFPS();
	camera->setPosition(vector3df(0, 3, -1.5));
	camera->setTarget(vector3df(0, 3, 0));
	ISceneNodeAnimator *fpsAnimator = *(camera->getAnimators().begin());
	((ISceneNodeAnimatorCameraFPS*)fpsAnimator)->setMoveSpeed(2.0f/1000.0f);
	camera->setNearValue(0.05f);
	camera->setFarValue(100.0f);

	Entity::Ptr e;

	std::string zombieFiles[] = {"data\\assets\\models\\zombie\\walk_scaled.obj",
		"data\\assets\\models\\zombie\\fury_scaled.obj",
		"data\\assets\\models\\zombie\\dead_scaled.obj"};

	vector3df zombiePos[] = {vector3df(0,2,0), vector3df(1,2,0), vector3df(-1,2,0)};
	vector3df zombieRot[] = {vector3df(0,120,0), vector3df(0,145,0), vector3df(0,60,0)};

	for (int i = 0; i < sizeof(zombieFiles)/sizeof(zombieFiles[0]); i++)
	{
		e = m_EntFactory->makeEntity(m_SceneManager->getRootSceneNode());
		m_Scene->addEntity(e);

		IAnimatedMesh* mesh = m_SceneManager->getMesh(zombieFiles[i].c_str());
		IMesh* tangentMesh = m_SceneManager->getMeshManipulator()->createMeshWithTangents(mesh);
		IMeshSceneNode *node = m_SceneManager->addMeshSceneNode(tangentMesh, e.get());

		mesh->drop();
		tangentMesh->drop();

		node->setMaterialType(matLib["d_solid_normal_spec"]);
		node->getMaterial(0).SpecularColor = SColor(0, 120, 0, 0);
		node->getMaterial(0).Shininess = 100.0f;
		node->setMaterialTexture(0, m_VideoDriver->getTexture("data\\assets\\models\\zombie\\difusse.jpg"));
		node->setMaterialTexture(1, m_VideoDriver->getTexture("data\\assets\\models\\zombie\\normal.jpg"));
		node->setMaterialTexture(2, m_VideoDriver->getTexture("data\\assets\\models\\zombie\\especular.jpg"));

		e->setPosition(zombiePos[i]);
		e->setRotation(zombieRot[i]);
	}

	// Add some point lights circling around
	for (int i = 0; i < 1; i++)
	{
		ILightSceneNode *light = m_SceneManager->addLightSceneNode(m_SceneManager->getRootSceneNode(), ZERO_VECTOR, SColorf(1.0f, 1.0f, 1.0f), 1.0f);
		light->setLightType(ELT_POINT);

		ISceneNodeAnimator *animator = m_SceneManager->createFlyCircleAnimator(vector3df(0,3,0), 3.0f, 0.001f); // Leaked, should be drop()'d but too lazy
		light->addAnimator(animator);
	}
}

void ContactScreen::createShaderTestScene5(void)
{
	const MaterialLibrary &matLib = m_RenderPipeline->getMaterialLibrary();

	createPlaneBackdrop();
	//createOriginMarker();

	ICameraSceneNode* camera = m_SceneManager->addCameraSceneNodeFPS();
	//camera->setPosition(vector3df(0, 3, -6));
	//camera->setTarget(vector3df(0, 3, 0));
	camera->setPosition(vector3df(-3.0f, 3.0f, 0));
	camera->setTarget(vector3df(0, 1, 1));
	ISceneNodeAnimator *fpsAnimator = *(camera->getAnimators().begin());
	((ISceneNodeAnimatorCameraFPS*)fpsAnimator)->setMoveSpeed(2.0f/1000.0f);

	camera->setNearValue(0.05f);
	camera->setFarValue(100.0f);
	//PRINT("Camera original FOV: " << camera->getFOV() << endl, YELLOW);

	Entity::Ptr e;

	e = m_EntFactory->makeEntity(m_SceneManager->getRootSceneNode());
	m_Scene->addEntity(e);

	ISceneNode* node = m_SceneManager->addSphereSceneNode(1.0, 16, e.get());
	node->setMaterialTexture(0, m_VideoDriver->getTexture("data\\assets\\textures\\blue.bmp"));
	node->setMaterialType(matLib["d_solid"]);
	node->getMaterial(0).SpecularColor = SColor(0, 120, 0, 0);
	node->getMaterial(0).Shininess = 100.0f;

	e->setPosition(vector3df(0, 2, 2));

	node = m_SceneManager->getSceneNodeFromName("plane_backdrop");
	node->setMaterialType(matLib["d_solid"]);
	node->setMaterialTexture(0, m_VideoDriver->getTexture("data\\assets\\textures\\yellow.bmp"));
	node->getMaterial(0).SpecularColor = SColor(0, 20, 0, 0);
	node->getMaterial(0).Shininess = 100.0f;
	//node->setPosition(vector3df(0, -2, 0));

	//node = m_SceneManager->addCubeSceneNode(1.0, NULL, -1, vector3df(-1.0f, 0.5f, 1.5f));
	IAnimatedMesh* mesh = m_SceneManager->getMesh("data\\assets\\models\\simple\\cube.obj");
    node = m_SceneManager->addAnimatedMeshSceneNode(mesh);
	node->setPosition(vector3df(-1.0f, 0.5f, 1.5f));
	node->setMaterialType(matLib["d_solid"]);
	node->setMaterialTexture(0, m_VideoDriver->getTexture("data\\assets\\textures\\white.bmp"));
	node->getMaterial(0).SpecularColor = SColor(0, 120, 0, 0);
	node->getMaterial(0).Shininess = 50.0f;

	//node = m_SceneManager->addCubeSceneNode(1.0, NULL, -1, vector3df(-0.8f, 0.5f, 2.5f));
	node = m_SceneManager->addAnimatedMeshSceneNode(mesh);
	node->setPosition(vector3df(-0.8f, 0.5f, 2.5f));
	node->setMaterialType(matLib["d_solid"]);
	node->setMaterialTexture(0, m_VideoDriver->getTexture("data\\assets\\textures\\white.bmp"));
	node->getMaterial(0).SpecularColor = SColor(0, 120, 0, 0);
	node->getMaterial(0).Shininess = 50.0f;

	//node = m_SceneManager->getSceneNodeFromName("origin_marker_x");
	//node->setMaterialType(matLib["d_solid"]);
	//node->setMaterialTexture(0, m_VideoDriver->getTexture("data\\assets\\textures\\white.bmp"));
	//node->getMaterial(0).SpecularColor = SColor(0, 20, 0, 0);
	//node->getMaterial(0).Shininess = 100.0f;

	//node = m_SceneManager->getSceneNodeFromName("origin_marker_y");
	//node->setMaterialType(matLib["d_solid"]);
	//node->setMaterialTexture(0, m_VideoDriver->getTexture("data\\assets\\textures\\white.bmp"));
	//node->getMaterial(0).SpecularColor = SColor(0, 20, 0, 0);
	//node->getMaterial(0).Shininess = 100.0f;

	//node = m_SceneManager->getSceneNodeFromName("origin_marker_z");
	//node->setMaterialType(matLib["d_solid"]);
	//node->setMaterialTexture(0, m_VideoDriver->getTexture("data\\assets\\textures\\white.bmp"));
	//node->getMaterial(0).SpecularColor = SColor(0, 20, 0, 0);
	//node->getMaterial(0).Shininess = 100.0f;

	// Add some point lights circling around
	for (int i = 0; i < 1; i++)
	{
		ILightSceneNode *light = m_SceneManager->addLightSceneNode(m_SceneManager->getRootSceneNode(), ZERO_VECTOR, SColorf(1.0f, 1.0f, 1.0f), 4.0f);
		light->getLightData().Attenuation = vector3df(1.0f, 0.0f, 0.01f);
		light->setLightType(ELT_POINT);
		light->enableCastShadow();

		ISceneNodeAnimator *animator = m_SceneManager->createFlyCircleAnimator(vector3df(0,3,0), 3.0f, 0.0002f); // Leaked, should be drop()'d but too lazy
		light->addAnimator(animator);
		//light->setPosition(vector3df(0, 0.5f, 0.1));
		//light->setPosition(vector3df(0, 5, -5));
	}

	//ILightSceneNode *light2 = m_SceneManager->addLightSceneNode(m_SceneManager->getRootSceneNode(), ZERO_VECTOR, SColorf(1.0f, 1.0f, 1.0f), 4.0f);
	//light2->setLightType(ELT_DIRECTIONAL);
	//light2->setRotation(vector3df(45, 0, 0));
	//light2->getLightData().AmbientColor = SColorf(0.2f, 0.2f, 0.2f);
	//light2->enableCastShadow();
}

void ContactScreen::createShaderTestScene6(void)
{
	const MaterialLibrary &matLib = m_RenderPipeline->getMaterialLibrary();

	createPlaneBackdrop();

	ICameraSceneNode* camera = m_SceneManager->addCameraSceneNodeFPS();
	camera->setPosition(vector3df(0, 3, -6));
	camera->setTarget(vector3df(0, 3, 0));
	ISceneNodeAnimator *fpsAnimator = *(camera->getAnimators().begin());
	((ISceneNodeAnimatorCameraFPS*)fpsAnimator)->setMoveSpeed(2.0f/1000.0f);

	Entity::Ptr e;

	e = m_EntFactory->makeEntity(m_SceneManager->getRootSceneNode());
	m_Scene->addEntity(e);

	IAnimatedMesh* mesh = m_SceneManager->getMesh("data\\assets\\models\\ED-209\\ED-209.obj");
	IMesh* tangentMesh = m_SceneManager->getMeshManipulator()->createMeshWithTangents(mesh);
	ISceneNode *node = m_SceneManager->addMeshSceneNode(tangentMesh, e.get());

	mesh->drop();
	tangentMesh->drop();

	node->setMaterialType(matLib["d_solid_normal_spec"]);
	//node->setMaterialType(matLib["d_solid_normal"]);
	//node->setMaterialType(matLib["d_solid"]);
	node->getMaterial(0).SpecularColor = SColor(0, 255, 0, 0);
	node->getMaterial(0).Shininess = 50.0f;
	node->setMaterialTexture(1, m_VideoDriver->getTexture("data\\assets\\models\\ED-209\\ED209_N.tga"));
	node->setMaterialTexture(2, m_VideoDriver->getTexture("data\\assets\\models\\ED-209\\ED209_S.tga"));

	e->setRotation(vector3df(0, 135, 0));

	node = m_SceneManager->getSceneNodeFromName("plane_backdrop");
	node->setMaterialType(matLib["d_solid"]);
	node->setMaterialTexture(0, m_VideoDriver->getTexture("data\\assets\\textures\\white.bmp"));
	node->getMaterial(0).SpecularColor = SColor(0, 20, 0, 0);
	node->getMaterial(0).Shininess = 100.0f;

	// Add some point lights circling around
	for (int i = 0; i < 1; i++)
	{
		ILightSceneNode *light = m_SceneManager->addLightSceneNode(m_SceneManager->getRootSceneNode(), ZERO_VECTOR, SColorf(1.0f, 1.0f, 1.0f), 4.0f);
		light->getLightData().Attenuation = vector3df(1.0f, 0.0f, 0.01f);
		light->setLightType(ELT_POINT);
		light->enableCastShadow(true);

		ISceneNodeAnimator *animator = m_SceneManager->createFlyCircleAnimator(vector3df(0,5,0), 5.0f, 0.0002f); // Leaked, should be drop()'d but too lazy
		light->addAnimator(animator);
	}
}

void ContactScreen::createShaderTestScene7(void)
{
	const MaterialLibrary &matLib = m_RenderPipeline->getMaterialLibrary();

	createPlaneBackdrop();

	ICameraSceneNode* camera = m_SceneManager->addCameraSceneNodeFPS();
	//camera->setPosition(vector3df(0, 3, -6));
	//camera->setTarget(vector3df(0, 3, 0));
	camera->setPosition(vector3df(0, 3.0f, 0));
	//camera->setTarget(vector3df(2, 2.9f, 2));
	camera->setTarget(vector3df(0, 2.9f, 1));
	ISceneNodeAnimator *fpsAnimator = *(camera->getAnimators().begin());
	((ISceneNodeAnimatorCameraFPS*)fpsAnimator)->setMoveSpeed(2.0f/1000.0f);

	camera->setNearValue(0.05f);
	camera->setFarValue(100.0f);
	//PRINT("Camera original FOV: " << camera->getFOV() << endl, YELLOW);

	ISceneNode *node = m_SceneManager->getSceneNodeFromName("plane_backdrop");
	node->setMaterialType(matLib["d_solid"]);
	node->setMaterialTexture(0, m_VideoDriver->getTexture("data\\assets\\textures\\yellow.bmp"));
	node->getMaterial(0).SpecularColor = SColor(0, 20, 0, 0);
	node->getMaterial(0).Shininess = 100.0f;
	//node->setPosition(vector3df(0, -2, 0));

	float gridWidth = 100.0f;
	float gridHeight = 100.0f;
	int gridSizeX = 25;
	int gridSizeY = 25;

	for (int i = 0; i < gridSizeX; i++)
	{
		for (int j = 0; j < gridSizeY; j++)
		{
			IAnimatedMesh* mesh = m_SceneManager->getMesh("data\\assets\\models\\simple\\cube.obj");
			node = m_SceneManager->addAnimatedMeshSceneNode(mesh);
			node->setPosition(vector3df((gridWidth / (gridSizeX-1)) * i - gridWidth/2, 0.5f,
				(gridHeight / (gridSizeY-1)) * j - gridHeight/2));
			node->setMaterialType(matLib["d_solid"]);
			node->setMaterialTexture(0, m_VideoDriver->getTexture("data\\assets\\textures\\white.bmp"));
			node->getMaterial(0).SpecularColor = SColor(0, 120, 0, 0);
			node->getMaterial(0).Shininess = 50.0f;
		}
	}
	
	//IAnimatedMesh* mesh = m_SceneManager->getMesh("data\\assets\\models\\simple\\cube.obj");
	//node = m_SceneManager->addAnimatedMeshSceneNode(mesh);
	//node->setPosition(vector3df(4.17f, 0.5f, 0));
	//node->setMaterialType(matLib["d_solid"]);
	//node->setMaterialTexture(0, m_VideoDriver->getTexture("data\\assets\\textures\\white.bmp"));
	//node->getMaterial(0).SpecularColor = SColor(0, 120, 0, 0);
	//node->getMaterial(0).Shininess = 50.0f;

	//node = m_SceneManager->addCubeSceneNode(1.0, NULL, -1);
	//node->setMaterialType(matLib["d_solid"]);
	//node->setMaterialTexture(0, m_VideoDriver->getTexture("data\\assets\\textures\\white.bmp"));
	//node->getMaterial(0).SpecularColor = SColor(0, 120, 0, 0);
	//node->getMaterial(0).Shininess = 50.0f;

	ILightSceneNode *light = m_SceneManager->addLightSceneNode(m_SceneManager->getRootSceneNode(), ZERO_VECTOR, SColorf(1.0f, 1.0f, 1.0f), 4.0f);
	light->setLightType(ELT_DIRECTIONAL);
	light->setRotation(vector3df(45, 30, 0));
	light->getLightData().AmbientColor = SColorf(0.2f, 0.2f, 0.2f);
	light->enableCastShadow();
}

void ContactScreen::createHDRTestScene(void)
{
	const MaterialLibrary &matLib = m_RenderPipeline->getMaterialLibrary();

	ICameraSceneNode* camera = m_SceneManager->addCameraSceneNodeFPS();
	camera->setPosition(vector3df(0, 0, 0));
	camera->setTarget(vector3df(0, 0, 1));
	ISceneNodeAnimator *fpsAnimator = *(camera->getAnimators().begin());
	((ISceneNodeAnimatorCameraFPS*)fpsAnimator)->setMoveSpeed(1.0f/1000.0f);
	camera->setNearValue(0.05f);
	camera->setFarValue(100.0f);

	Entity::Ptr e;

	e = m_EntFactory->makeEntity(m_SceneManager->getRootSceneNode());
	m_Scene->addEntity(e);

	ISceneNode *node;
	IAnimatedMesh* mesh;
	IMesh* tangentMesh;
	
	mesh = m_SceneManager->getMesh("data\\assets\\models\\Ruins\\Ruins_nograss.obj");
	tangentMesh = m_SceneManager->getMeshManipulator()->createMeshWithTangents(mesh);
	node = m_SceneManager->addMeshSceneNode(tangentMesh, e.get());

	mesh->drop();
	tangentMesh->drop();

	node->setMaterialType(matLib["d_solid"]);

	for (int i = 0; i < node->getMaterialCount(); i++) {
		SMaterial& mat = node->getMaterial(i);

		mat.SpecularColor = SColor(0, 128, 0, 0);
		node->getMaterial(i).Shininess = 200.0f;

		ITexture* &tDiff = mat.TextureLayer[0].Texture;
		ITexture* &tNorm = mat.TextureLayer[1].Texture;
		ITexture* &tSpec = mat.TextureLayer[2].Texture;

		if (tDiff == m_VideoDriver->getTexture("data\\assets\\models\\Ruins\\pasbrick.jpg"))
		{
			mat.SpecularColor = SColor(0, 128, 0, 0);
			node->getMaterial(i).Shininess = 10.0f;
			tNorm = m_VideoDriver->getTexture("data\\assets\\models\\Ruins\\pasbrick-normal.jpg");
			mat.MaterialType = matLib["d_solid_normal"];
		}

		if (tDiff == m_VideoDriver->getTexture("data\\assets\\models\\Ruins\\PASDetai.jpg"))
		{
			mat.SpecularColor = SColor(0, 128, 0, 0);
			node->getMaterial(i).Shininess = 10.0f;
			tNorm = m_VideoDriver->getTexture("data\\assets\\models\\Ruins\\PASDetai-normal.jpg");
			mat.MaterialType = matLib["d_solid_normal"];
		}

		if (tDiff == m_VideoDriver->getTexture("data\\assets\\models\\Ruins\\PAS_Rust.jpg"))
		{
			mat.SpecularColor = SColor(0, 128, 0, 0);
			node->getMaterial(i).Shininess = 100.0f;
			tNorm = m_VideoDriver->getTexture("data\\assets\\models\\Ruins\\PAS_Rust-normal.jpg");
			mat.MaterialType = matLib["d_solid_normal"];
		}

		if (tDiff == m_VideoDriver->getTexture("data\\assets\\models\\Ruins\\PAS_Wood.jpg"))
		{
			mat.SpecularColor = SColor(0, 128, 0, 0);
			node->getMaterial(i).Shininess = 100.0f;
			tNorm = m_VideoDriver->getTexture("data\\assets\\models\\Ruins\\PAS_Wood-normal.jpg");
			tSpec = m_VideoDriver->getTexture("data\\assets\\models\\Ruins\\PAS_Wood-specular.jpg");
			mat.MaterialType = matLib["d_solid_normal_spec"];
		}

		if (tDiff == m_VideoDriver->getTexture("data\\assets\\models\\Ruins\\PAS_Cret.jpg"))
		{
			mat.SpecularColor = SColor(0, 128, 0, 0);
			node->getMaterial(i).Shininess = 15.0f;
			tNorm = m_VideoDriver->getTexture("data\\assets\\models\\Ruins\\PAS_Cret-normal.jpg");
			mat.MaterialType = matLib["d_solid_normal"];
		}
	}

	e = m_EntFactory->makeEntity(m_SceneManager->getRootSceneNode());
	m_Scene->addEntity(e);

	ILightSceneNode *light;

	light = m_SceneManager->addLightSceneNode(m_SceneManager->getRootSceneNode(), vector3df(1.0f, 0.0f, 1.0f), SColorf(2.0f, 2.0f, 2.0f), 1.0f);
	light->setLightType(ELT_POINT);
	light->enableCastShadow(false);
	//light->getLightData().AmbientColor = SColorf(0.2f, 0.2f, 0.2f);

	light = m_SceneManager->addLightSceneNode(m_SceneManager->getRootSceneNode(), vector3df(1.0f, 0.0f, -1.0f), SColorf(1.0f, 1.0f, 0.5f), 1.0f);
	light->setLightType(ELT_POINT);
	light->enableCastShadow(false);

	light = m_SceneManager->addLightSceneNode(m_SceneManager->getRootSceneNode(), vector3df(1.0f, 0.0f, -2.0f), SColorf(0.8f, 0.5f, 0.5f), 1.0f);
	light->setLightType(ELT_POINT);
	light->enableCastShadow(false);

	light = m_SceneManager->addLightSceneNode(m_SceneManager->getRootSceneNode(), vector3df(-1.0f, 0.0f, 1.0f), SColorf(0.2f, 0.2f, 1.0f), 1.0f);
	light->setLightType(ELT_POINT);
	light->enableCastShadow(false);

	light = m_SceneManager->addLightSceneNode(m_SceneManager->getRootSceneNode(), vector3df(-1.0f, 0.0f, 0.0f), SColorf(0.2f, 2.8f, 0.2f), 1.0f);
	light->setLightType(ELT_POINT);
	light->enableCastShadow(false);

	light = m_SceneManager->addLightSceneNode(m_SceneManager->getRootSceneNode(), vector3df(-1.0f, 0.0f, -1.0f), SColorf(1.0f, 0.0f, 1.0f), 1.0f);
	light->setLightType(ELT_POINT);
	light->enableCastShadow(false);
}

void ContactScreen::createZombieRuinsScene(void)
{
	const MaterialLibrary &matLib = m_RenderPipeline->getMaterialLibrary();

	ICameraSceneNode* camera = m_SceneManager->addCameraSceneNodeFPS();
	camera->setPosition(vector3df(0, 0, 0));
	camera->setTarget(vector3df(0, 0, 1));
	ISceneNodeAnimator *fpsAnimator = *(camera->getAnimators().begin());
	((ISceneNodeAnimatorCameraFPS*)fpsAnimator)->setMoveSpeed(1.0f/1000.0f);
	camera->setNearValue(0.05f);
	camera->setFarValue(100.0f);

	Entity::Ptr e;

	e = m_EntFactory->makeEntity(m_SceneManager->getRootSceneNode());
	m_Scene->addEntity(e);

	ISceneNode *node;
	IAnimatedMesh* mesh;
	IMesh* tangentMesh;
	
	mesh = m_SceneManager->getMesh("data\\assets\\models\\Ruins\\Ruins_nograss.obj");
	tangentMesh = m_SceneManager->getMeshManipulator()->createMeshWithTangents(mesh);
	node = m_SceneManager->addMeshSceneNode(tangentMesh, e.get());

	mesh->drop();
	tangentMesh->drop();

	node->setMaterialType(matLib["d_solid"]);

	for (int i = 0; i < node->getMaterialCount(); i++) {
		SMaterial& mat = node->getMaterial(i);

		mat.SpecularColor = SColor(0, 128, 0, 0);
		node->getMaterial(i).Shininess = 200.0f;

		ITexture* &tDiff = mat.TextureLayer[0].Texture;
		ITexture* &tNorm = mat.TextureLayer[1].Texture;
		ITexture* &tSpec = mat.TextureLayer[2].Texture;

		if (tDiff == m_VideoDriver->getTexture("data\\assets\\models\\Ruins\\pasbrick.jpg"))
		{
			mat.SpecularColor = SColor(0, 128, 0, 0);
			node->getMaterial(i).Shininess = 10.0f;
			tNorm = m_VideoDriver->getTexture("data\\assets\\models\\Ruins\\pasbrick-normal.jpg");
			mat.MaterialType = matLib["d_solid_normal"];
		}

		if (tDiff == m_VideoDriver->getTexture("data\\assets\\models\\Ruins\\PASDetai.jpg"))
		{
			mat.SpecularColor = SColor(0, 128, 0, 0);
			node->getMaterial(i).Shininess = 10.0f;
			tNorm = m_VideoDriver->getTexture("data\\assets\\models\\Ruins\\PASDetai-normal.jpg");
			mat.MaterialType = matLib["d_solid_normal"];
		}

		if (tDiff == m_VideoDriver->getTexture("data\\assets\\models\\Ruins\\PAS_Rust.jpg"))
		{
			mat.SpecularColor = SColor(0, 128, 0, 0);
			node->getMaterial(i).Shininess = 100.0f;
			tNorm = m_VideoDriver->getTexture("data\\assets\\models\\Ruins\\PAS_Rust-normal.jpg");
			mat.MaterialType = matLib["d_solid_normal"];
		}

		if (tDiff == m_VideoDriver->getTexture("data\\assets\\models\\Ruins\\PAS_Wood.jpg"))
		{
			mat.SpecularColor = SColor(0, 128, 0, 0);
			node->getMaterial(i).Shininess = 100.0f;
			tNorm = m_VideoDriver->getTexture("data\\assets\\models\\Ruins\\PAS_Wood-normal.jpg");
			tSpec = m_VideoDriver->getTexture("data\\assets\\models\\Ruins\\PAS_Wood-specular.jpg");
			mat.MaterialType = matLib["d_solid_normal_spec"];
		}

		if (tDiff == m_VideoDriver->getTexture("data\\assets\\models\\Ruins\\PAS_Cret.jpg"))
		{
			mat.SpecularColor = SColor(0, 128, 0, 0);
			node->getMaterial(i).Shininess = 15.0f;
			tNorm = m_VideoDriver->getTexture("data\\assets\\models\\Ruins\\PAS_Cret-normal.jpg");
			mat.MaterialType = matLib["d_solid_normal"];
		}
	}

	e = m_EntFactory->makeEntity(m_SceneManager->getRootSceneNode());
	m_Scene->addEntity(e);

	mesh = m_SceneManager->getMesh("data\\assets\\models\\zombie\\walk_scaled.obj");
	m_SceneManager->getMeshManipulator()->scale(mesh, vector3df(0.65f, 0.65f, 0.65f));
	tangentMesh = m_SceneManager->getMeshManipulator()->createMeshWithTangents(mesh);
	node = m_SceneManager->addMeshSceneNode(tangentMesh, e.get());

	mesh->drop();
	tangentMesh->drop();

	node->setMaterialType(matLib["d_solid_normal_spec"]);
	node->getMaterial(0).SpecularColor = SColor(0, 128, 0, 0);
	node->getMaterial(0).Shininess = 10.0f;
	node->setMaterialTexture(0, m_VideoDriver->getTexture("data\\assets\\models\\zombie\\difusse.jpg"));
	node->setMaterialTexture(1, m_VideoDriver->getTexture("data\\assets\\models\\zombie\\normal.jpg"));
	node->setMaterialTexture(2, m_VideoDriver->getTexture("data\\assets\\models\\zombie\\especular.jpg"));

	e->setPosition(vector3df(1, -1.75f, 2));
	e->setRotation(vector3df(0, 90, 0));

	ILightSceneNode *light;

	//light = m_SceneManager->addLightSceneNode(NULL, vector3df(0, 10.0f, 0), SColorf(0.1f, 0.1f, 0.1f), 10.0f);
	//light->setRotation(vector3df(90, 0, 0));
	//light->getLightData().Attenuation = vector3df(1.0f, 0.0f, 0.01f);
	//light->getLightData().OuterCone = 3 * PI / 8.0f;
	//light->getLightData().InnerCone = PI / 3.0f;
	//light->setLightType(ELT_SPOT);
	//light->enableCastShadow(true);

	//bool flashlight = true;
	//if (flashlight)
	//{
	//	light = m_SceneManager->addLightSceneNode(camera, vector3df(0.2f, -0.3f, 0.05f), SColorf(0.4f, 0.4f, 0.4f), 10.0f);
	//	light->getLightData().Attenuation = vector3df(1.0f, 0.0f, 0.5f);
	//	light->getLightData().AmbientColor = SColorf(0.06f, 0.06f, 0.06f);
	//	light->getLightData().OuterCone = PI / 5.0f;
	//	light->getLightData().InnerCone = PI / 6.0f;
	//	light->setLightType(ELT_SPOT);
	//	light->enableCastShadow(true);
	//}

	light = m_SceneManager->addLightSceneNode(m_SceneManager->getRootSceneNode(), ZERO_VECTOR, SColorf(1.0f, 1.0f, 1.0f), 4.0f);
	light->setLightType(ELT_DIRECTIONAL);
	light->setRotation(vector3df(70, 210, 0));
	light->getLightData().AmbientColor = SColorf(0.2f, 0.2f, 0.2f);
	light->enableCastShadow();
}

void ContactScreen::createScifiRoomScene(void)
{
	const MaterialLibrary &matLib = m_RenderPipeline->getMaterialLibrary();

	ICameraSceneNode* camera = m_SceneManager->addCameraSceneNodeFPS();
	camera->setPosition(vector3df(-1, -0.6f, -4.0f));
	camera->setTarget(vector3df(0, -0.85f, -1.8f));
	ISceneNodeAnimator *fpsAnimator = *(camera->getAnimators().begin());
	((ISceneNodeAnimatorCameraFPS*)fpsAnimator)->setMoveSpeed(1.0f/1000.0f);

	Entity::Ptr e;

	e = m_EntFactory->makeEntity(m_SceneManager->getRootSceneNode());
	m_Scene->addEntity(e);

	IAnimatedMesh* mesh;
	IMesh* tangentMesh;
	ISceneNode *node;

	mesh = m_SceneManager->getMesh("data\\assets\\models\\SciFi_Room\\scifi.obj");
	tangentMesh = m_SceneManager->getMeshManipulator()->createMeshWithTangents(mesh);
	node = m_SceneManager->addMeshSceneNode(tangentMesh, e.get());

	mesh->drop();
	tangentMesh->drop();

	node->setMaterialType(matLib["d_solid"]);
	node->setMaterialFlag(EMF_LIGHTING, true);

	// Floor open plates
	//node->getMaterial(0).TextureLayer[0].Texture = m_VideoDriver->getTexture("data\\assets\\textures\\white.bmp");
	//node->getMaterial(1).TextureLayer[0].Texture = m_VideoDriver->getTexture("data\\assets\\textures\\yellow.bmp");
	//node->getMaterial(2).TextureLayer[0].Texture = m_VideoDriver->getTexture("data\\assets\\textures\\blue.bmp");

	// Plates still over the opened pipe access
	//node->getMaterial(3).TextureLayer[0].Texture = m_VideoDriver->getTexture("data\\assets\\textures\\white.bmp");
	//node->getMaterial(4).TextureLayer[0].Texture = m_VideoDriver->getTexture("data\\assets\\textures\\yellow.bmp");
	//node->getMaterial(5).TextureLayer[0].Texture = m_VideoDriver->getTexture("data\\assets\\textures\\blue.bmp");

	// cables on the ground
	//node->getMaterial(6).TextureLayer[0].Texture = m_VideoDriver->getTexture("data\\assets\\textures\\white.bmp");
	//// weird pedestal by the door
	//node->getMaterial(7).TextureLayer[0].Texture = m_VideoDriver->getTexture("data\\assets\\textures\\yellow.bmp");
	//// top button on the door at the end
	//node->getMaterial(8).TextureLayer[0].Texture = m_VideoDriver->getTexture("data\\assets\\textures\\blue.bmp");

	//// end door lower button
	//node->getMaterial(9).TextureLayer[0].Texture = m_VideoDriver->getTexture("data\\assets\\textures\\blue.bmp");
	//// end door top button ring
	//node->getMaterial(10).TextureLayer[0].Texture = m_VideoDriver->getTexture("data\\assets\\textures\\cyan.bmp");
	//// end door lower button ring
	//node->getMaterial(11).TextureLayer[0].Texture = m_VideoDriver->getTexture("data\\assets\\textures\\green.bmp");
	//// end door button panel
	//node->getMaterial(12).TextureLayer[0].Texture = m_VideoDriver->getTexture("data\\assets\\textures\\red.bmp");
	//// side door button panel
	//node->getMaterial(13).TextureLayer[0].Texture = m_VideoDriver->getTexture("data\\assets\\textures\\white.bmp");
	//// side door lower button
	//node->getMaterial(14).TextureLayer[0].Texture = m_VideoDriver->getTexture("data\\assets\\textures\\yellow.bmp");

	//// side door upper button ring
	//node->getMaterial(15).TextureLayer[0].Texture = m_VideoDriver->getTexture("data\\assets\\textures\\blue.bmp");
	//// side door lower button ring
	//node->getMaterial(16).TextureLayer[0].Texture = m_VideoDriver->getTexture("data\\assets\\textures\\cyan.bmp");
	//// side door upper button
	//node->getMaterial(17).TextureLayer[0].Texture = m_VideoDriver->getTexture("data\\assets\\textures\\green.bmp");
	////
	//node->getMaterial(18).TextureLayer[0].Texture = m_VideoDriver->getTexture("data\\assets\\textures\\red.bmp");
	//// 
	//node->getMaterial(19).TextureLayer[0].Texture = m_VideoDriver->getTexture("data\\assets\\textures\\white.bmp");
	//// 
	//node->getMaterial(20).TextureLayer[0].Texture = m_VideoDriver->getTexture("data\\assets\\textures\\yellow.bmp");

	for (int i = 0; i < node->getMaterialCount(); i++) {
		SMaterial& mat = node->getMaterial(i);

		mat.SpecularColor = SColor(0, 128, 0, 0);
		node->getMaterial(i).Shininess = 200.0f;

		ITexture* &tDiff = mat.TextureLayer[0].Texture;
		ITexture* &tNorm = mat.TextureLayer[1].Texture;
		ITexture* &tSpec = mat.TextureLayer[2].Texture;

		if (tDiff == m_VideoDriver->getTexture("data\\assets\\models\\SciFi_Room\\SCI3.jpg"))
		{
			tDiff = m_VideoDriver->getTexture("data\\assets\\models\\SciFi_Room\\1324-tiled.jpg");
			tNorm = m_VideoDriver->getTexture("data\\assets\\models\\SciFi_Room\\1324-normal-tiled.jpg");
			mat.MaterialType = matLib["d_solid_normal"];
		}

		if (tDiff == m_VideoDriver->getTexture("data\\assets\\models\\SciFi_Room\\SCI1.jpg"))
		{
			//tDiff = m_VideoDriver->getTexture("data\\assets\\models\\SciFi_Room\\8064-diffuse.jpg");
			tNorm = m_VideoDriver->getTexture("data\\assets\\models\\SciFi_Room\\SCI1-normal.jpg");
			mat.MaterialType = matLib["d_solid_normal"];
		}

		if (tDiff == m_VideoDriver->getTexture("data\\assets\\models\\SciFi_Room\\SCI6.jpg"))
		{
			tNorm = m_VideoDriver->getTexture("data\\assets\\models\\SciFi_Room\\SCI6-normal.jpg");
			mat.MaterialType = matLib["d_solid_normal"];
		}

		if (tDiff == m_VideoDriver->getTexture("data\\assets\\models\\SciFi_Room\\Sci9.jpg"))
		{
			mat.SpecularColor = SColor(0, 0, 0, 0);
		}
	}

	e = m_EntFactory->makeEntity(m_SceneManager->getRootSceneNode());
	m_Scene->addEntity(e);

	//mesh = m_SceneManager->getMesh("data\\assets\\models\\ED-209\\ED-209_scaled.obj");
	mesh = m_SceneManager->getMesh("data\\assets\\models\\ED-209\\ED-209.obj");
	m_SceneManager->getMeshManipulator()->scale(mesh, vector3df(0.3f, 0.3f, 0.3f));
	tangentMesh = m_SceneManager->getMeshManipulator()->createMeshWithTangents(mesh);
	node = m_SceneManager->addMeshSceneNode(tangentMesh, e.get());

	mesh->drop();
	tangentMesh->drop();

	node->setMaterialType(matLib["d_solid_normal_spec"]);
	for (int i = 0; i < node->getMaterialCount(); i++) {
		node->getMaterial(i).SpecularColor = SColor(0, 255, 0, 0);
		node->getMaterial(i).Shininess = 50.0f;
	}
	node->setMaterialTexture(1, m_VideoDriver->getTexture("data\\assets\\models\\ED-209\\ED209_N.tga"));
	node->setMaterialTexture(2, m_VideoDriver->getTexture("data\\assets\\models\\ED-209\\ED209_S.tga"));

	e->setRotation(vector3df(0, 175, 0));
	e->setPosition(vector3df(0, -1.7f, -1.7f));


	ILightSceneNode *light;

	//light = m_SceneManager->addLightSceneNode(NULL, vector3df(0, 5.0f, 0), SColorf(0.3f, 0.3f, 0.3f), 4.0f);
	//light->getLightData().Attenuation = vector3df(1.0f, 0.0f, 0.01f);
	//light->setLightType(ELT_POINT);
	//light->enableCastShadow(false);

	// Set up a flashing red light at the back of the room
	light = m_SceneManager->addLightSceneNode(NULL, vector3df(0.7f, 1, 1), SColorf(1, 0, 0), 10.0f);
	light->setRotation(vector3df(90, 0, 0));
	light->getLightData().Attenuation = vector3df(1.0f, 0.0f, 0.05f);
	light->getLightData().OuterCone = 3 * PI / 8.0f;
	light->getLightData().InnerCone = PI / 3.0f;
	light->setLightType(ELT_SPOT);
	light->enableCastShadow(true);

	m_FlashingLight = light;
	m_FlashingLightColor = light->getLightData().DiffuseColor;
	m_CurrentMillis = 0;
	m_PeriodMillis = 2000;
	m_MaxBrightness = 2.0f;
	m_MinBrightness = 0.6f;
	m_AmbientFactor = 0.05f;


	bool flashlight = true;
	if (flashlight)
	{
		light = m_SceneManager->addLightSceneNode(camera, vector3df(0.3f, -0.4f, 0.2f), SColorf(0.6f, 0.6f, 0.6f), 10.0f);
		light->getLightData().Attenuation = vector3df(1.0f, 0.0f, 0.1f);
		light->getLightData().AmbientColor = SColorf(0.06f, 0.06f, 0.06f);
		light->getLightData().OuterCone = PI / 5.0f;
		light->getLightData().InnerCone = PI / 6.0f;
		light->setLightType(ELT_SPOT);
		light->enableCastShadow(true);
	}
	else {
		// A spinning spotlight around the mech
		e = m_EntFactory->makeEntity(m_SceneManager->getRootSceneNode());
		m_Scene->addEntity(e);
		e->setPosition(vector3df(0, -1.7f, -1.5f));

		ISceneNodeAnimator *animator = m_SceneManager->createRotationAnimator(vector3df(0, 0.2f, 0)); // leaked
		e->addAnimator(animator);

		// happy accident, light passes behind the fan grate
		//light = m_SceneManager->addLightSceneNode(e.get(), vector3df(0, 2.0f, -3), SColorf(0.6f, 0.6f, 0.6f), 10.0f);
		light = m_SceneManager->addLightSceneNode(e.get(), vector3df(0, 2.0f, -1.8f), SColorf(1.0f, 1.0f, 1.0f), 10.0f);
		light->setRotation(vector3df(atan(2.0/2.0)*180/PI, 0, 0));
		light->getLightData().Attenuation = vector3df(1.0f, 0.0f, 0.1f);
		light->getLightData().AmbientColor = SColorf(0.06f, 0.06f, 0.06f);
		light->getLightData().OuterCone = PI / 5.0f;
		light->getLightData().InnerCone = PI / 6.0f;
		light->setLightType(ELT_SPOT);
		light->enableCastShadow(true);
		
		//node = m_SceneManager->addSphereSceneNode(0.1f, 16, light, -1);
		//node->setMaterialType(matLib["d_solid"]);
		//node->setMaterialTexture(0,m_VideoDriver->getTexture("data\\assets\\textures\\white.bmp"));
	}
}

void ContactScreen::printSubtreeTypes(ISceneNode *node, int level)
{
	const irr::c8 *name = m_SceneManager->getSceneNodeTypeName(node->getType());
	std::string str(level, ' ');
	PRINT(str << ((name)? name : "UNKNOWN") << endl, WHITE);

	const core::list<ISceneNode*> &children = node->getChildren();
	core::list<ISceneNode*>::ConstIterator it = children.begin();

	for (; it != children.end(); it++)
		printSubtreeTypes(*it, level+1);
}
