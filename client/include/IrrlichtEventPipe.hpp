#ifndef __IRRLICHTEVENTPIPE_HPP__
#define __IRRLICHTEVENTPIPE_HPP__

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Jon Schiavo
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * A pipe through which Irrlicht pumps messages, and we handle them
 */

#include <Common.hpp>

/*
 * STRUCTURE: EVENT_PIPE_POINTERS
 * Used for passing a group of pointers the event pipe will need to do its job.
 * Add any pointers here that the class will need inside of OnEvent, and then
 * copy them into the class in the constructor
 */
typedef struct EVENT_PIPE_POINTERS
{
	IrrlichtDevice* device;
} EVENT_PIPE_POINTERS;

/******************************************************
* Class  : IrrlichtEventPipe
* Purpose: Implementation of IEventReceiver
* Notes  :
******************************************************/
class IrrlichtEventPipe : public IEventReceiver
{
public:
						IrrlichtEventPipe			();
						~IrrlichtEventPipe			();

	// Public Members
	void				setPointers					(const EVENT_PIPE_POINTERS &pointers);

	// Implemented from IEventReceiver
	virtual bool		OnEvent						(const SEvent& e);

private:
	// Private Members
	IrrlichtDevice*		m_Device;
};

#endif