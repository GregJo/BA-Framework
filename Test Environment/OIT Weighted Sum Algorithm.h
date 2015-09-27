#pragma once
#include "GraphicsWindow.h"
#include "FreeCamera.h"
#include "Quad.h"
#include "Model.h"
#include "Framebuffer.h"
#include "IOIT Algorithms.h"
#include "Smoke Particle System.h"

class OITWeightedSum : public IOITAlgorithm
{
public:
	OITWeightedSum(){}
	~OITWeightedSum(){}

	void VInit(GraphicsWindow* window);

	void setGlobalScale(float globalScale)
	{
		m_globalScale = globalScale;
	}

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

	GLuint m_resultTexture;
	GLuint m_opaqueTextureHandle;

	GLSLProgram* m_weightedSumShader;
	GLSLProgram* m_weightedSumBillboardsShader;
	GLSLProgram* m_weightedSumSmokeParticleSystemShader;

	//Framebuffer* m_frameBuffer;

	std::vector<GLuint> m_additionalTextures;
	std::vector<unsigned int> m_activeColorAttachments;
};