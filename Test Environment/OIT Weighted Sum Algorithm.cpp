#pragma once
#include "OIT Weighted Sum Algorithm.h"
#include "OGLErrorHandling.h"


void OITWeightedSum::VInit(GraphicsWindow* window)
{
	m_width = window->getWindowWidth();
	m_height = window->getWindowHeight();

	// Load Depth Peeling shader for the depth peeling reference solution.
	m_weightedSumShader = new GLSLProgram();
	m_weightedSumShader->initShaderProgram("WeightedSumVert.glsl","","","","WeightedSumFrag.glsl");

	m_weightedSumBillboardsShader = new GLSLProgram();
	m_weightedSumBillboardsShader->initShaderProgram("WeightedSumBillboardsVert.glsl","","","","WeightedSumBillboardsFrag.glsl");

	m_weightedSumSmokeParticleSystemShader = new GLSLProgram();
	m_weightedSumSmokeParticleSystemShader->initShaderProgram("WeightedSumSmokeParticleSystemVert.glsl","","","","WeightedSumSmokeParticleSystemFrag.glsl");

	//// Create framebuffer.
	//m_frameBuffer = new Framebuffer(window->getWindowWidth(),window->getWindowHeight());
	//m_frameBuffer->setColorAttachment(0);
	//m_frameBuffer->setDepthStencilTexture();
	//m_frameBuffer->unbind();

	m_activeColorAttachments;
	m_activeColorAttachments.push_back(0);

	m_additionalTextures;
	m_additionalTextures.push_back(m_opaqueTextureHandle);

	glGenTextures(1,&m_resultTexture);

	glBindTexture(GL_TEXTURE_2D, m_resultTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, m_opaqueTextureHandle);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	check_gl_error();
}

void OITWeightedSum::VAlgorithm(FreeCamera* camera, std::vector<Model*> models, std::vector<Quad*> transparentQuads, 
				Framebuffer* frameBufferOpaque, GLSLProgram* defaultShader, 
				GLuint sampler)
{
	frameBufferOpaque->bindForReading();
	
	glBindTexture(GL_TEXTURE_2D, m_opaqueTextureHandle);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_width, m_height);

	frameBufferOpaque->unbind();

	frameBufferOpaque->bind();
	frameBufferOpaque->bindForRenderPass(m_activeColorAttachments);

	glDisable(GL_CULL_FACE);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.0f);
	
	defaultShader->use();
	defaultShader->setUniform("scaleCoeff", m_globalScale);
	defaultShader->setUniform("VPMatrix", camera->getVPMatrix());

	for (unsigned int i = 0; i < models.size(); i++)
	{
		models[i]->renderTransparentWithAdditionalTextures(m_additionalTextures,sampler);
	}
		
	glDisable(GL_ALPHA_TEST);

	m_weightedSumShader->use();
	m_weightedSumShader->setUniform("scaleCoeff", m_globalScale);
	m_weightedSumShader->setUniform("VPMatrix", camera->getVPMatrix());
	m_weightedSumShader->setUniform("windowSize", glm::vec2(m_width,m_height));

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	for (unsigned int i = 0; i < transparentQuads.size(); i++)
	{
		transparentQuads[i]->renderWithAdditionalTextures(m_additionalTextures,sampler);
	}

	frameBufferOpaque->unbind();
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//defaultShader->use();

	frameBufferOpaque->bindForReading();

	glBindTexture(GL_TEXTURE_2D, m_resultTexture);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_width, m_height);

	frameBufferOpaque->unbind();
}

