#ifndef __COMMON_HPP__
#define __COMMON_HPP__

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Jon Schiavo
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Contains common definitions used throughout other source files.
 */

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

// These definitions require "vector3d.h"
#define ZERO_VECTOR irr::core::vector3df(0,0,0)
#define ONES_VECTOR irr::core::vector3df(1,1,1)
#define UNIT_X irr::core::vector3df(1,0,0)
#define UNIT_Y irr::core::vector3df(0,1,0)
#define UNIT_Z irr::core::vector3df(0,0,1)

// These definitions require "btBulletCollisionCommon.h"
#define BT_ZERO_VECTOR btVector3(0,0,0)
#define BT_ONES_VECTOR btVector3(1,1,1)
#define BT_UNIT_X btVector3(1,0,0)
#define BT_UNIT_Y btVector3(0,1,0)
#define BT_UNIT_Z btVector3(0,0,1)

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

#ifndef PRINT_BTVECTOR
#define PRINT_BTVECTOR(v) { std::cout<<"VECTOR: <"<<v.x()<<", "<<v.y()<<", "<<v.z()<<">"<<std::endl; }
#endif

#endif