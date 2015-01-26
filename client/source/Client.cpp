/*
	Client.cpp
	Copyright (c) [ORGNAME] 2013
	Author: Jon Schiavo
*/

#include <chrono>
#include <thread>

#include "..\\include\\Client.hpp"

#include <Entity.hpp>
#include <BaseComponent.hpp>
#include <ComponentPhysical.hpp>
#include <ComponentMesh.hpp>

Client::Client() :
	m_Started(false),
	m_Device(NULL),
	m_VideoDriver(NULL),
	m_SceneManager(NULL),
	m_GUIEnvironment(NULL),
	m_EventPipe(NULL),
	m_LastTick(0),
	m_MaxFrameRate(10000)
{
}

Client::~Client()
{
	this->shutdown();
}

void Client::setArgs(long argc, char **argv)
{
	if(!argv || m_Started)
		return;

	// TODO: Args to controller passed from command line
}

bool Client::init()
{
	if(m_Started)
		return true;

	// Pretty header
	PRINT("Controller starting up..."<<endl, GREEN);

	// Create the event pipe (all of Irrlichts messages to us will go through here)
	m_EventPipe = new IrrlichtEventPipe();

	// Create the Irrlicht device.  This is our primary interface to the engine
	cout<<"Creating Irrlicht device"<<endl;

	SIrrlichtCreationParameters irrParams;
	irrParams.DriverType = EDT_NULL;

	m_Device = irr::createDeviceEx(irrParams);
	if(!m_Device)
		return false;

	IVideoModeList* vModeList = m_Device->getVideoModeList();

	irrParams.DriverType = EDT_OPENGL;
	irrParams.Fullscreen = true;
	irrParams.WindowSize = dimension2du(1920,1080);//vModeList->getDesktopResolution();
	irrParams.Bits = vModeList->getDesktopDepth();
	irrParams.AntiAlias = 0;//4;
	irrParams.Stencilbuffer = false;//true;
	irrParams.Vsync = false;//true;
	//irrParams.EventReceiver = m_EventPipe;

	m_Device->drop();

	m_Device = irr::createDeviceEx(irrParams);
	if(!m_Device)
		return false;

	//m_Renderer = createRenderer(m_Device, "data\\shaders\\");

	// Getting drivers and managers from device
	m_VideoDriver = m_Device->getVideoDriver();
	m_SceneManager = m_Device->getSceneManager();
	m_DebugSceneManager = m_SceneManager->createNewSceneManager(false);
	m_GUIEnvironment = m_Device->getGUIEnvironment();
	m_Timer = m_Device->getTimer();
	m_FileSystem = m_Device->getFileSystem();

	if(!m_VideoDriver || !m_SceneManager || !m_DebugSceneManager || !m_GUIEnvironment)
		return false;

	//m_Renderer->createPostProcessingEffect(irr::video::EPE_FOG);
	//video::CPostProcessingEffect* aa = m_Renderer->createPostProcessingEffect(irr::video::EPE_ANTIALIASING);
 //   video::CPostProcessingEffectChain* bloom = m_Renderer->createPostProcessingEffectChain();
 //   bloom->createEffect(irr::video::EPE_BLOOM_V); //Vertical bloom
 //   bloom->createEffect(irr::video::EPE_BLOOM_H); //Horizontal bloom

 //   aa->setActive(false);
 //   bloom->setActive(false);
	

	// Once everything is created, pass the relevant points to the event pipe
	EVENT_PIPE_POINTERS epp;
	epp.device = m_Device;
	m_EventPipe->setPointers(epp);

	launchContactScreen();

	m_Started = true;
	return true;
}

long Client::go()
{
	if(!m_Device)
		return -1;

	PRINT("GO"<<endl, YELLOW);

	int lastFPS = -1; 
	chrono::system_clock::duration sleepDuration(0), zeroSec(0), timePerFrame(chrono::system_clock::period::den/m_MaxFrameRate);

	m_Timer->tick(); // Init start time of simulation to be now
	m_LastTick = m_Timer->getTime();
	while(true)
	{
		if(!m_Device->run())
			break;

		if((bool)m_CurrentGame)
		{
			// Update times
			float delta = (m_Timer->getTime() - m_LastTick) * 0.001f;
			m_LastTick = m_Timer->getTime();

			sleepDuration = timePerFrame - 
				(chrono::system_clock::duration((int)ceil(delta * chrono::system_clock::period::den)) - sleepDuration);

			if (m_VideoDriver->getFPS() != lastFPS)
			{
				lastFPS = m_VideoDriver->getFPS();
				m_Device->setWindowCaption((L"50s game (FPS: " + to_wstring(lastFPS) + L")").c_str());
			}

			// TODO: move all of this into the Game classes
			m_VideoDriver->beginScene(true, true, irr::video::SColor(255,100,101,140));

			m_CurrentGame->frame(delta);
			m_CurrentGame->debugFrame(delta);
			m_CurrentGame->updateGUI(delta);

			m_VideoDriver->endScene();

			if (sleepDuration > zeroSec)
			{
				this_thread::sleep_for(sleepDuration);
			}
		}
	}

	return 0;
}

bool Client::shutdown()
{
	if(!m_Started)
		return true;

	m_Started = false;
	return true;
}

RESULT Client::launchContactScreen()
{
	// If we already have a current game, we cannot start a new one
	if((bool)m_CurrentGame)
		return R_E_FAIL;

	GAME_INITIALIZER init;

	init.device = m_Device;
	init.videoDriver = m_VideoDriver;
	init.sceneManager = m_SceneManager;
	init.debugSceneManager = m_DebugSceneManager;
	init.guiEnv = m_GUIEnvironment;
	init.fileSystem = m_FileSystem;

	m_CurrentGame = IGame::Ptr(new ContactScreen(init));
	m_CurrentGame->init();

	//// HACK: Need to call swapMaterials on every new scene node as it's created.
	//irr::video::CMaterialSwapper* swapper = m_Renderer->getMaterialSwapper();
    //swapper->swapMaterials();

	return R_OK;
}

RESULT Client::launchOfflineGame()
{
	return R_E_NOTIMPL;
}

RESULT Client::endCurrentGame()
{
	return R_E_NOTIMPL;
}