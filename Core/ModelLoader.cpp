#pragma once
#include "ModelLoader.h"
#include "Logger.h"

ModelLoader::ModelLoader()
{
}

ModelLoader::~ModelLoader()
{
	delete[] m_textureIds;
}

bool ModelLoader::importModel(const std::string pFile, unsigned int pFlags)
{
	// Extract the directory part from the file name
    std::string::size_type slashIndex = pFile.find_last_of("/");

    if (slashIndex == std::string::npos) {
        m_modelPathDir = ".";
    }
    else if (slashIndex == 0) {
        m_modelPathDir = "/";
    }
    else {
        m_modelPathDir = pFile.substr(0, slashIndex+1);
    }

	m_importer.FreeScene();

	const aiScene* scene = m_importer.ReadFile( pFile, pFlags );
	m_originalPosition = scene->mRootNode;

	if(!scene)  { Logger::GetInstance().Log(m_importer.GetErrorString()); return false; }

	//Access file content here.
	m_scene = scene;

	m_meshes = scene->mMeshes;

	m_materials = scene->mMaterials;

	m_mesh_count = scene->mNumMeshes;

	return true;
}

const aiScene* ModelLoader::getScene() {return m_scene;}
aiMesh** ModelLoader::getMeshes(){return m_meshes;}
unsigned int* ModelLoader::getTextureIds(){return m_textureIds;}
aiMaterial** ModelLoader::getMaterials(){return m_materials;}
glm::vec3 ModelLoader::getPosition(){return glm::vec3(m_originalPosition->mTransformation.d1, m_originalPosition->mTransformation.d2, m_originalPosition->mTransformation.d3);}
float ModelLoader::getScale(){return m_scale;}