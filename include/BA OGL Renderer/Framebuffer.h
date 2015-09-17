#pragma once
#include "GraphicsWindow.h"
#include "Quad.h"
#include "..\..\dependencies\gl\include\glew.h"
#include <vector>

class Framebuffer 
{
public:
	Framebuffer();

	Framebuffer(unsigned int width, unsigned int height);

	~Framebuffer();

	void bind();

	void bindForWriting();

	void bindForReading();

	static void unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glDrawBuffer(GL_BACK);
		glReadBuffer(GL_BACK);
	}

	void clean();

	void cleanColorAttachment(unsigned int index, const float clearColor);

	const GLuint	getColorAttachment(int index);
	void			setColorAttachment(int index);

	//const GLuint	getColorAttachment(int index);
	void			setSingleChannelColorAttachment(int index);

	// Frame Buffer depth only.
	const GLuint	getDepthTextureOnly();
	void			setDepthTextureOnly();

	// Use Textures, if you need to display the depth- and/or stencil-buffer, or if you need their values as input in shaders.
	const GLuint	getDepthStencilTexture();
	void			setDepthStencilTexture();

	// Use render buffer objects, if you DON'T need to display the depth- and/or stencil-buffer, or if you DON'T need their values as input in shaders.
	const GLuint	getDepthAttachment();
	void			setDepthAttachment();

	const GLuint	getDepthStencilAttachment();
	void			setDepthStencilAttachment();

	const GLuint	getFBOHandle() { return m_fboHandle; }

	void bindForRenderPass(std::vector<unsigned int> attachmentIndices);
	void bindForDepthPass();

	void debugPass(GraphicsWindow& graphicsWindow, std::vector<unsigned int> attachmentIndices);
	void debugDepthOnlyPass(GraphicsWindow& graphicsWindow, Quad* screenFillingQuad);

private:

	GLuint m_fboHandle;

	GLuint m_depthRBOHandle;
	GLuint m_depthStencilRBOHandle;

	std::vector<GLuint> m_colorTextureHandles;
	GLuint  m_depthStencilTextureHandle;
	GLuint  m_depthTextureOnlyHandle;

	unsigned int m_width;
	unsigned int m_height;

	unsigned int m_numColorAttachments;

};