#pragma once 

#include "GLSLProgram.h"
#include "Mesh.h"
#include "../../dependencies/glm/glm.hpp"
#include "../../dependencies/glm/gtc/matrix_transform.hpp"

//TODO: Create logs.
//TODO: Create Material struct and a method that returns it.
//Errors to cover:
//TODO: Add a shader member variable and a default shader.

class Model
{
public:
    enum class RenderMode 
	{
		OPAQUE,
		TRANSPARENT,
		COMPLETE
    };

	//! \param position Position of the model in worldspace.
	Model() { }
	Model(glm::vec3 position, std::string modelPath);
	~Model();

	bool loadModel(const std::string pFile, unsigned int pFlags);
	bool loadModel(const aiScene* scene);

	//! \brief Load all Textures of the scene, often the model itself.
	//! \param scene Scene containing the model, often the model itself.
	//! \return true Return true if loading succeeded.
	bool loadTextures(const aiScene* scene);

	void setGLSLProgram(GLuint shaderProgHandle);
	void setGLSLProgram(GLSLProgram* shaderProgram);
	
	void useCurrentGLSLProgram() { m_currentGLSLProg->use(); }

	GLSLProgram* getCurrentGLSLProgram() { return m_currentGLSLProg; }

	void setPosition(glm::vec3 position)
	{	
		m_position = position;
	}

	void setScale(float scale)
	{	
		m_scale = scale;
	}

	glm::vec3 getPosition()
	{	
		return m_position;
	}

	glm::mat4 getWorldMatrix()
	{
		return glm::translate(glm::mat4(1), m_position);
	}

	float getScale()
	{	
		return m_scale;
	}

	glm::mat4 getScaleMatrix()
	{
		return glm::scale(glm::mat4(1),glm::vec3(m_scale,m_scale,m_scale)); 
	}

	//! \brief Load mesh matrial.
	//! \param material Material of a mesh.
	void loadMaterial(aiMaterial* material);

	//TODO: Different samplers for different texture(diffuse, glossy, etc.) types. 
	void render();
	void renderOpaque();
	void renderTransparent();

	void renderWithAdditionalTextures(std::vector<GLuint> additionalTextureHandles, GLuint samplerHandle);
	void renderOpaqueWithAdditionalTextures(std::vector<GLuint> additionalTextureHandles, GLuint samplerHandle);
	void renderTransparentWithAdditionalTextures(std::vector<GLuint> additionalTextureHandles, GLuint samplerHandle);

	// current TODO
	void render(RenderMode renderMode, std::vector<GLuint> additionalTextureHandles, GLuint samplerHandle);

private:
	GLSLProgram*		m_currentGLSLProg;

	std::string			m_modelPath;

	std::vector<Mesh*>	m_meshes;
	std::vector<Mesh*>	m_opaqueMeshes;
	std::vector<Mesh*>	m_transparentMeshes;
	GLuint*				m_textureIds;
	unsigned int*		m_materialIds;

	float				m_scale;
	glm::vec3			m_position;
	glm::vec3			m_originalPosition;

	unsigned int		m_mesh_count;
};
