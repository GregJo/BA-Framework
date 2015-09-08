#pragma once
#include "..\..\dependencies\gl\include\glew.h"
#include "..\..\dependencies\assimp\include\assimp\scene.h"
#include "VAO.h"
#include "MeshData.h"
#include "GLSLUniformBuffer.h"
#include "Logger.h"
#include <vector>

class Mesh
{
public:
	struct Material
	{
		GLfloat diffuse[4];
		GLfloat ambient[4];
		GLfloat specular[4];
		GLfloat emissive[4];
		float shininess;
		//int texCount;
	};

	Mesh();

    ~Mesh();

    bool loadMeshData(const aiMesh* mesh);
	void initMeshData();
	
	void loadMaterial(aiMaterial* material, GLuint shaderProgHandle);

	GLSLUniformBuffer* getMaterialUniformBuffer() { return m_material_uniform_buffer; }

	void setDiffuseTextureHandle(GLuint diffuseTextureHandle) { m_diffuseTextureHandle = diffuseTextureHandle; }
	GLuint getDiffuseTextureHandle() { return m_diffuseTextureHandle; }

	void setTransparencyAttribute(bool transparencyAttribute ){ m_isTransparent = transparencyAttribute; }
	bool isTransparent(){ return m_isTransparent; }

    void render();

private:

#define INVALID_MATERIAL 0xFFFFFFFF

	GLfloat* m_vertex_buffer_data;
	GLfloat* m_texture_coord_buffer_data;
	GLfloat* m_normal_buffer_data;
	GLfloat* m_color_buffer_data;
	GLuint*  m_index_buffer_data;

	unsigned int m_vertices_count;
	unsigned int m_indices_count;

	unsigned int m_materialIndex;
	bool m_isTransparent;
	
	VAO* m_vao;

	GLSLUniformBuffer* m_material_uniform_buffer;
	Material m_material;

	GLuint m_diffuseTextureHandle;
};