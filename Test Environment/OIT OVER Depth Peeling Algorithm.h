#pragma once
#include "IOIT Algorithms.h"

class OIT_OVER_DepthPeeling : public IOITAlgorithm
{
public:
	OIT_OVER_DepthPeeling(){}
	~OIT_OVER_DepthPeeling()
	{
		m_depthPeelingShader->~GLSLProgram();

		m_frameBufferPing->~Framebuffer();
		m_frameBufferPong->~Framebuffer();

		m_compositeScreenFillingQuad->~Quad();
	}

	// Alternative to the additional numOfDepthPeelLayers parameter.
	void setNumOfDepthPeelingPasses(unsigned int numOfDepthPeelLayers) { m_numOfDepthPeelPasses = numOfDepthPeelLayers; }
	void VInit(GraphicsWindow* window);
	void VAlgorithm(FreeCamera* camera, std::vector<Model*> models, std::vector<Quad*> transparentQuads, 
					Framebuffer* frameBufferOpaque, GLSLProgram* defaultShader, 
					GLuint sampler);

	GLuint VGetResultTextureHandle() { return m_resultTexture; }

private:
	unsigned int m_width;
	unsigned int m_height;

	unsigned int m_numOfDepthPeelPasses;

	GLuint m_resultTexture;

	GLSLProgram* m_depthPeelingShader;

	Framebuffer* m_frameBufferPing;
	Framebuffer* m_frameBufferPong;

	Quad* m_compositeScreenFillingQuad;

	std::vector<GLuint> m_additionalTextures;
	std::vector<unsigned int> m_activeColorAttachments;
	std::vector<GLuint> m_depthPeelLayerHandles;

	void mf_createDepthPeelLayerTextures(int numOfDepthPeelLayers);
	void mf_composite(Framebuffer* frameBufferOpaque);
};