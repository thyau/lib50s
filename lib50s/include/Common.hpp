#ifndef __COMMON_HPP__
#define __COMMON_HPP__

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Jon Schiavo
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Contains common includes and macro definitions used throughout other source files.
 */

//======================================
// Includes
//======================================
// System API
#include <windows.h>

// I/O
#include <stdio.h>
#include <iostream>
 
// STL
#include <vector>
#include <map>
#include <queue>
#include <list>
#include <stack>
#include <set>
#include <memory>

// Irrlicht
#include <irrlicht.h>

// Bullet
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

// Misc
#include <assert.h>
#include "..\\include\\Error.hpp"

//======================================
// Enumerators
//======================================
enum RESULT
{
	R_OK			= 0x1,
	R_E_FAIL		= 0x80000001,
	R_E_NOTIMPL		= 0x80000002,
	R_E_INVALIDARG	= 0x80000003,
	R_E_OUTOFMEMORY	= 0x80000004,
	R_E_POINTER		= 0x80000005,
	R_E_UNEXPECTED	= 0x80000006,

	R_FORCE_32BIT	= 0xFFFFFFFF
};

//======================================
// Utility Definitions
//======================================
#define STUB { /* Empty */ }

#define ZERO_VECTOR vector3df(0,0,0)
#define ONES_VECTOR vector3df(1,1,1)
#define UNIT_X vector3df(1,0,0)
#define UNIT_Y vector3df(0,1,0)
#define UNIT_Z vector3df(0,0,1)

#define BT_ZERO_VECTOR btVector3(0,0,0)
#define BT_ONES_VECTOR btVector3(1,1,1)
#define BT_UNIT_X btVector3(1,0,0)
#define BT_UNIT_Y btVector3(0,1,0)
#define BT_UNIT_Z btVector3(0,0,1)

inline btVector3 btVec(const irr::core::vector3d<btScalar> &v) { return btVector3(v.X, v.Y, v.Z); }
inline irr::core::vector3d<btScalar> irrVec(const btVector3 &v) { return irr::core::vector3d<btScalar>(v[0], v[1], v[2]); }

inline void toBtTransform(const irr::core::matrix4 &irrTrans, btTransform &btTrans)
{
	// TODO: Need to test this to make sure it's correct
	btTrans.setOrigin(btVector3(irrTrans(3,0), irrTrans(3,1), irrTrans(3,2)));
	// Irrlicht uses D3D-style 4x4 matrix and multiplying with vector on the left
	// bullet multiplies with vector on the right
	btTrans.setBasis(btMatrix3x3(irrTrans(0,0), irrTrans(1,0), irrTrans(2,0), 
								 irrTrans(0,1), irrTrans(1,1), irrTrans(2,1), 
								 irrTrans(0,2), irrTrans(1,2), irrTrans(2,2)));
}

inline void toIrrTransform(const btTransform &btTrans, irr::core::matrix4 &irrTrans)
{
	// TODO: Need to test this to make sure it's correct
	irrTrans.setTranslation(irrVec(btTrans.getOrigin()));

	const btMatrix3x3 &btBasis = btTrans.getBasis();
	irrTrans(0,0) = btBasis[0][0];
	irrTrans(1,0) = btBasis[0][1];
	irrTrans(2,0) = btBasis[0][2];
	irrTrans(0,1) = btBasis[1][0];
	irrTrans(1,1) = btBasis[1][1];
	irrTrans(2,1) = btBasis[1][2];
	irrTrans(0,2) = btBasis[2][0];
	irrTrans(1,2) = btBasis[2][1];
	irrTrans(2,2) = btBasis[2][2];
}

inline bool btTransformEqual(const btTransform &t1, const btTransform &t2, btScalar tolerance)
{
	const btMatrix3x3 &b1 = t1.getBasis(), &b2 = t2.getBasis();
	const btVector3 &v1 = t1.getOrigin(), &v2 = t2.getOrigin();

	return (abs(b1[0][0] - b2[0][0]) < tolerance) && 
		   (abs(b1[0][1] - b2[0][1]) < tolerance) && 
		   (abs(b1[0][2] - b2[0][2]) < tolerance) && 
		   (abs(b1[1][0] - b2[1][0]) < tolerance) && 
		   (abs(b1[1][1] - b2[1][1]) < tolerance) && 
		   (abs(b1[1][2] - b2[1][2]) < tolerance) && 
		   (abs(b1[2][0] - b2[2][0]) < tolerance) && 
		   (abs(b1[2][1] - b2[2][1]) < tolerance) && 
		   (abs(b1[2][2] - b2[2][2]) < tolerance) && 
		   (abs(v1.x() - v2.x()) < tolerance) &&
		   (abs(v1.y() - v2.y()) < tolerance) &&
		   (abs(v1.z() - v2.z()) < tolerance);
}

//Colors for console text
#define WHITE		FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY
#define RED			FOREGROUND_RED|FOREGROUND_INTENSITY
#define ORANGE		FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY
#define GREEN		FOREGROUND_GREEN|FOREGROUND_INTENSITY
#define BLUE		FOREGROUND_BLUE|FOREGROUND_INTENSITY
#define YELLOW		FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY
#define MAGENTA		FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_BLUE
#define CYAN		FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY

//======================================
// Macro Definitions
//======================================
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(a) { if(a) { delete[] (a); (a) = NULL; } }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) { if(p) { delete p; p = NULL; } }
#endif

#ifndef SAFE_DROP
#define SAFE_DROP(p) { if(p) { p->drop(); p = NULL; } }
#endif

#ifndef SIZE_OF_STATIC_ARRAY
#define SIZE_OF_STATIC_ARRAY(a) sizeof(a) / sizeof(a[0])
#endif

#ifndef DEG_TO_RAD
#define DEG_TO_RAD(x) (x * (PI/180.0f))
#endif

#ifndef RAD_TO_DEG
#define RAD_TO_DEG(x) ((x * 180.0f) / PI)
#endif

#ifndef CONSOLE_COLOR
#define CONSOLE_COLOR(c) { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c); }
#endif

#ifndef PRINT
#define PRINT(stream, color) { CONSOLE_COLOR(color); std::cout<<stream; CONSOLE_COLOR(WHITE); }
#endif

#ifndef PRINT_BTVECTOR
#define PRINT_BTVECTOR(v) { std::cout<<"VECTOR: <"<<v.x()<<", "<<v.y()<<", "<<v.z()<<">"<<std::endl; }
#endif

using namespace std;

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


#endif