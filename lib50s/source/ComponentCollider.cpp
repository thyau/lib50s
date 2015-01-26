#include "..\include\ComponentCollider.hpp"


ComponentCollider::ComponentCollider(void)
	:  m_Shape(shared_ptr<btCollisionShape>()), m_ChildShapeID(-1), m_CompPhysicalBody(ComponentPhysical::Ptr()), m_IsStatic(false)
{
}


ComponentCollider::~ComponentCollider(void)
{
	detachShape();
}

void ComponentCollider::onAttach(const std::shared_ptr<Entity> &parent)
{
	BaseComponent::onAttach(parent);
	m_Connections[ENT_SIGNAL_SUBTREE] = parent->subscribeSubtreeChange(WO_CALL_FIRST, boost::bind(&ComponentCollider::sig_SubtreeChange, this, _1));
	//m_Connections[ENT_SIGNAL_POST_ANIMATE] = parent->subscribePostAnimate(WO_CALL_FIRST, boost::bind(&ComponentCollider::sig_PostAnimate, this, _1));

	reattachShape();
}

void ComponentCollider::onDetach(const std::shared_ptr<Entity> &parent)
{
	BaseComponent::onDetach(parent);
	detachShape();
}

void ComponentCollider::postAnimate(u32 timeMs)
{
	if (!m_IsStatic && m_CompPhysicalBody)
	{
		// TODO: optimize to remove this call if this collider and ComponentPhysical are attached to the same entity
		m_CompPhysicalBody->updateChildTransform(m_Parent->getAbsoluteTransformation(), m_ChildShapeID, m_Mass);
	}
}

void ComponentCollider::sig_SubtreeChange(ISceneNode *root)
{
	assert(m_Parent && "Parent should not be null for ComponentCollider to receive SubtreeChange events");
	reattachShape();
}

void ComponentCollider::SpawnData::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	// TODO: implement
}

void ComponentCollider::SpawnData::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	// TODO: implement
}

btCollisionShape* ComponentCollider::SpawnData::createCollisionShape() const
{
	btCollisionShape* shape = NULL;

	switch (m_ShapeType)
	{
	// This code borrows from btWorldImporter.cpp
	case STATIC_PLANE_PROXYTYPE:
		{
			shape = new btStaticPlaneShape(m_PlaneNormal,m_PlaneConstant);
			//shape->setLocalScaling(localScaling);
			break;
		}

	case SPHERE_SHAPE_PROXYTYPE:
		shape = new btSphereShape(min(min(m_BoundingBoxSize.x(), m_BoundingBoxSize.y()), m_BoundingBoxSize.z())/2);
		break;

	case CYLINDER_SHAPE_PROXYTYPE:
		{
			float radius, height;

			switch (m_Orientation)
			{
			case 0:
				radius = min(m_BoundingBoxSize.y(), m_BoundingBoxSize.z()) / 2;
				height = m_BoundingBoxSize.x();
				shape = new btCylinderShapeX(btVector3(height/2, radius, radius));
				break;

			case 1:
				radius = min(m_BoundingBoxSize.x(), m_BoundingBoxSize.z()) / 2;
				height = m_BoundingBoxSize.y();
				shape = new btCylinderShape(btVector3(radius, height/2, radius));
				break;

			case 2:
				radius = min(m_BoundingBoxSize.x(), m_BoundingBoxSize.y()) / 2;
				height = m_BoundingBoxSize.z();
				shape = new btCylinderShapeZ(btVector3(radius, radius, height/2));
				break;

			default:
				PRINT("ComponentCollider::SpawnData::createCollisionShape: invalid axis for btCylinderShape\n", YELLOW);
			}
			break;
		}

	case CAPSULE_SHAPE_PROXYTYPE:
		{
			float radius, height;

			switch (m_Orientation)
			{
			case 0:
				//shape = createCapsuleShapeX(implicitShapeDimensions.getY()+bsd->m_collisionMargin*2,2*implicitShapeDimensions.getX());
				radius = min(m_BoundingBoxSize.y(), m_BoundingBoxSize.z()) / 2;
				height = max(0.0f, m_BoundingBoxSize.x() - radius*2);
				shape = new btCapsuleShapeX(radius, height);
				break;
			
			case 1:
				//shape = createCapsuleShapeY(implicitShapeDimensions.getX()+bsd->m_collisionMargin*2,2*implicitShapeDimensions.getY());
				radius = min(m_BoundingBoxSize.x(), m_BoundingBoxSize.z()) / 2;
				height = max(0.0f, m_BoundingBoxSize.y() - radius*2);
				shape = new btCapsuleShape(radius, height);
				break;

			case 2:
				//shape = createCapsuleShapeZ(implicitShapeDimensions.getX()+bsd->m_collisionMargin*2,2*implicitShapeDimensions.getZ());
				radius = min(m_BoundingBoxSize.x(), m_BoundingBoxSize.y()) / 2;
				height = max(0.0f, m_BoundingBoxSize.z() - radius*2);
				shape = new btCapsuleShapeZ(radius, height);
				break;
							
			default:
				PRINT("ComponentCollider::SpawnData::createCollisionShape: invalid axis for btCapsuleShape\n", YELLOW);
			}		
			break;
		}

	case BOX_SHAPE_PROXYTYPE:
		shape = new btBoxShape(m_BoundingBoxSize/2);
		break;
				
	default:
		// TODO: load trimesh shapes
		PRINT("ComponentCollider::SpawnData::createCollisionShape: unknown shape type\n", YELLOW);
		break;
	}

	if (!shape) {
		// Error: default to a unit cube
		shape = new btBoxShape(BT_ONES_VECTOR/2);
	}

	return shape;
}

void ComponentCollider::detachShape()
{
	if (m_CompPhysicalBody)
	{
		//PRINT("ComponentCollider::detachShape from " << hex << m_CompPhysicalBody.get() << endl, WHITE);

		m_CompPhysicalBody->removeChildShape(m_ChildShapeID);

		m_ChildShapeID = -1;
		m_CompPhysicalBody.reset();
	}
}

bool ComponentCollider::attachShape(Entity::Ptr e)
{
	ComponentPhysical::Ptr cp = dynamic_pointer_cast<ComponentPhysical>(e->getComponent(COMPONENT_PHYSICAL));

	if (cp)
	{
		// Also updates the position of Entity e, which as assume to be an ancestor of m_Parent
		m_Parent->updateAbsolutePositionRecursive();
		m_ChildShapeID = cp->addChildShape(m_Parent->getAbsoluteTransformation(), m_Shape.get(), m_Mass);
		m_CompPhysicalBody = cp;

		//PRINT("ComponentCollider::attachShape to " << hex << cp.get() << " succeeded." << endl, WHITE);

		return true;
	}
	else
	{
		//PRINT("ComponentCollider::attachShape to " << hex << cp.get() << " failed." << endl, WHITE);

		return false;
	}
}

void ComponentCollider::reattachShape(void)
{
	//PRINT("ComponentCollider::reattachShape" << endl, WHITE);

	detachShape();

	// Try to attach our shape to the nearest ancestor Entity
	ISceneNode *node = m_Parent.get();

	while (node)
	{
		if (node->getType() == ESNT_ENTITY)
		{
			Entity *e = dynamic_cast<Entity*>(node);

			if (attachShape(e->getSharedPointer()))
			{
				break;
			}
		}
		
		node = node->getParent();
	}
}
