#pragma once
#include "Framebuffer.h"
#include "..\Core\Logger.h"
#include "OGLErrorHandling.h"


Framebuffer::Framebuffer() : m_numColorAttachments(0), m_depthRBOHandle(0), m_depthStencilRBOHandle(0), m_depthStencilTextureHandle(0), m_fboHandle(0)
{
	// Generate and bind the framebuffer 
	glGenFramebuffers(1, &m_fboHandle); 
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboHandle);
	check_gl_error();
}

Framebuffer::Framebuffer(unsigned int width, unsigned int height) : m_width(width), m_height(height), m_numColorAttachments(0), m_depthRBOHandle(0), m_depthStencilRBOHandle(0), m_depthStencilTextureHandle(0), m_fboHandle(0)
{
	// Generate and bind the framebuffer 
	glGenFramebuffers(1, &m_fboHandle); 
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboHandle);
	check_gl_error();
}

Framebuffer::~Framebuffer()
{

}

void Framebuffer::bind() 
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboHandle);
	check_gl_error();
}

void Framebuffer::bindForWriting()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fboHandle);
	check_gl_error();
}

void Framebuffer::bindForReading()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fboHandle);
	check_gl_error();
}

void Framebuffer::clean()
{
	bind();
	if(m_colorTextureHandles.size() != 0)
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}
	if (m_depthRBOHandle != GL_INVALID_VALUE || m_depthTextureOnlyHandle != GL_INVALID_VALUE)
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	} else if (m_depthStencilRBOHandle != GL_INVALID_VALUE || m_depthStencilTextureHandle != GL_INVALID_VALUE)
	{
		glClear(GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	}
	unbind();
	check_gl_error();
}

void Framebuffer::cleanColorAttachment(unsigned int index, const float clearColor)
{
	if(index > m_numColorAttachments)
	{
		Logger::GetInstance().Log("Color attachment index out of bounds!");
		return;
	}

	//glDrawBuffer(GL_COLOR_ATTACHMENT0+index);
	glClearBufferfv(GL_COLOR, index, &clearColor);
	check_gl_error();
}

const GLuint	Framebuffer::getColorAttachment(int index)
{
	return m_colorTextureHandles[index];
}
void			Framebuffer::setColorAttachment(int index)
{
	GLuint textureHandle = 0;
	glGenTextures(1,&textureHandle);
	m_colorTextureHandles.push_back(textureHandle);

	glBindTexture(GL_TEXTURE_2D, m_colorTextureHandles[index]);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, m_colorTextureHandles[index], 0);

	m_numColorAttachments++;

	check_gl_error();
}

void			Framebuffer::setSingleChannelColorAttachment(int index)
{
	GLuint textureHandle = 0;
	glGenTextures(1,&textureHandle);
	m_colorTextureHandles.push_back(textureHandle);

	glBindTexture(GL_TEXTURE_2D, m_colorTextureHandles[index]);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, m_width, m_height, 0, GL_R, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, m_colorTextureHandles[index], 0);

	m_numColorAttachments++;

	check_gl_error();
}

// Frame Buffer depth only.
const GLuint	Framebuffer::getDepthTextureOnly()
{
	return m_depthTextureOnlyHandle;
}
void			Framebuffer::setDepthTextureOnly()
{
	glGenTextures(1, &m_depthTextureOnlyHandle);
	glBindTexture(GL_TEXTURE_2D, m_depthTextureOnlyHandle);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTextureOnlyHandle, 0);

	check_gl_error();
}

// Use Textures, if you need to display the depth- and/or stencil-buffer, or if you need their values as input in shaders.
const GLuint	Framebuffer::getDepthStencilTexture()
{
	return m_depthStencilTextureHandle;
}
void			Framebuffer::setDepthStencilTexture()
{
	glGenTextures(1, &m_depthStencilTextureHandle);

	glBindTexture(GL_TEXTURE_2D, m_depthStencilTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, m_width, m_height, 0, GL_DEPTH_STENCIL, 
					GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthStencilTextureHandle, 0);

	check_gl_error();
}

// Use render buffer objects, if you DON'T need to display the depth- and/or stencil-buffer, or if you DON'T need their values as input in shaders.
const GLuint	Framebuffer::getDepthAttachment()
{
	return m_depthRBOHandle;
}
void			Framebuffer::setDepthAttachment()
{
	glGenRenderbuffers(1, &m_depthRBOHandle);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthRBOHandle);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRBOHandle);
	
	check_gl_error();
}

const GLuint	Framebuffer::getDepthStencilAttachment()
{
	return m_depthStencilRBOHandle;
}
void			Framebuffer::setDepthStencilAttachment()
{
	glGenRenderbuffers(1, &m_depthStencilRBOHandle);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilRBOHandle);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthStencilRBOHandle);

	check_gl_error();
}

void Framebuffer::bindForRenderPass(std::vector<unsigned int> attachmentIndices)
{
	GLenum* drawBuffers = new GLenum[attachmentIndices.size()];

	for (unsigned int i = 0; i < attachmentIndices.size(); i++)
	{
		drawBuffers[i] = GL_COLOR_ATTACHMENT0+i;
	}

	glDrawBuffers(attachmentIndices.size(), drawBuffers);

	check_gl_error();
}

void Framebuffer::bindForDepthPass()
{
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	check_gl_error();
}

void Framebuffer::debugPass(GraphicsWindow& graphicsWindow, std::vector<unsigned int> attachmentIndices)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	bindForReading();

	unsigned int windowWidth = graphicsWindow.getWindowWidth();
	unsigned int windowHeight = graphicsWindow.getWindowHeight();

	GLsizei oneThirdWidth = (GLsizei)(windowWidth / 3.0f);
	GLsizei oneThirdHeight = (GLsizei)(windowHeight / 3.0f);

	if (attachmentIndices.size() > 8)
	{
		Logger::GetInstance().Log("Color attachment count out of bounds!");
		return;
	}

	for (unsigned int i = 0; i < attachmentIndices.size(); i++)
	{
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndices[i]);

		glBlitFramebuffer(0, 0, windowWidth, windowHeight,
			oneThirdWidth*((i)%3), oneThirdHeight*((i)/3), oneThirdWidth*(((i)%3)+1), oneThirdHeight*(((i+3))/3), GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}

	check_gl_error();
}

// Error here!
void Framebuffer::debugDepthOnlyPass(GraphicsWindow& graphicsWindow, Quad* screenFillingQuad)
{

}