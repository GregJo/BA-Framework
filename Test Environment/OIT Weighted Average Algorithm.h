#pragma once
#include "GraphicsWindow.h"
#include "FreeCamera.h"
#include "Quad.h"
#include "Model.h"
#include "Framebuffer.h"
#include "IOIT Algorithms.h"

class OITWeightedAverage : public IOITAlgorithm
{
public:
	OITWeightedAverage(){}
	~OITWeightedAverage(){}

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

	GLSLProgram* m_accumTransparencyShader;
	GLSLProgram* m_weightedAverageShader;
	GLSLProgram* m_screenFillingQuadShader;

	Framebuffer* m_accumFrameBuffer;

	Quad* m_compositeScreenFillingQuad;

	std::vector<GLuint> m_additionalTexturesTransparency;
	std::vector<GLuint> m_additionalTexturesOpaque;

	std::vector<unsigned int> m_activeColorAttachments;
};