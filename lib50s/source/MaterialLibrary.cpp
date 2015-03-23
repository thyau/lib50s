/*
	MaterialLibrary.cpp
	Copyright (c) [ORGNAME] 2014
	Author: Timothy Yau
*/

#include <sstream>

#include "..\\include\\MaterialLibrary.hpp"
#include "..\\include\\WindowsConsole.hpp"

#include "EMaterialTypes.h"
#include "EPrimitiveTypes.h"

using namespace std;

using namespace irr;
using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;

MaterialLibrary::MaterialLibrary(IVideoDriver *driver)
	: m_VideoDriver(driver)
{
}

MaterialLibrary::~MaterialLibrary()
{
	// There's no way to un-register materials with the driver

	for (MATERIAL_DEFINITION &def : m_MaterialDefs)
	{
		def.constantSetCallback->drop();
	}
}

u32 MaterialLibrary::addMaterial(const MATERIAL_DEFINITION &materialDef)
{
	IGPUProgrammingServices *gpuServices = m_VideoDriver->getGPUProgrammingServices();

	if (m_MaterialIDs.find(materialDef.materialName) != m_MaterialIDs.end())
	{
		PRINT("MaterialLibrary::addMaterial: Error: Material " << materialDef.materialName << " already exists." << endl, RED);
		return -1;
	}

	m_MaterialDefs.push_back(materialDef);
	MATERIAL_DEFINITION &def = m_MaterialDefs.back();

	if (def.constantSetCallback)
	{
		def.constantSetCallback->grab();
	}
	else
	{
		std::vector<IShaderConstantSetCallBack*> callbacks;
		IShaderConstantSetCallBack *cb;

		cb = def.vertexShader->getDefaultCallbackObject();
		if (cb) { callbacks.push_back(cb); }

		cb = def.pixelShader->getDefaultCallbackObject();
		if (cb) { callbacks.push_back(cb); }

		if (def.geometryShader)
		{
			cb = def.geometryShader->getDefaultCallbackObject();
			if (cb) { callbacks.push_back(cb); }
		}

		def.constantSetCallback = new DefaultConstantSetCallback(callbacks);

		for (auto callback : callbacks)
			callback->drop();
	}

	std::string vsSource = def.vertexShader->buildSource();
	std::string psSource = def.pixelShader->buildSource();
	std::string gsSource = def.geometryShader? def.geometryShader->buildSource() : "";

	PRINT("Adding material " << def.materialName << endl, WHITE);

	u32 id = gpuServices->addHighLevelShaderMaterial(vsSource.c_str(), 
		def.vertexShader->getEntryPointName().c_str(),
		def.vertexShader->getCompileTarget(),
		psSource.c_str(),
		def.pixelShader->getEntryPointName().c_str(),
		def.pixelShader->getCompileTarget(),
		gsSource.empty()? NULL : gsSource.c_str(),
		def.geometryShader? def.geometryShader->getEntryPointName().c_str() : NULL,
		def.geometryShader? def.geometryShader->getCompileTarget() : EGST_GS_4_0,
		def.geometryShader? def.geometryShader->getGeometryInType() : EPT_TRIANGLES,
		def.geometryShader? def.geometryShader->getGeometryOutType() : EPT_TRIANGLE_STRIP,
		def.geometryShader? def.geometryShader->getMaxVerticesOut() : 0,
		def.constantSetCallback,
		def.baseMaterial,
		0,
		def.gpuShadingLanguage);

	if (id == -1)
	{
		PRINT("MaterialLibrary::addMaterial: Error: Material " << materialDef.materialName << " failed to compile and/or register." << endl, RED);

		int lineNum;
		std::string line;
		istringstream iss;
		vector<std::string> sourceStrings;
		sourceStrings.push_back(vsSource);
		sourceStrings.push_back(psSource);
		sourceStrings.push_back(gsSource);

		for (unsigned int i = 0; i < sourceStrings.size(); i++)
		{
			iss.clear();
			iss.str(sourceStrings[i]);
			lineNum = 1;

			while (true) {
				getline(iss, line);

				if (iss.eof())
					break;

				PRINT(to_string(lineNum) << ":\t" << line << endl, WHITE);
				lineNum++;
			}
		}
	}
	else {
		m_MaterialIDs.insert(make_pair(def.materialName, id));
		m_VideoDriver->setMaterialRendererName(id, def.materialName.c_str());
	}

	return id;
}

u32 MaterialLibrary::getMaterialID(const std::string &name) const
{
	auto matNameId = m_MaterialIDs.find(name);

	if (matNameId == m_MaterialIDs.end())
		return -1;
	else
		return matNameId->second;
}

E_MATERIAL_TYPE MaterialLibrary::operator[](const std::string &name) const
{
	return (E_MATERIAL_TYPE)getMaterialID(name);
}

vector<std::string> MaterialLibrary::getMaterialNames() const
{
	vector<std::string> names;

	for (auto& matNameId : m_MaterialIDs)
	{
		names.push_back(matNameId.first);
	}

	return names;
}

void MaterialLibrary::addIrrlichtBuiltInMaterials()
{
	int i = 0;

	while (sBuiltInMaterialTypeNames[i])
	{
		m_MaterialIDs.insert(make_pair(sBuiltInMaterialTypeNames[i], i));
		i += 1;
	}
}

MaterialLibrary::DefaultConstantSetCallback::DefaultConstantSetCallback(std::vector<IShaderConstantSetCallBack*> callbacks)
	: m_Callbacks(callbacks)
{
	for (auto callback : m_Callbacks)
	{
		callback->grab();
	}
}

MaterialLibrary::DefaultConstantSetCallback::~DefaultConstantSetCallback()
{
	for (auto callback : m_Callbacks)
	{
		callback->drop();
	}
}

void MaterialLibrary::DefaultConstantSetCallback::OnSetMaterial(const SMaterial& material)
{
	for (auto callback : m_Callbacks)
	{
		callback->OnSetMaterial(material);
	}
}

void MaterialLibrary::DefaultConstantSetCallback::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	for (auto callback : m_Callbacks)
	{
		callback->OnSetConstants(services, userData);
	}
}
