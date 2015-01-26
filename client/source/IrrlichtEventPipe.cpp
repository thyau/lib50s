/*
	IrrlichtEventPipe.cpp
	Copyright (c) [ORGNAME] 2013
	Author: Jon Schiavo
*/

#include "..\\include\\IrrlichtEventPipe.hpp"

IrrlichtEventPipe::IrrlichtEventPipe() :
	m_Device(NULL)
{
}

IrrlichtEventPipe::~IrrlichtEventPipe()
{
}

void IrrlichtEventPipe::setPointers(const EVENT_PIPE_POINTERS &pointers)
{
	m_Device = pointers.device;
}

bool IrrlichtEventPipe::OnEvent(const SEvent& e)
{
	// TODO: Fill this method out
	// Any events that we need to watch should be parsed here.
	// See the tutorial here for more info: http://irrlicht.sourceforge.net/docu/example004.html

	return true;
}