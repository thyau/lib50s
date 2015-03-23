/*
	DebugScene.cpp
	Copyright (c) [ORGNAME] 2013
	Author: Timothy Yau
*/

#include "ICameraSceneNode.h"
#include "matrix4.h"

#include "BulletCompat.hpp"
#include "WindowsConsole.hpp"

#include "..\include\DebugScene.hpp"

using namespace std;

using namespace irr;
using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;

DebugScene::DebugScene(ISceneManager *sceneMgr, ISceneManager *debugSceneMgr)
	: m_DrawBoundingBox(false), m_DrawPhysicsShape(false)
{
	PRINT("Scene mgr is: " << hex << sceneMgr << ", debug is " << hex << debugSceneMgr << endl, WHITE);

	sceneMgr->grab();
	m_SceneMgr = sceneMgr;
	debugSceneMgr->grab();
	m_DebugSceneMgr = debugSceneMgr;
	m_GeomCreator = m_DebugSceneMgr->getGeometryCreator();
	m_MeshManipulator = m_DebugSceneMgr->getMeshManipulator();

	addCameraAndLights(m_DebugSceneMgr);
	//m_LightManager = new DebugLightManager();
	//m_DebugSceneMgr->setLightManager(m_LightManager);
}


DebugScene::~DebugScene(void)
{
	for (NODE_TRACKING_INFO &info : trackingInfo)
	{
		info.m_TrackedNode->drop();
	}

	//m_LightManager->drop();
	removeCameraAndLights(m_DebugSceneMgr);

	m_DebugSceneMgr->drop();
	m_SceneMgr->drop();
}

void DebugScene::updateScene()
{
	vector<NODE_TRACKING_INFO>::iterator infoIter = trackingInfo.begin();

	while(infoIter != trackingInfo.end())
	{
		NODE_TRACKING_INFO &info = *infoIter;

		if (info.m_TrackedNode->getReferenceCount() == 1)
		{
			// We are the only reference left to this node, drop it completely
			info.m_TrackedNode->drop();
			trackingInfo.erase(infoIter++);
			continue;
		}

		updateBoundingBoxNode(info);
		updatePhysicsNodes(info);

		infoIter++;
	}

	updateCameraAndLightPos(m_SceneMgr, m_DebugSceneMgr);
}

void DebugScene::trackNode(ISceneNode *node)
{
	for (NODE_TRACKING_INFO &info : trackingInfo)
	{
		if (info.m_TrackedNode == node)
			return; // Don't track the same node multiple times
	}

	node->grab();

	NODE_TRACKING_INFO info;
	info.m_TrackedNode = node;

	trackingInfo.push_back(info);
}

// Track all nodes in the subtree
void DebugScene::trackNodeSubtree(ISceneNode *root)
{
	trackNode(root);

	const core::list<ISceneNode*> &children = root->getChildren();
	core::list<ISceneNode*>::ConstIterator it = children.begin();

	for (; it != children.end(); it++)
		trackNodeSubtree(*it);
}

bool DebugScene::stopTrackingNode(ISceneNode *node)
{
	vector<NODE_TRACKING_INFO>::iterator infoIter = trackingInfo.begin();

	while(infoIter != trackingInfo.end())
	{
		NODE_TRACKING_INFO &info = *infoIter;

		if (info.m_TrackedNode == node)
		{
			info.m_TrackedNode->drop();
			trackingInfo.erase(infoIter);
			return true;
		}

		infoIter++;
	}

	return false;
}

void DebugScene::stopTrackingSubtree(ISceneNode *root)
{
	stopTrackingNode(root);

	const core::list<ISceneNode*> &children = root->getChildren();
	core::list<ISceneNode*>::ConstIterator it = children.begin();

	for (; it != children.end(); it++)
		stopTrackingSubtree(*it);
}

void DebugScene::stopTrackingAll(void)
{
	for (NODE_TRACKING_INFO &info : trackingInfo)
	{
		info.m_TrackedNode->drop();
	}

	trackingInfo.clear();
}

