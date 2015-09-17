#pragma once
#include "GraphicsWindow.h"
#include "FreeCamera.h"
#include "Quad.h"
#include "Model.h"
#include "Framebuffer.h"
#include "IOIT Algorithms.h"

class OITWeightedSum : public IOITAlgorithm
{
public:
	OITWeightedSum(){}
	~OITWeightedSum(){}

	void VInit(GraphicsWindow* window);

	void VAlgorithm(FreeCamera* camera, std::vector<Model*> models, std::vector<Quad*> transparentQuads, 
				Framebuffer* frameBufferOpaque, GLSLProgram* defaultShader, 
				GLuint sampler);

	GLuint VGetResultTextureHandle() { return m_resultTexture; }
private:
	unsigned int m_width;
	unsigned int m_height;

	GLuint m_resultTexture;
	GLuint m_opaqueTextureHandle;

	GLSLProgram* m_weightedSumShader;

	//Framebuffer* m_frameBuffer;

	std::vector<GLuint> m_additionalTextures;
	std::vector<unsigned int> m_activeColorAttachments;
};