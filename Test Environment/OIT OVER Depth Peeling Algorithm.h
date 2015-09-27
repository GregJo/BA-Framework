#pragma once
#include "IOIT Algorithms.h"
#include "Smoke Particle System.h"

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

	void setGlobalScale(float globalScale)
	{
		m_globalScale = globalScale;
	}

	// Alternative to the additional numOfDepthPeelLayers parameter.
	void setNumOfDepthPeelingPasses(unsigned int numOfDepthPeelLayers) { m_numOfDepthPeelPasses = numOfDepthPeelLayers; }
	void VInit(GraphicsWindow* window);
	void loadDepthPeelingShaderBillboards();
	void loadDepthPeelingShaderSmokeParticles();
	void VAlgorithm(FreeCamera* camera, std::vector<Model*> models, std::vector<Quad*> transparentQuads, 
					Framebuffer* frameBufferOpaque, GLSLProgram* defaultShader, 
					GLuint sampler);
	void VAlgorithm(FreeCamera* camera, std::vector<Model*> models, std::vector<Quad*> transparentQuads, std::vector<Quad*> transparentBillboards,
					Framebuffer* frameBufferOpaque, GLSLProgram* defaultShader, 
					GLuint sampler);
	void VAlgorithm(FreeCamera* camera, std::vector<Model*> models, std::vector<Quad*> transparentQuads, std::vector<SmokeParticleSystem*> smokeParticleSystems,
					Framebuffer* frameBufferOpaque, GLSLProgram* defaultShader, 
					GLuint sampler);

	GLuint VGetResultTextureHandle() { return m_resultTexture; }

private:
	float m_globalScale;

	unsigned int m_width;
	unsigned int m_height;

	unsigned int m_numOfDepthPeelPasses;

	GLuint m_resultTexture;

	GLSLProgram* m_billboardSmokeParticleShader;

	GLSLProgram* m_depthPeelingShader;
	GLSLProgram* m_depthPeelingShaderBillboards;
	GLSLProgram* m_depthPeelingShaderSmokeParticles;

	Framebuffer* m_frameBufferPing;
	Framebuffer* m_frameBufferPong;

	Quad* m_compositeScreenFillingQuad;

	std::vector<GLuint> m_additionalTextures;
	std::vector<unsigned int> m_activeColorAttachments;
	std::vector<GLuint> m_depthPeelLayerHandles;

	void mf_createDepthPeelLayerTextures(int numOfDepthPeelLayers);
	void mf_composite(Framebuffer* frameBufferOpaque);
};