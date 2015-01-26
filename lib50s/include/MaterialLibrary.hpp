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
		IShaderConstantSetCallBack *constantSetCallback;

		std::string materialName;

		E_MATERIAL_TYPE baseMaterial;

		E_GPU_SHADING_LANGUAGE gpuShadingLanguage;
	} MATERIAL_DEFINITION;

	MaterialLibrary(IVideoDriver *driver);
	virtual ~MaterialLibrary();

	// Add a material to this library, and register it with the video driver.
	// This process also involves generating and compiling all the necessary shaders.
	// Returns the ID of the registered material renderer. Returns -1 on failure.
	virtual u32							addMaterial(const MATERIAL_DEFINITION &def);

	// Returns the ID of the registered material renderer, or -1 if not found.
	virtual u32							getMaterialID(const std::string &name) const;
	// Convenience function to look up material ID
	virtual E_MATERIAL_TYPE				operator[](const std::string &name) const;

	virtual std::vector<std::string>	getMaterialNames() const;

	// Add the Irrlicht built-in materials to the name/id lookup table
	virtual void						addIrrlichtBuiltInMaterials();

protected:
	class DefaultConstantSetCallback : public IShaderConstantSetCallBack
	{
	public:
		DefaultConstantSetCallback(std::vector<IShaderConstantSetCallBack*> callbacks);
		virtual ~DefaultConstantSetCallback();

		virtual void OnSetMaterial(const SMaterial& material);
		virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);

	protected:
		std::vector<IShaderConstantSetCallBack*> m_Callbacks;
	};

	std::vector<MATERIAL_DEFINITION> m_MaterialDefs;
	std::map<std::string, u32> m_MaterialIDs; // Note: May have additional entries not found in m_MaterialDefs

	IVideoDriver *m_VideoDriver;

private:
	// I don't see any reason for copying a MaterialLibrary as that would require
	// copying all the shaders, callbacks, etc.
	MaterialLibrary(MaterialLibrary &obj);
};

#endif