void OITWeightedSum::VAlgorithm(FreeCamera* camera, std::vector<Model*> models, std::vector<Quad*> transparentQuads, std::vector<Quad*> transparentBillboards, 
				Framebuffer* frameBufferOpaque, GLSLProgram* defaultShader, 
				GLuint sampler)
{
	frameBufferOpaque->bindForReading();
	
	glBindTexture(GL_TEXTURE_2D, m_opaqueTextureHandle);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_width, m_height);

	frameBufferOpaque->unbind();

	frameBufferOpaque->bind();
	frameBufferOpaque->bindForRenderPass(m_activeColorAttachments);

	glDisable(GL_CULL_FACE);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.0f);

	defaultShader->use();
	defaultShader->setUniform("scaleCoeff", m_globalScale);
	defaultShader->setUniform("VPMatrix", camera->getVPMatrix());
	
	for (unsigned int i = 0; i < models.size(); i++)
	{
		models[i]->renderTransparentWithAdditionalTextures(m_additionalTextures,sampler);
	}
		
	glDisable(GL_ALPHA_TEST);

	m_weightedSumShader->use();
	m_weightedSumShader->setUniform("scaleCoeff", m_globalScale);
	m_weightedSumShader->setUniform("VPMatrix", camera->getVPMatrix());
	m_weightedSumShader->setUniform("windowSize", glm::vec2(m_width,m_height));

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	for (unsigned int i = 0; i < transparentQuads.size(); i++)
	{
		transparentQuads[i]->renderWithAdditionalTextures(m_additionalTextures,sampler);
	}

	m_weightedSumBillboardsShader->use();
	m_weightedSumBillboardsShader->setUniform("scaleCoeff", m_globalScale);
	m_weightedSumBillboardsShader->setUniform("VPMatrix", camera->getVPMatrix());
	m_weightedSumBillboardsShader->setUniform("worldspaceCameraRightVector", camera->getRightVector());
	m_weightedSumBillboardsShader->setUniform("worldspaceCameraUpVector", camera->getUpVector());
	m_weightedSumBillboardsShader->setUniform("windowSize", glm::vec2(m_width,m_height));

	for (unsigned int i = 0; i < transparentBillboards.size(); i++)
	{
		m_weightedSumBillboardsShader->setUniform("worldspaceParticleCenter", transparentBillboards[i]->getPosition());
		transparentBillboards[i]->renderWithAdditionalTextures(m_additionalTextures,sampler);
	}

	frameBufferOpaque->unbind();
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//defaultShader->use();

	frameBufferOpaque->bindForReading();

	glBindTexture(GL_TEXTURE_2D, m_resultTexture);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_width, m_height);

	frameBufferOpaque->unbind();
}

void OITWeightedSum::VAlgorithm(FreeCamera* camera, std::vector<Model*> models, std::vector<Quad*> transparentQuads, std::vector<SmokeParticleSystem*> smokeParticleSystems, 
								Framebuffer* frameBufferOpaque, GLSLProgram* defaultShader, 
								GLuint sampler)
{
	frameBufferOpaque->bindForReading();
	
	glBindTexture(GL_TEXTURE_2D, m_opaqueTextureHandle);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_width, m_height);

	frameBufferOpaque->unbind();

	frameBufferOpaque->bind();
	frameBufferOpaque->bindForRenderPass(m_activeColorAttachments);

	glDisable(GL_CULL_FACE);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.0f);

	defaultShader->use();
	defaultShader->setUniform("scaleCoeff", m_globalScale);
	defaultShader->setUniform("VPMatrix", camera->getVPMatrix());
	
	for (unsigned int i = 0; i < models.size(); i++)
	{
		models[i]->renderTransparentWithAdditionalTextures(m_additionalTextures,sampler);
	}
		
	glDisable(GL_ALPHA_TEST);

	m_weightedSumShader->use();
	m_weightedSumShader->setUniform("scaleCoeff", m_globalScale);
	m_weightedSumShader->setUniform("VPMatrix", camera->getVPMatrix());
	m_weightedSumShader->setUniform("windowSize", glm::vec2(m_width,m_height));

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	for (unsigned int i = 0; i < transparentQuads.size(); i++)
	{
		transparentQuads[i]->renderWithAdditionalTextures(m_additionalTextures, sampler);
	}

	for (unsigned int i = 0; i < smokeParticleSystems.size(); i++)
	{
		smokeParticleSystems[i]->render(m_weightedSumSmokeParticleSystemShader, camera, m_width, m_height);
	}

	frameBufferOpaque->unbind();
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//defaultShader->use();

	frameBufferOpaque->bindForReading();

	glBindTexture(GL_TEXTURE_2D, m_resultTexture);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_width, m_height);

	frameBufferOpaque->unbind();
}