IMeshSceneNode* DebugScene::createNodeForShape(btCollisionShape *shape) const
{
	IMeshSceneNode *sceneNode = NULL;

	// This code mirrors ComponentCollider::SpawnData::createCollisionShape
	switch (shape->getShapeType() /* BroadphaseNativeTypes */)
	{
	case SPHERE_SHAPE_PROXYTYPE:
		{
			btSphereShape *sphereShape = (btSphereShape*)(shape);
			float radius = sphereShape->getRadius() + sphereShape->getMargin();
			sceneNode = m_DebugSceneMgr->addSphereSceneNode(radius, 8, m_DebugSceneMgr->getRootSceneNode());
		}
		break;

	case CYLINDER_SHAPE_PROXYTYPE:
	case CAPSULE_SHAPE_PROXYTYPE:
		{
			float radius = 0, height;
			int upAxis;

			if (shape->getShapeType() == CYLINDER_SHAPE_PROXYTYPE)
			{
				btCylinderShape *cylinderShape = (btCylinderShape*)(shape);
				btVector3 bBox = cylinderShape->getHalfExtentsWithMargin() * 2;
				upAxis = cylinderShape->getUpAxis();

				switch (upAxis)
				{
				case 0: // Cylinder around X axis
					radius = min(bBox.y(), bBox.z()) / 2; height = bBox.x();
					break;

				case 1: // Y axis
					radius = min(bBox.x(), bBox.z()) / 2; height = bBox.y();
					break;

				case 2: // Z axis
					radius = min(bBox.x(), bBox.y()) / 2; height = bBox.z();
					break;

				default:
					PRINT("DebugScene::createNodeForShape: invalid axis for btCylinderShape\n", YELLOW);
				}
			}
			else
			{
				btCapsuleShape *capsuleShape = (btCapsuleShape*)(shape);
				radius = capsuleShape->getRadius();
				height = capsuleShape->getHalfHeight() * 2;
				upAxis = capsuleShape->getUpAxis();
			}

			if (radius != 0)
			{
				// TODO: need to delete memory??
				IMesh *cylinderMesh = m_GeomCreator->createCylinderMesh(radius, height, 4); 
				matrix4 meshMatrix;

				if (upAxis == 0)
				{
					meshMatrix.setRotationDegrees(vector3df(0, 0, 90));
					meshMatrix.setTranslation(vector3df(height/2, 0, 0));
				}
				else if (upAxis == 1)
				{
					meshMatrix.setTranslation(vector3df(0, -height/2, 0));
				}
				else
				{
					meshMatrix.setRotationDegrees(vector3df(90, 0, 0));
					meshMatrix.setTranslation(vector3df(0, 0, -height/2));
				}

				m_MeshManipulator->transform(cylinderMesh, meshMatrix);

				sceneNode = m_DebugSceneMgr->addMeshSceneNode(cylinderMesh, m_DebugSceneMgr->getRootSceneNode());

				if (shape->getShapeType() == CAPSULE_SHAPE_PROXYTYPE)
				{
					// Create two more balls at the end
					IMeshSceneNode *b1 = m_DebugSceneMgr->addSphereSceneNode(radius, 16, sceneNode);
					IMeshSceneNode *b2 = m_DebugSceneMgr->addSphereSceneNode(radius, 16, sceneNode);
					vector3df displacement = ZERO_VECTOR;

					if (upAxis == 0)
						displacement.X = height / 2;
					else if (upAxis == 1)
						displacement.Y = height / 2;
					else
						displacement.Z = height / 2;

					b1->setPosition(displacement);
					b2->setPosition(-displacement);
					setWireframeMatProps(b1);
					setWireframeMatProps(b2);
				}
			}
		}
		break;
	
	case BOX_SHAPE_PROXYTYPE:
		{
			btBoxShape *boxShape = (btBoxShape*)(shape);
			btVector3 halfExtents = boxShape->getHalfExtentsWithMargin();
			sceneNode = m_DebugSceneMgr->addCubeSceneNode(1.0f, m_DebugSceneMgr->getRootSceneNode(), -1, ZERO_VECTOR, ZERO_VECTOR, irrVec(halfExtents * 2));
		}
		break;

	default:
		// TODO: mesh shapes
		PRINT("DebugScene::createNodeForShape: unknown shape type\n", YELLOW);
	}

	if (sceneNode)
		setWireframeMatProps(sceneNode);

	return sceneNode;
}

