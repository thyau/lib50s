/*
	Main.cpp
	Copyright (c) [ORGNAME] 2013
	Author: Jon Schiavo
*/

#include <iostream>

#include "Common.hpp"
#include "CrashDumper.hpp"
#include "Error.hpp"
#include "WindowsConsole.hpp"

#include "..\\include\\Client.hpp"

using namespace std;

bool init_controller(Client &Control)
{
	try
	{
		if(!Control.init())
			THROW("::main: Controller initialization failed");
	}
	catch(...)
	{
		// ;_;
		// Something has gone horribly wrong.  Pause here so we can read the output
		CONSOLE_COLOR(RED);
		cout<<"FATAL ERROR: Could not initialize controller"<<endl;
		cout<<"Press ENTER to continue"<<endl;
		cin.ignore();

		return false;
	}

	return true;
}

int main(int argc, char **argv)
{
	int retcode = -1;

	// Set initial console color and maximize
	CONSOLE_COLOR(WHITE);
	ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);

	// Create the controller and give it the command line args
	Client Control;
	Control.setArgs(argc, argv);

	// Initialize the controller.  This will startup the Irrlicht engine
	// and import our resources
	if(!init_controller(Control))
		return retcode;

	// Start the main loop
	retcode = Control.go();

	PRINT("Control returned to main.  Controller exited with code: "<<retcode<<endl, YELLOW);

	// Shut down the controller.  This will shut down the engine and free
	// our resources
	if(!Control.shutdown())
		cout<<"Shutdown FAILED: Non-fatal error"<<endl;
	else
		cout<<"Shutdown completed"<<endl;

	// We want to pause and keep the console open after the program exits
	cout<<"ENTER to continue";
	cin.ignore();

	return retcode;
}