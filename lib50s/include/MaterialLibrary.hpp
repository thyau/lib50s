#ifndef __MATERIALLIBRARY_HPP__
#define __MATERIALLIBRARY_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Manages a set of materials and their definitions, and registers
 * then with the Irrlicht engine. There does not presently seem to
 * be a way to unregister materials.
 */

#include <vector>
#include <map>

#include "IShaderConstantSetCallBack.h"
#include "EMaterialTypes.h"
#include "IGPUProgrammingServices.h"
#include "IMaterialRendererServices.h"
#include "IVideoDriver.h"
#include "irrTypes.h"

#include "..\\include\\Common.hpp"
#include "..\\include\\IVertexShader.hpp"
#include "..\\include\\IPixelShader.hpp"
#include "..\\include\\IGeometryShader.hpp"

class MaterialLibrary
{
public:
	typedef std::shared_ptr<MaterialLibrary> Ptr;
	typedef std::weak_ptr<MaterialLibrary> wPtr;

	// Fill in and pass these structs to addMaterial()
	typedef struct
	{
		IVertexShader::Ptr vertexShader;
		IPixelShader::Ptr pixelShader;
		IGeometryShader::Ptr geometryShader; // May be empty ptr

		// If NULL, the default callbacks of the vertex, pixel, and geometry shaders are called sequentially
		// If newly allocated object, user should call drop() after passing to addMaterial().
		irr::video::IShaderConstantSetCallBack *constantSetCallback;

		std::string materialName;

		irr::video::E_MATERIAL_TYPE baseMaterial;

		irr::video::E_GPU_SHADING_LANGUAGE gpuShadingLanguage;
	} MATERIAL_DEFINITION;

	MaterialLibrary(irr::video::IVideoDriver *driver);
	virtual ~MaterialLibrary();

	// Add a material to this library, and register it with the video driver.
	// This process also involves generating and compiling all the necessary shaders.
	// Returns the ID of the registered material renderer. Returns -1 on failure.
	virtual irr::u32					addMaterial(const MATERIAL_DEFINITION &def);

	// Returns the ID of the registered material renderer, or -1 if not found.
	virtual irr::u32					getMaterialID(const std::string &name) const;
	// Convenience function to look up material ID
	virtual irr::video::E_MATERIAL_TYPE	operator[](const std::string &name) const;

	virtual std::vector<std::string>	getMaterialNames() const;

	// Add the Irrlicht built-in materials to the name/id lookup table
	virtual void						addIrrlichtBuiltInMaterials();

protected:
	class DefaultConstantSetCallback : public irr::video::IShaderConstantSetCallBack
	{
	public:
		DefaultConstantSetCallback(std::vector<IShaderConstantSetCallBack*> callbacks);
		virtual ~DefaultConstantSetCallback();

		virtual void OnSetMaterial(const irr::video::SMaterial& material);
		virtual void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData);

	protected:
		std::vector<IShaderConstantSetCallBack*> m_Callbacks;
	};

	std::vector<MATERIAL_DEFINITION> m_MaterialDefs;
	std::map<std::string, irr::u32> m_MaterialIDs; // Note: May have additional entries not found in m_MaterialDefs

	irr::video::IVideoDriver *m_VideoDriver;

private:
	// I don't see any reason for copying a MaterialLibrary as that would require
	// copying all the shaders, callbacks, etc.
	MaterialLibrary(MaterialLibrary &obj);
};

#endif