void DebugScene::setWireframeMatProps(IMeshSceneNode *node) const
{
	node->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
	node->setMaterialFlag(EMF_WIREFRAME, true);
	node->setMaterialFlag(EMF_GOURAUD_SHADING, false);
	node->setMaterialFlag(EMF_LIGHTING, false);
	node->setMaterialFlag(EMF_BACK_FACE_CULLING, false);

	for (unsigned int i = 0; i < node->getMaterialCount(); i++)
	{
		node->getMaterial(i).PolygonOffsetDirection = EPO_FRONT;
		node->getMaterial(i).PolygonOffsetFactor = 1;
	}
}

void DebugScene::setMeshColor(IMeshSceneNode *node, int lightID) const
{
	SColor color;

	switch (lightID)
	{
	case DEBUG_LIGHT_BOUNDING_BOX:
		color = SColor(0, 255, 255, 255);
		break;
	case DEBUG_LIGHT_PHYSICS_SHAPE:
		color = SColor(0, 80, 255, 80);
		break;
	default:
		color = SColor(0, 255, 0, 0);
		break;
	}

	m_MeshManipulator->setVertexColors(node->getMesh()->getMeshBuffer(0), color);
}

void DebugScene::updateBoundingBoxNode(NODE_TRACKING_INFO &info)
{
	if (m_DrawBoundingBox)
	{
		if (!info.m_BoundingBoxNode)
		{
			info.m_BoundingBoxNode = m_DebugSceneMgr->addCubeSceneNode(1, m_DebugSceneMgr->getRootSceneNode(), DEBUG_LIGHT_BOUNDING_BOX, ZERO_VECTOR, ZERO_VECTOR, ONES_VECTOR);
			setWireframeMatProps(info.m_BoundingBoxNode);
			setMeshColor(info.m_BoundingBoxNode, DEBUG_LIGHT_BOUNDING_BOX);
		}

		aabbox3df bBox = info.m_TrackedNode->getBoundingBox();
		info.m_BoundingBoxNode->setScale(bBox.getExtent());

		const matrix4 &trans = info.m_TrackedNode->getAbsoluteTransformation();
		vector3df offset;
		trans.rotateVect(offset, bBox.getCenter());
		info.m_BoundingBoxNode->setPosition(trans.getTranslation() + offset);
		info.m_BoundingBoxNode->setRotation(trans.getRotationDegrees());
	}
	else if (info.m_BoundingBoxNode)
	{
		info.m_BoundingBoxNode->remove();
		info.m_BoundingBoxNode = NULL;
	}
}

void DebugScene::updatePhysicsNodes(NODE_TRACKING_INFO &info)
{
	if (m_DrawPhysicsShape)
	{
		if (info.m_TrackedNode->getType() == ESNT_ENTITY)
		{
			Entity *e = dynamic_cast<Entity*>(info.m_TrackedNode);
			ComponentPhysical::Ptr cp = dynamic_pointer_cast<ComponentPhysical>(e->getComponent(COMPONENT_PHYSICAL));

			if (cp)
			{
				btCompoundShape *compoundShape = cp->m_CompoundShape;
				btTransform bodyTrans = cp->m_Body->getCenterOfMassTransform();

				for (int i = 0; i < compoundShape->getNumChildShapes(); i++)
				{
					btCollisionShape *shape = compoundShape->getChildShape(i);
					btTransform shapeTransform = bodyTrans * compoundShape->getChildTransform(i);
					matrix4 shapeIrrTransform;

					toIrrTransform(shapeTransform, shapeIrrTransform);

					if (i >= (int)info.m_PhysicsShapeNodes.size())
					{
						info.m_PhysicsShapeNodes.push_back(createNodeForShape(shape));
						info.m_PhysicsShapes.push_back(shape);

						if (info.m_PhysicsShapeNodes[i])
							setMeshColor(info.m_PhysicsShapeNodes[i], DEBUG_LIGHT_PHYSICS_SHAPE);
					}
					else if (shape != info.m_PhysicsShapes[i])
					{
						info.m_PhysicsShapes[i] = shape;
						info.m_PhysicsShapeNodes[i]->remove();
						if (info.m_PhysicsShapeNodes[i] = createNodeForShape(shape))
							setMeshColor(info.m_PhysicsShapeNodes[i], DEBUG_LIGHT_PHYSICS_SHAPE);
					}

					// Might be NULL if shape creation failed
					if (info.m_PhysicsShapeNodes[i])
					{
						info.m_PhysicsShapeNodes[i]->setPosition(shapeIrrTransform.getTranslation());
						info.m_PhysicsShapeNodes[i]->setRotation(shapeIrrTransform.getRotationDegrees());
					}
				}
			}
		}
	}
	else if (info.m_PhysicsShapeNodes.size() > 0)
	{
		for (ISceneNode *node : info.m_PhysicsShapeNodes)
		{
			if (node) { node->remove(); }
		}

		info.m_PhysicsShapeNodes.clear();
		info.m_PhysicsShapes.clear();
	}
}

