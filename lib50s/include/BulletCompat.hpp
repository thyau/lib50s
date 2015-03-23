#ifndef BULLET_COMMPAT_HPP
#define BULLET_COMMPAT_HPP

/*
 * Copyright (c) [ORGNAME] 2015
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Helper functions and definitions for converting between BulletPhysics and Irrlicht types.
 */

#include "vector3d.h"
#include "matrix4.h"
#include "btBulletCollisionCommon.h"

inline btVector3 btVec(const irr::core::vector3d<btScalar> &v) { return btVector3(v.X, v.Y, v.Z); }
inline irr::core::vector3d<btScalar> irrVec(const btVector3 &v) { return irr::core::vector3d<btScalar>(v[0], v[1], v[2]); }

inline void toBtTransform(const irr::core::matrix4 &irrTrans, btTransform &btTrans)
{
	// TODO: Need to test this to make sure it's correct
	btTrans.setOrigin(btVector3(irrTrans(3, 0), irrTrans(3, 1), irrTrans(3, 2)));
	// Irrlicht uses D3D-style 4x4 matrix and multiplying with vector on the left
	// bullet multiplies with vector on the right
	btTrans.setBasis(btMatrix3x3(irrTrans(0, 0), irrTrans(1, 0), irrTrans(2, 0),
		irrTrans(0, 1), irrTrans(1, 1), irrTrans(2, 1),
		irrTrans(0, 2), irrTrans(1, 2), irrTrans(2, 2)));
}

inline void toIrrTransform(const btTransform &btTrans, irr::core::matrix4 &irrTrans)
{
	// TODO: Need to test this to make sure it's correct
	irrTrans.setTranslation(irrVec(btTrans.getOrigin()));

	const btMatrix3x3 &btBasis = btTrans.getBasis();
	irrTrans(0, 0) = btBasis[0][0];
	irrTrans(1, 0) = btBasis[0][1];
	irrTrans(2, 0) = btBasis[0][2];
	irrTrans(0, 1) = btBasis[1][0];
	irrTrans(1, 1) = btBasis[1][1];
	irrTrans(2, 1) = btBasis[1][2];
	irrTrans(0, 2) = btBasis[2][0];
	irrTrans(1, 2) = btBasis[2][1];
	irrTrans(2, 2) = btBasis[2][2];
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

#endif
