#pragma once
#include "GLSLUniformBuffer.h"
#include "Logger.h"
#include "OGLErrorHandling.h"

void GLSLUniformBuffer::getActiveUniformBlock(std::string name, GLuint shaderProgHandle)
{
	m_blockIndex = glGetUniformBlockIndex(shaderProgHandle, name.c_str());

	glGetActiveUniformBlockiv(shaderProgHandle, m_blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &m_blockSize);

	check_gl_error();
}

void GLSLUniformBuffer::setUniformBuffer(void* data, const unsigned int dataSizeInBytes)
{
	glGenBuffers( 1, &m_uniformBufferHandle );
	glBindBuffer( GL_UNIFORM_BUFFER, m_uniformBufferHandle );
	glBufferData( GL_UNIFORM_BUFFER, m_blockSize, data, GL_STATIC_DRAW );

	check_gl_error();
}

void GLSLUniformBuffer::bindUniformBuffer()
{
	if(m_blockIndex == GL_INVALID_INDEX)
	{
		Logger::GetInstance().Log("No valid active inactive block.");
		return;
	}

	if(m_uniformBufferHandle != GL_INVALID_VALUE)
		glBindBufferBase( GL_UNIFORM_BUFFER, m_blockIndex, m_uniformBufferHandle );
	else
		Logger::GetInstance().Log("Binding of the uniform buffer unsuccessful.");

	check_gl_error();
}

void GLSLUniformBuffer::unbindUniformBuffer()
{
	glBindBufferBase( GL_UNIFORM_BUFFER, 0, 0 );

	check_gl_error();
}
