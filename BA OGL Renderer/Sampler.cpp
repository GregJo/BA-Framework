#pragma once
#include "Sampler.h"
#include "OGLErrorHandling.h"


Sampler::Sampler()
{
	m_sampler = 0;
	glGenSamplers(1, &m_sampler);

	check_gl_error();
}
Sampler::~Sampler()
{
	glDeleteSamplers(1, &m_sampler); 
		
	check_gl_error();
}

void Sampler::initSampler(GLint wrapS, GLint wrapT, GLint minFilter, GLint maxFilter) 
{
	glSamplerParameteri(m_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);  
	glSamplerParameteri(m_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(m_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(m_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	check_gl_error();
}

void Sampler::bindSampler(unsigned int bindingLocation)
{
	glBindSampler(bindingLocation, m_sampler);
	glActiveTexture(GL_TEXTURE0 + bindingLocation);

	check_gl_error();
}

void Sampler::unbindSampler()
{
	glBindSampler(GL_TEXTURE0, 0);
	glActiveTexture(GL_TEXTURE0);
}