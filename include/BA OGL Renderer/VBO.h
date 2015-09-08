#pragma once
#include "..\..\dependencies\gl\include\glew.h"
#include "MeshData.h"

class VBO
{
public:
	VBO(MeshData* mesh_Data) : m_mesh_Data(mesh_Data), m_mesh_BO_handle(0), m_created_mesh_BO_handle(false), m_created_mesh_index_BO_handle(false) {}
	~VBO();

	void InitVBO();

	GLuint getIndicesCount(){ return m_mesh_Data->getIndicesCount(); }
	const GLuint* getIndicesData(){ return m_mesh_Data->getMeshIndexDataAsArray(); }

private:
	MeshData* m_mesh_Data;
	GLuint m_mesh_BO_handle;
	GLuint m_mesh_index_BO_handle;
	bool m_created_mesh_BO_handle;
	bool m_created_mesh_index_BO_handle;
};