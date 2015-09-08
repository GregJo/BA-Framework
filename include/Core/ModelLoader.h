#pragma once 
#include "BitMap.h"

#include "..\..\dependencies\assimp\include\assimp\Importer.hpp"
#include "..\..\dependencies\assimp\include\assimp\scene.h"
#include "..\..\dependencies\assimp\include\assimp\postprocess.h"

#include "..\..\dependencies\glm\glm.hpp"
#include "..\..\dependencies\glm\gtc\matrix_transform.hpp"



class ModelLoader
{
public:
	//! \param position Position of the model in worldspace.
	ModelLoader();
	~ModelLoader();

	//! \brief Import Model.
	//! \param pFile Path of the model.
	//! \param pFlags Post process flags for assimp's model importer (flags for i.e. precalculated tangent space, flip UVs, etc.).
	//! \return true if loading succeeded.
	bool importModel(const std::string pFile, unsigned int pFlags);

	const aiScene* getScene();
	aiMesh** getMeshes();
	unsigned int* getTextureIds();
	aiMaterial** getMaterials();
	glm::vec3 getPosition();
	float getScale();

private:
	Assimp::Importer m_importer;

	std::string		m_modelPathDir;
	const aiScene*  m_scene;
	aiMesh**	m_meshes;
	aiNode*	m_originalPosition;				//Root of the aiScene of assimp.
	unsigned int*	m_textureIds;
	aiMaterial**	m_materials;

	float			m_scale;
	glm::vec3       m_position;

	unsigned int	m_mesh_count;
};