#pragma once
#include "Model.h"
#include "Logger.h"
#include "TextureManager.h"
#include "OGLErrorHandling.h"
#include <map>

Model::Model(glm::vec3 position, std::string modelPath) :  m_position(position), m_scale(0), m_mesh_count(0), m_modelPath(modelPath)
{
	m_currentGLSLProg=new GLSLProgram();
	m_currentGLSLProg->initShaderProgram("Vert.glsl", "", "", "", "Frag.glsl");

	check_gl_error();
}

Model::~Model()
{
	//Delete ModelMeshes in m_model_meshes?
	delete[] m_textureIds;
}

bool Model::loadModel(const aiScene* scene)
{
	m_originalPosition = glm::vec3(scene->mRootNode->mTransformation.d1,scene->mRootNode->mTransformation.d2,scene->mRootNode->mTransformation.d3);

	aiMaterial* material;

	m_mesh_count = scene->mNumMeshes;

	Mesh** meshes = new Mesh*[m_mesh_count];

	m_materialIds = new unsigned int[m_mesh_count];

	std::vector<std::string> fullTexturePaths;

	for(unsigned int i = 0; i < m_mesh_count; i++)
	{
		meshes[i] = new Mesh();
		meshes[i]->loadMeshData(scene->mMeshes[i]);

		meshes[i]->initMeshData();

		material = scene->mMaterials[scene->mMeshes[i]->mMaterialIndex];

		// At the moment diffuse textures only.
		fullTexturePaths = TextureManager::GetInstance().makeFullTexturePaths(material, m_modelPath);

		if(fullTexturePaths.size() > 0) 
		{
			TextureManager::GetInstance().loadTexture(fullTexturePaths[0]);
		
			meshes[i]->setDiffuseTextureHandle(TextureManager::GetInstance().getTextureHandle(fullTexturePaths[0]));

			unsigned int hasOpacityMap = material->GetTextureCount(aiTextureType_OPACITY);

			if (hasOpacityMap > 0)
			{
				meshes[i]->setTransparencyAttribute(true);
				m_transparentMeshes.push_back(meshes[i]);
			} else
			{
				m_opaqueMeshes.push_back(meshes[i]);
			}
		}

		//load Material
		meshes[i]->loadMaterial(material, m_currentGLSLProg->getHandle());
		m_meshes.push_back(meshes[i]);
	}

	delete[] meshes;

	check_gl_error();

	return true;
}

void Model::setGLSLProgram(GLSLProgram* shaderProgram)
{
	m_currentGLSLProg = shaderProgram;
}

void Model::loadMaterial(aiMaterial* material)
{

}

