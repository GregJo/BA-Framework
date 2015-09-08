#pragma once
#include <string>
#include <vector>
#include "..\..\dependencies\gl\include\glew.h"

//Errors to cover:
class GLSLUniformBuffer
{
public:
	GLSLUniformBuffer(): m_uniformBufferHandle(0),
						 m_blockIndex(0),
						 m_blockSize(0)
	{}
	~GLSLUniformBuffer(){}

	void getActiveUniformBlock(std::string name, GLuint shaderProgHandle);
	//Maybe an extra Parameter for usage (GL_STATIC_DRAW, ...), or another method?
	void setUniformBuffer(void* data, const unsigned int dataSizeInBytes);

	void bindUniformBuffer();
	void unbindUniformBuffer();

private:
	//void*  m_data;
	GLuint m_uniformBufferHandle;
	GLuint m_blockIndex;
	GLint  m_blockSize;
};