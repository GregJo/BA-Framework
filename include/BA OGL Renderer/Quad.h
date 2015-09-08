#pragma once
#include "../../dependencies/glm/glm.hpp"
#include "VAO.h"
#include "MeshData.h"
#include "GLSLProgram.h"
#include "../../dependencies/glm/glm.hpp"
#include "../../dependencies/glm/gtc/matrix_transform.hpp"

class Quad
{
public:
	Quad();
	Quad(glm::vec3 center_point, glm::vec3 position, float width, float height);
	// Width and height is meant for the viewport
	Quad(glm::vec3 left_top, glm::vec3 left_bottom, glm::vec3 right_bottom, glm::vec3 right_top, glm::vec3 position, unsigned int width,  unsigned int height, unsigned int mask);

	~Quad();

	void initQuad();

	void setTexCoords(GLfloat texCoords[8]);

	void setTexture(std::string texturePath);
	void setTexture(GLuint textureHandle)
	{
		m_texHandle = textureHandle;
	}

	void setGLSLProgram(GLSLProgram& shaderProgram);

	void initDefaultTexture(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);

	void setPosition(glm::vec3 position)
	{	
		m_position = position;
	}

	glm::vec3 getPosition() 
	{ 
		return m_position; 
	} 

	glm::mat4 getWorldMatrix()
	{
		return glm::translate(glm::mat4(1), m_position);
	}

	void render();

	void renderWithAdditionalTextures(std::vector<GLuint> additionalTextureHandles, GLuint samplerHandle);

	class GLSLProgram* getCurrentShaderProgram(){ return m_currentShaderProgram; }

private:

	glm::vec3 m_position;

	GLuint m_width;
	GLuint m_height;

	GLuint m_texHandle;

	GLfloat* m_vertex_buffer_data;
	GLfloat* m_texture_coord_buffer_data;
	GLfloat* m_normal_buffer_data;
	GLfloat* m_color_buffer_data;
	GLuint*  m_index_buffer_data;

	VAO* m_vao;

	GLSLProgram* m_defaultShaderProgram;
	GLSLProgram* m_currentShaderProgram;
};