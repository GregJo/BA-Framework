#pragma once
#include "..\..\dependencies\gl\include\glew.h"

class Sampler
{
public:
	Sampler();
	~Sampler();

	void initSampler(GLint wrapS, GLint wrapT, GLint minFilter, GLint maxFilter);

	void bindSampler(unsigned int bindingLocation);

	void unbindSampler();

private:
	GLuint m_sampler;
};