void Model::render()
{
	// Later on this implement the ability of using shader programs per mesh.
	//m_currentShaderProg->use();

	for(unsigned int i = 0; i < m_meshes.size(); i++)
	{
		//TODO: Set different samplers for different texture types(diffuse, specular, etc.) via glActiveTexture(GL_TEXTUREi)
		m_meshes[i]->getMaterialUniformBuffer()->bindUniformBuffer();

		if(m_meshes[i]->getDiffuseTextureHandle() != 0)
		{
			glBindTexture(GL_TEXTURE_2D, m_meshes[i]->getDiffuseTextureHandle());
		}
		//m_model_meshes[i]->bindMaterial();
		m_meshes[i]->render();
		//Set Samplers via SetUniform().
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	check_gl_error();
}

void Model::renderOpaque()
{
	// Later on this implement the ability of using shader programs per mesh.
	//m_currentShaderProg->use();

	for(unsigned int i = 0; i < m_opaqueMeshes.size(); i++)
	{
		//TODO: Set different samplers for different texture types(diffuse, specular, etc.) via glActiveTexture(GL_TEXTUREi)
		m_opaqueMeshes[i]->getMaterialUniformBuffer()->bindUniformBuffer();

		if(m_opaqueMeshes[i]->getDiffuseTextureHandle() != 0)
		{
			glBindTexture(GL_TEXTURE_2D, m_opaqueMeshes[i]->getDiffuseTextureHandle());
		}
		//m_model_meshes[i]->bindMaterial();
		m_opaqueMeshes[i]->render();
		//Set Samplers via SetUniform().
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	check_gl_error();
}

void Model::renderTransparent()
{
	// Later on this implement the ability of using shader programs per mesh.
	//m_currentShaderProg->use();

	for(unsigned int i = 0; i < m_transparentMeshes.size(); i++)
	{
		//TODO: Set different samplers for different texture types(diffuse, specular, etc.) via glActiveTexture(GL_TEXTUREi)
		m_transparentMeshes[i]->getMaterialUniformBuffer()->bindUniformBuffer();

		if(m_transparentMeshes[i]->getDiffuseTextureHandle() != 0)
		{
			glBindTexture(GL_TEXTURE_2D, m_transparentMeshes[i]->getDiffuseTextureHandle());
		}
		//m_model_meshes[i]->bindMaterial();
		m_transparentMeshes[i]->render();
		//Set Samplers via SetUniform().
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	check_gl_error();
}

void Model::renderWithAdditionalTextures(std::vector<GLuint> additionalTextureHandles, GLuint samplerHandle)
{
	// Later on this implement the ability of using shader programs per mesh.
	//m_currentShaderProg->use();

	for(unsigned int i = 0; i < m_meshes.size(); i++)
	{
		//TODO: Set different samplers for different texture types(diffuse, specular, etc.) via glActiveTexture(GL_TEXTUREi)
		m_meshes[i]->getMaterialUniformBuffer()->bindUniformBuffer();

		if(m_meshes[i]->getDiffuseTextureHandle() != 0)
		{
			glBindSampler(0, samplerHandle);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_meshes[i]->getDiffuseTextureHandle());
		}
		if(additionalTextureHandles.size()>0 && additionalTextureHandles.size()<31)
		{
			for (unsigned int j = 0; j < additionalTextureHandles.size(); j++)
			{
				glBindSampler(1+j, samplerHandle);
				glActiveTexture(GL_TEXTURE1+j);
				//glActiveTexture(GL_TEXTURE1+j);

				glBindTexture(GL_TEXTURE_2D, additionalTextureHandles[j]);
			}
			glActiveTexture(GL_TEXTURE31);
		}

		//m_model_meshes[i]->bindMaterial();
		m_meshes[i]->render();
		//Set Samplers via SetUniform().
	}
	glBindSampler(0, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	check_gl_error();
}

void Model::renderOpaqueWithAdditionalTextures(std::vector<GLuint> additionalTextureHandles, GLuint samplerHandle)
{
	// Later on this implement the ability of using shader programs per mesh.
	//m_currentShaderProg->use();

	for(unsigned int i = 0; i < m_opaqueMeshes.size(); i++)
	{
		//TODO: Set different samplers for different texture types(diffuse, specular, etc.) via glActiveTexture(GL_TEXTUREi)
		m_opaqueMeshes[i]->getMaterialUniformBuffer()->bindUniformBuffer();

		if(m_opaqueMeshes[i]->getDiffuseTextureHandle() != 0)
		{
			glBindSampler(0, samplerHandle);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_opaqueMeshes[i]->getDiffuseTextureHandle());
		}
		if(additionalTextureHandles.size()>0 && additionalTextureHandles.size()<31)
		{
			for (unsigned int j = 0; j < additionalTextureHandles.size(); j++)
			{
				glBindSampler(1+j, samplerHandle);
				glActiveTexture(GL_TEXTURE1+j);
				//glActiveTexture(GL_TEXTURE1+j);

				glBindTexture(GL_TEXTURE_2D, additionalTextureHandles[j]);
			}
			glActiveTexture(GL_TEXTURE31);
		}

		//m_model_meshes[i]->bindMaterial();
		m_opaqueMeshes[i]->render();
		//Set Samplers via SetUniform().
	}
	glBindSampler(0, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	check_gl_error();
}

void Model::renderTransparentWithAdditionalTextures(std::vector<GLuint> additionalTextureHandles, GLuint samplerHandle)
{
	// Later on this implement the ability of using shader programs per mesh.
	//m_currentShaderProg->use();

	for(unsigned int i = 0; i < m_transparentMeshes.size(); i++)
	{
		//TODO: Set different samplers for different texture types(diffuse, specular, etc.) via glActiveTexture(GL_TEXTUREi)
		m_transparentMeshes[i]->getMaterialUniformBuffer()->bindUniformBuffer();

		if(m_transparentMeshes[i]->getDiffuseTextureHandle() != 0)
		{
			glBindSampler(0, samplerHandle);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_transparentMeshes[i]->getDiffuseTextureHandle());
		}
		if(additionalTextureHandles.size()>0 && additionalTextureHandles.size()<31)
		{
			for (unsigned int j = 0; j < additionalTextureHandles.size(); j++)
			{
				glBindSampler(1+j, samplerHandle);
				glActiveTexture(GL_TEXTURE1+j);
				//glActiveTexture(GL_TEXTURE1+j);

				glBindTexture(GL_TEXTURE_2D, additionalTextureHandles[j]);
			}
			glActiveTexture(GL_TEXTURE31);
		}

		//m_model_meshes[i]->bindMaterial();
		m_transparentMeshes[i]->render();
		//Set Samplers via SetUniform().
	}
	glBindSampler(0, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	check_gl_error();
}

void Model::render(RenderMode renderMode, std::vector<GLuint> additionalTextureHandles, GLuint samplerHandle)
{
	// Later on this implement the ability of using shader programs per mesh.
	//m_currentShaderProg->use();

	std::vector<Mesh*>	meshesToRender;
	unsigned int samplerCount = 0;

	if (renderMode == RenderMode::COMPLETE)
	{
		meshesToRender = m_meshes;
	} else if (renderMode == RenderMode::OPAQUE)
	{
		meshesToRender = m_opaqueMeshes;
	} else if (renderMode == RenderMode::TRANSPARENT)
	{
		meshesToRender = m_transparentMeshes;
	}

	for(unsigned int i = 0; i < meshesToRender.size(); i++)
	{
		samplerCount = 0;
		//TODO: Set different samplers for different texture types(diffuse, specular, etc.) via glActiveTexture(GL_TEXTUREi) -> add a sampler count, that counts for every 
		meshesToRender[i]->getMaterialUniformBuffer()->bindUniformBuffer();

		if(meshesToRender[i]->getDiffuseTextureHandle() != 0)
		{
			glBindSampler(0, samplerHandle);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, meshesToRender[i]->getDiffuseTextureHandle());
			samplerCount++;
		}
		if(additionalTextureHandles.size()>0 && additionalTextureHandles.size()<31)
		{
			for (unsigned int j = 0; j < additionalTextureHandles.size(); j++)
			{
				glBindSampler(samplerCount, samplerHandle);
				glActiveTexture(GL_TEXTURE0+samplerCount);

				glBindTexture(GL_TEXTURE_2D, additionalTextureHandles[j]);
				samplerCount++;
			}
		}
		glActiveTexture(GL_TEXTURE31);

		//m_model_meshes[i]->bindMaterial();
		meshesToRender[i]->render();
		//Set Samplers via SetUniform().
	}
	glBindSampler(0, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	check_gl_error();
}