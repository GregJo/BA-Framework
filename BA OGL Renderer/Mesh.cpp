#pragma once
#include "VAO.h"
#include "Mesh.h"
#include "scene.h"
#include "OGLErrorHandling.h"



Mesh::Mesh() : m_vertex_buffer_data(nullptr),
	m_texture_coord_buffer_data(nullptr),
	m_normal_buffer_data(nullptr), 
	m_color_buffer_data(nullptr),
	m_index_buffer_data(nullptr),
	m_vertices_count(0),
	m_indices_count(0),
	m_diffuseTextureHandle(0),
	m_material_uniform_buffer(new GLSLUniformBuffer()),
	m_isTransparent(false)
{}

Mesh::~Mesh()
{
	delete[] m_vertex_buffer_data;
	delete[] m_texture_coord_buffer_data;
	delete[] m_normal_buffer_data;
	delete[] m_color_buffer_data;
	delete[] m_index_buffer_data;
	//m_material_uniform_buffer->~GLSLUniformBuffer();
	m_vao->~VAO();
}

bool Mesh::loadMeshData(const aiMesh* mesh)
{
	m_vertices_count = mesh->mNumVertices;

	m_indices_count  = mesh->mNumFaces*3;//(*mesh)->mFaces->mNumIndices;

	m_materialIndex  = mesh->mMaterialIndex;
	if(mesh->HasPositions())
	{	
		m_vertex_buffer_data  = new GLfloat[m_vertices_count*3];
		
		for(unsigned int i=0; i<m_vertices_count; i++)
		{
			m_vertex_buffer_data[i*3]   = mesh->mVertices[i].x;
			m_vertex_buffer_data[i*3+1] = mesh->mVertices[i].y;
			m_vertex_buffer_data[i*3+2] = mesh->mVertices[i].z;
		}
	} 

	//TODO: Check for correctness.
	if(mesh->HasTextureCoords(0))
	{
		//if(mesh->mNumUVComponents[0] == 2)
		//{
		m_texture_coord_buffer_data  = new GLfloat[m_vertices_count*2];
			for(unsigned int i=0; i<m_vertices_count; i++)
			{
				m_texture_coord_buffer_data[i*2]   = mesh->mTextureCoords[0][i].x;
				m_texture_coord_buffer_data[i*2+1] = mesh->mTextureCoords[0][i].y;
			}
		//}
	}

	if(mesh->HasNormals())
	{
		m_normal_buffer_data = new GLfloat[m_vertices_count*3];
		for(unsigned int i=0; i<m_vertices_count; i++)
		{
			m_normal_buffer_data[i*3]   = mesh->mNormals[i].x;
			m_normal_buffer_data[i*3+1] = mesh->mNormals[i].y;
			m_normal_buffer_data[i*3+2] = mesh->mNormals[i].z;
		}
	}

	//TODO: Check for correctness.
	if(mesh->HasVertexColors(0))
	{
		m_color_buffer_data = new GLfloat[4];

		m_color_buffer_data[0] = mesh->mColors[0]->r;
		m_color_buffer_data[1] = mesh->mColors[0]->g;
		m_color_buffer_data[2] = mesh->mColors[0]->b;
		m_color_buffer_data[3] = mesh->mColors[0]->a;

	}

	if(mesh->mNumFaces > 0)
	{
		m_index_buffer_data = new GLuint[m_indices_count];
		
		for(unsigned int i=0; i<mesh->mNumFaces; i++)
		{
			for(unsigned int j=0; j<mesh->mFaces[i].mNumIndices; j++)
			{
				m_index_buffer_data[i*3+j] = mesh->mFaces[i].mIndices[j];
			}
		}
	}

	mesh->mName.C_Str();

	if(m_vertex_buffer_data == nullptr)
	{
		Logger::GetInstance().Log("Model mesh has not been loaded: %s\n", mesh->mName.C_Str());
		return false;
	}
	else
	{
		return true;
	}
}

void Mesh::initMeshData()
{
	MeshData* mesh = new MeshData( m_vertex_buffer_data,
								m_texture_coord_buffer_data,
								m_normal_buffer_data,
								m_color_buffer_data,
								m_index_buffer_data,
								m_vertices_count, m_indices_count);

	VBO* vbo = new VBO(mesh);
	m_vao = new VAO(vbo);

	m_vao->InitVAO();

	check_gl_error();
}

void Mesh::loadMaterial(aiMaterial* material, GLuint shaderProgHandle)
{
 // Use this piece of code if one model mesh contains more then one texture (i.e. normal map, glossy map, ...).   
 //	  aiString texPath;   //contains filename of texture
 //   if(AI_SUCCESS == mat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath)){
 //           //bind texture
 //           //unsigned int texId = textureIdMap[texPath.data];
 //           //mesh.texIndex = texId;
 //           m_material.texCount = 1;
 //       }
 //   else
 //       m_material.texCount = 0;

	float c[4] = {1.0f, 1.0f, 0.0f, 1.0f};
    aiColor4D diffuse;
	if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
	{
		c[0]=diffuse.r;
		c[1]=diffuse.g;
		c[2]=diffuse.b;
		c[3]=diffuse.a;
	}
    memcpy(m_material.diffuse, c, sizeof(c));
	
	c[0]=0.0;
	c[1]=1.0;
	c[2]=1.0;
	c[3]=1.0;
	aiColor4D ambient;
	if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambient))
	{
		c[0]=ambient.r;
		c[1]=ambient.g;
		c[2]=ambient.b;
		c[3]=ambient.a;
	}
    memcpy(m_material.ambient, c, sizeof(c));

	c[0]=1.0;
	c[1]=0.5;
	c[2]=1.0;
	c[3]=1.0;
	aiColor4D specular;
    if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular))
	{
		c[0]=specular.r;
		c[1]=specular.g;
		c[2]=specular.b;
		c[3]=specular.a;
	}
    memcpy(m_material.specular, c, sizeof(c));

	c[0]=1;
	c[1]=0;
	c[2]=1;
	c[3]=1.0;
	aiColor4D emissive;
    if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &emissive))
	{
		c[0]=emissive.r;
		c[1]=emissive.g;
		c[2]=emissive.b;
		c[3]=emissive.a;
	}
    memcpy(m_material.emissive, c, sizeof(c));

	float shininess = 0.0;
    unsigned int max;
    aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS, &shininess, &max);
    m_material.shininess = shininess;

	m_material_uniform_buffer->getActiveUniformBlock("Material", shaderProgHandle);
	m_material_uniform_buffer->setUniformBuffer((void*)(&m_material), sizeof(m_material));

	check_gl_error();
}

void Mesh::render()
{
	m_vao->RenderVAO();

	check_gl_error();
}