void DebugScene::addCameraAndLights(ISceneManager *debugMgr)
{
	// Lights have no effect right now
	debugMgr->addLightSceneNode(debugMgr->getRootSceneNode(), ZERO_VECTOR, SColorf(0.0f, 10.0f, 0.0f), 1000.0f, DEBUG_LIGHT_BOUNDING_BOX);
	debugMgr->addLightSceneNode(debugMgr->getRootSceneNode(), ZERO_VECTOR, SColorf(0.3f, 1.0f, 0.3f), 10.0f, DEBUG_LIGHT_PHYSICS_SHAPE);

	ICameraSceneNode *camera = debugMgr->addCameraSceneNode(debugMgr->getRootSceneNode(), ZERO_VECTOR, ZERO_VECTOR, DEBUG_CAMERA_ID, true);
	//camera->bindTargetAndRotation(true);
}

void DebugScene::removeCameraAndLights(ISceneManager *debugMgr)
{
	ISceneNode *n;
	
	if (n = debugMgr->getSceneNodeFromId(DEBUG_LIGHT_BOUNDING_BOX)) { n->remove(); }
	if (n = debugMgr->getSceneNodeFromId(DEBUG_LIGHT_PHYSICS_SHAPE)) { n->remove(); }
	if (n = debugMgr->getSceneNodeFromId(DEBUG_CAMERA_ID)) { n->remove(); }
}

void DebugScene::updateCameraAndLightPos(ISceneManager *mgr, ISceneManager *debugMgr)
{
	ICameraSceneNode *camera = mgr->getActiveCamera(), *debugCamera;

	if (camera)
	{
		const matrix4 &trans = camera->getAbsoluteTransformation();
		vector3df pos = trans.getTranslation();

		debugMgr->getSceneNodeFromId(DEBUG_LIGHT_BOUNDING_BOX)->setPosition(pos);
		debugMgr->getSceneNodeFromId(DEBUG_LIGHT_PHYSICS_SHAPE)->setPosition(pos);

		debugCamera = dynamic_cast<ICameraSceneNode*>(debugMgr->getSceneNodeFromId(DEBUG_CAMERA_ID));
		debugCamera->setPosition(pos);
		//debugCamera->setRotation(trans.getRotationDegrees());
		debugCamera->setTarget(camera->getTarget());
		debugCamera->setProjectionMatrix(camera->getProjectionMatrix());
	}
}

//void DebugScene::DebugLightManager::OnNodePreRender(ISceneNode* node)
//{
//	u32 i;
//	s32 nodeID;
//
//	nodeID = node->getID();
//
//	// Only enable those lights that match the node's ID
//	for(i = 0; i < m_SceneLightList->size(); i++)
//	{
//		scene::ISceneNode* lightNode = (*m_SceneLightList)[i];
//		lightNode->setVisible(lightNode->getID() == nodeID);
//	}
//}
