#ifndef __MATERIALTYPESWITCHER_HPP__
#define __MATERIALTYPESWITCHER_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Convenience class to save, switch, and restore the material types of a scene node.
 */

#include "..\\include\\Common.hpp"

class MaterialTypeSwitcher
{
public:
	virtual void switchMaterialTypes(ISceneNode *node, E_MATERIAL_TYPE newMatType)
	{
		int numMaterials = node->getMaterialCount();
		m_SwitchedOutMaterials.clear();
		m_SwitchedOutMaterials.reserve(numMaterials);
		for (int i = 0; i < numMaterials; i++)
		{
			m_SwitchedOutMaterials.push_back(node->getMaterial(i).MaterialType);
		}

		node->setMaterialType(newMatType);
	}

	virtual void restoreMaterialTypes(ISceneNode *node)
	{
		for (unsigned int i = 0; i < m_SwitchedOutMaterials.size(); i++)
		{
			node->getMaterial(i).MaterialType = m_SwitchedOutMaterials[i];
		}
	}

protected:
	std::vector<E_MATERIAL_TYPE> m_SwitchedOutMaterials;
};

#endif