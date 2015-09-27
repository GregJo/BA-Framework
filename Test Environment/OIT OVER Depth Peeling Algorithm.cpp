#pragma once
#include "OIT OVER Depth Peeling Algorithm.h"
#include "OGLErrorHandling.h"

void OIT_OVER_DepthPeeling::mf_createDepthPeelLayerTextures(int numOfDepthPeelLayers)
{
	for (int i = 0; i < numOfDepthPeelLayers+1; i++) // "+1" because of the initial render result needs to be stored as well.
	{
		GLuint textureHandle = 0;
		glGenTextures(1,&textureHandle);

		glBindTexture(GL_TEXTURE_2D, textureHandle);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		m_depthPeelLayerHandles.push_back(textureHandle);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	check_gl_error();
}

void OIT_OVER_DepthPeeling::VInit(GraphicsWindow* window)
{
	m_globalScale = 1.0f;
	m_width = window->getWindowWidth();
	m_height = window->getWindowHeight();
	mf_createDepthPeelLayerTextures(m_numOfDepthPeelPasses);

	// Load Depth Peeling shader for the depth peeling reference solution.
	m_billboardSmokeParticleShader = new GLSLProgram();
	m_billboardSmokeParticleShader->initShaderProgram("BillboardSmokeParticleVert.glsl","","","","BillboardSmokeParticleFrag.glsl");

	// Load Depth Peeling shader for the depth peeling reference solution.
	m_depthPeelingShader = new GLSLProgram();
	m_depthPeelingShader->initShaderProgram("DepthPeelingVert.glsl","","","","DepthPeelingFrag.glsl");

	// Load screen filling quad shader.
	GLSLProgram* screenFillingQuadShader = new GLSLProgram();
	screenFillingQuadShader->initShaderProgram("screenFillingQuadVert.glsl","","","","screenFillingQuadFrag.glsl");

	loadDepthPeelingShaderBillboards();
	loadDepthPeelingShaderSmokeParticles();

	// Create depth only framebuffers for depth peeling ping ponging.
	m_frameBufferPing = new Framebuffer(window->getWindowWidth(),window->getWindowHeight());
	m_frameBufferPing->setColorAttachment(0);
	m_frameBufferPing->setDepthStencilTexture();
	m_frameBufferPing->unbind();

	m_frameBufferPong = new Framebuffer(window->getWindowWidth(),window->getWindowHeight());
	m_frameBufferPong->setColorAttachment(0);
	m_frameBufferPong->setDepthStencilTexture();
	m_frameBufferPong->unbind();

	m_activeColorAttachments;
	m_activeColorAttachments.push_back(0);

	m_additionalTextures;
	m_additionalTextures.push_back(m_frameBufferPing->getDepthStencilTexture());

	// Create screen filling quad.
	m_compositeScreenFillingQuad = new Quad(glm::vec3(-1.0,1.0,0), glm::vec3(-1.0,-1.0,0), glm::vec3(1.0,-1.0,0), glm::vec3(1.0,1.0,0), glm::vec3(0), 0, 0, 0);
	
	m_compositeScreenFillingQuad->initQuad();
	m_compositeScreenFillingQuad->setGLSLProgram(*screenFillingQuadShader);
	m_compositeScreenFillingQuad->setTexture(m_frameBufferPing->getColorAttachment(0));

	glGenTextures(1,&m_resultTexture);

	glBindTexture(GL_TEXTURE_2D, m_resultTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	check_gl_error();
}

void OIT_OVER_DepthPeeling::loadDepthPeelingShaderBillboards()
{
	// Load Depth Peeling shader for the depth peeling reference solution.
	m_depthPeelingShaderBillboards = new GLSLProgram();
	m_depthPeelingShaderBillboards->initShaderProgram("DepthPeelingBillboardsVert.glsl","","","","DepthPeelingFrag.glsl");
}

void OIT_OVER_DepthPeeling::loadDepthPeelingShaderSmokeParticles()
{
	// Load Depth Peeling shader for the depth peeling reference solution.
	m_depthPeelingShaderSmokeParticles = new GLSLProgram();
	m_depthPeelingShaderSmokeParticles->initShaderProgram("DepthPeelingSmokeParticleSystemVert.glsl","","","","DepthPeelingSmokeParticleSystemFrag.glsl");
}

void OIT_OVER_DepthPeeling::VAlgorithm(FreeCamera* camera, std::vector<Model*> models, std::vector<Quad*> transparentQuads, 
										Framebuffer* frameBufferOpaque, GLSLProgram* defaultShader, 
										GLuint sampler)
{
	Framebuffer* frameBuffer_tmp;

	if (m_numOfDepthPeelPasses>0)
	{
		m_frameBufferPing->bind();
		m_frameBufferPing->bindForRenderPass(m_activeColorAttachments);
			
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// Blitting the opaque scene depth to propperly depth test the transparen against it.
		frameBufferOpaque->bindForReading();

		m_frameBufferPing->bindForWriting();
		glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, 
							GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		frameBufferOpaque->unbind();

		m_frameBufferPing->bind();
		m_frameBufferPing->bindForRenderPass(m_activeColorAttachments);

		defaultShader->use();
		defaultShader->setUniform("scaleCoeff",m_globalScale);
		for (unsigned int i = 0; i < models.size(); i++)
		{
			models[i]->renderTransparent();
		}

		for (unsigned int i = 0; i < transparentQuads.size(); i++)
		{
			transparentQuads[i]->render();
		}

		m_frameBufferPing->unbind();

		// Save initial layer.
		m_frameBufferPing->bindForReading();
			
		// Copy FBO color attachment texture to storage texture.
		glBindTexture(GL_TEXTURE_2D, m_depthPeelLayerHandles[0]);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_width, m_height);

		m_frameBufferPing->unbind();

		// Switch shader for depth peeling
		m_depthPeelingShader->use();

		m_depthPeelingShader->setUniform("scaleCoeff", m_globalScale);
		m_depthPeelingShader->setUniform("VPMatrix", camera->getVPMatrix());
		m_depthPeelingShader->setUniform("windowSize", glm::vec2(m_width,m_height));

		for (unsigned int i = 0; i < models.size(); i++)
		{
			models[i]->setGLSLProgram(m_depthPeelingShader);
		}

		for (unsigned int i = 0; i < transparentQuads.size(); i++)
		{
			transparentQuads[i]->setGLSLProgram(*m_depthPeelingShader);
		}

		GLuint texturePing = m_frameBufferPing->getColorAttachment(0);
		GLuint texturePong = m_frameBufferPing->getColorAttachment(0);

		for (unsigned int i = 0; i < m_numOfDepthPeelPasses; i++)
		{
			if (i%2==0)
			{
				texturePing = m_frameBufferPong->getColorAttachment(0);
				texturePong = m_frameBufferPing->getDepthStencilTexture();
			} else
			{	
				texturePing = m_frameBufferPing->getColorAttachment(0);
				texturePong = m_frameBufferPong->getDepthStencilTexture();
			}

			// Peeling pass.
			m_frameBufferPong->bind();
			m_frameBufferPong->bindForRenderPass(m_activeColorAttachments);
			
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_LEQUAL);

			// Blitting the opaque scene depth to propperly depth test the transparen against it.
			frameBufferOpaque->bindForReading();

			m_frameBufferPong->bindForWriting();
			glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, 
								GL_DEPTH_BUFFER_BIT, GL_NEAREST);

			frameBufferOpaque->unbind();

			m_frameBufferPong->bind();
			m_frameBufferPong->bindForRenderPass(m_activeColorAttachments);

			m_depthPeelingShader->use();

			// Render a layer.
			for (unsigned int j = 0; j < models.size(); j++)
			{
				models[j]->renderTransparentWithAdditionalTextures(m_additionalTextures,sampler);
			}

			for (unsigned int j = 0; j < transparentQuads.size(); j++)
			{
				transparentQuads[j]->renderWithAdditionalTextures(m_additionalTextures,sampler);
			}

			m_frameBufferPong->unbind();

			// Save layer.
			m_frameBufferPong->bindForReading();
			
			// Copy FBO color attachment texture to storage texture.
			glBindTexture(GL_TEXTURE_2D, m_depthPeelLayerHandles[i+1]);
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_width, m_height);

			m_frameBufferPong->unbind();

			// Ping pong
			frameBuffer_tmp = m_frameBufferPing;
			m_frameBufferPing = m_frameBufferPong;
			m_frameBufferPong = frameBuffer_tmp;
			m_additionalTextures[0] = m_frameBufferPing->getDepthStencilTexture();
		}

		for (unsigned int i = 0; i < models.size(); i++)
		{
			models[i]->setGLSLProgram(defaultShader);
		}

		for (unsigned int i = 0; i < transparentQuads.size(); i++)
		{
			transparentQuads[i]->setGLSLProgram(*defaultShader);
		}

		// Default pass state.
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		//________________________________________________________________________________________________________________________________________________________________
	}

	m_frameBufferPing->bind();
	m_frameBufferPing->clean();
	m_frameBufferPing->unbind();

	m_frameBufferPong->bind();
	m_frameBufferPong->clean();
	m_frameBufferPong->unbind();

	mf_composite(frameBufferOpaque);

	check_gl_error();
}

void OIT_OVER_DepthPeeling::VAlgorithm(FreeCamera* camera, std::vector<Model*> models, std::vector<Quad*> transparentQuads, std::vector<Quad*> transparentBillboards,
										Framebuffer* frameBufferOpaque, GLSLProgram* defaultShader, 
										GLuint sampler)
{
	Framebuffer* frameBuffer_tmp;

	if (m_numOfDepthPeelPasses>0)
	{
		m_frameBufferPing->bind();
		m_frameBufferPing->bindForRenderPass(m_activeColorAttachments);
			
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// Blitting the opaque scene depth to propperly depth test the transparen against it.
		frameBufferOpaque->bindForReading();

		m_frameBufferPing->bindForWriting();
		glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, 
							GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		m_frameBufferPing->bind();
		m_frameBufferPing->bindForRenderPass(m_activeColorAttachments);

		for (unsigned int i = 0; i < models.size(); i++)
		{
			models[i]->renderTransparent();
		}

		for (unsigned int i = 0; i < transparentQuads.size(); i++)
		{
			transparentQuads[i]->render();
		}

		transparentBillboards[0]->getCurrentShaderProgram()->use();

		transparentBillboards[0]->getCurrentShaderProgram()->setUniform("scaleCoeff", m_globalScale);
		transparentBillboards[0]->getCurrentShaderProgram()->setUniform("VPMatrix", camera->getVPMatrix());
		transparentBillboards[0]->getCurrentShaderProgram()->setUniform("worldspaceCameraRightVector", camera->getRightVector());
		transparentBillboards[0]->getCurrentShaderProgram()->setUniform("worldspaceCameraUpVector", camera->getUpVector());
		transparentBillboards[0]->getCurrentShaderProgram()->setUniform("VPMatrix", camera->getVPMatrix());
		transparentBillboards[0]->getCurrentShaderProgram()->setUniform("windowSize", glm::vec2(m_width,m_height));
		for (unsigned int i = 0; i < transparentBillboards.size(); i++)
		{
			transparentBillboards[i]->getCurrentShaderProgram()->setUniform("worldspaceParticleCenter", transparentBillboards[i]->getPosition());
			transparentBillboards[i]->render();
		}
		defaultShader->use();
		m_frameBufferPing->unbind();

		// Save initial layer.
		m_frameBufferPing->bindForReading();
			
		// Copy FBO color attachment texture to storage texture.
		glBindTexture(GL_TEXTURE_2D, m_depthPeelLayerHandles[0]);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_width, m_height);

		m_frameBufferPing->unbind();

		// Switch shader for depth peeling
		m_depthPeelingShader->use();

		m_depthPeelingShader->setUniform("scaleCoeff", m_globalScale);
		m_depthPeelingShader->setUniform("VPMatrix", camera->getVPMatrix());
		m_depthPeelingShader->setUniform("windowSize", glm::vec2(m_width,m_height));

		m_depthPeelingShaderBillboards->use();

		m_depthPeelingShaderBillboards->setUniform("scaleCoeff", m_globalScale);
		m_depthPeelingShaderBillboards->setUniform("VPMatrix", camera->getVPMatrix());
		//m_depthPeelingShaderBillboards->setUniform("worldspaceParticleCenter", glm::vec3(500,200,0));
		m_depthPeelingShaderBillboards->setUniform("worldspaceCameraRightVector", camera->getRightVector());
		m_depthPeelingShaderBillboards->setUniform("worldspaceCameraUpVector", camera->getUpVector());
		m_depthPeelingShaderBillboards->setUniform("VPMatrix", camera->getVPMatrix());
		m_depthPeelingShaderBillboards->setUniform("windowSize", glm::vec2(m_width,m_height));

		for (unsigned int i = 0; i < models.size(); i++)
		{
			models[i]->setGLSLProgram(m_depthPeelingShader);
		}

		for (unsigned int i = 0; i < transparentQuads.size(); i++)
		{
			transparentQuads[i]->setGLSLProgram(*m_depthPeelingShader);
		}

		GLSLProgram* tmpBillboardShader = transparentBillboards[0]->getCurrentShaderProgram();

		for (unsigned int i = 0; i < transparentBillboards.size(); i++)
		{
			transparentBillboards[i]->setGLSLProgram(*m_depthPeelingShaderBillboards);
		}

		GLuint texturePing = m_frameBufferPing->getColorAttachment(0);
		GLuint texturePong = m_frameBufferPing->getColorAttachment(0);

		for (unsigned int i = 0; i < m_numOfDepthPeelPasses; i++)
		{
			if (i%2==0)
			{
				texturePing = m_frameBufferPong->getColorAttachment(0);
				texturePong = m_frameBufferPing->getDepthStencilTexture();
			} else
			{	
				texturePing = m_frameBufferPing->getColorAttachment(0);
				texturePong = m_frameBufferPong->getDepthStencilTexture();
			}

			// Peeling pass.
			m_frameBufferPong->bind();
			m_frameBufferPong->bindForRenderPass(m_activeColorAttachments);
			
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_LEQUAL);

			// Blitting the opaque scene depth to propperly depth test the transparen against it.
			frameBufferOpaque->bindForReading();

			m_frameBufferPong->bindForWriting();
			glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, 
								GL_DEPTH_BUFFER_BIT, GL_NEAREST);

			m_frameBufferPong->bind();
			m_frameBufferPong->bindForRenderPass(m_activeColorAttachments);

			m_depthPeelingShader->use();

			// Render a layer.
			for (unsigned int j = 0; j < models.size(); j++)
			{
				models[j]->renderTransparentWithAdditionalTextures(m_additionalTextures,sampler);
			}

			for (unsigned int j = 0; j < transparentQuads.size(); j++)
			{
				transparentQuads[j]->renderWithAdditionalTextures(m_additionalTextures,sampler);
			}

			m_depthPeelingShaderBillboards->use();

			for (unsigned int i = 0; i < transparentBillboards.size(); i++)
			{
				m_depthPeelingShaderBillboards->setUniform("worldspaceParticleCenter", transparentBillboards[i]->getPosition());
				transparentBillboards[i]->renderWithAdditionalTextures(m_additionalTextures,sampler);
			}

			m_frameBufferPong->unbind();

			// Save layer.
			m_frameBufferPong->bindForReading();
			
			// Copy FBO color attachment texture to storage texture.
			glBindTexture(GL_TEXTURE_2D, m_depthPeelLayerHandles[i+1]);
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_width, m_height);

			m_frameBufferPong->unbind();

			// Ping pong
			frameBuffer_tmp = m_frameBufferPing;
			m_frameBufferPing = m_frameBufferPong;
			m_frameBufferPong = frameBuffer_tmp;
			m_additionalTextures[0] = m_frameBufferPing->getDepthStencilTexture();
		}

		for (unsigned int i = 0; i < models.size(); i++)
		{
			models[i]->setGLSLProgram(defaultShader);
		}

		for (unsigned int i = 0; i < transparentQuads.size(); i++)
		{
			transparentQuads[i]->setGLSLProgram(*defaultShader);
		}

		for (unsigned int i = 0; i < transparentBillboards.size(); i++)
		{
			transparentBillboards[i]->setGLSLProgram(*tmpBillboardShader);
		}

		// Default pass state.
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		//________________________________________________________________________________________________________________________________________________________________
	}

	m_frameBufferPing->bind();
	m_frameBufferPing->clean();
	m_frameBufferPing->unbind();

	m_frameBufferPong->bind();
	m_frameBufferPong->clean();
	m_frameBufferPong->unbind();

	mf_composite(frameBufferOpaque);

	check_gl_error();
}

void OIT_OVER_DepthPeeling::VAlgorithm(FreeCamera* camera, std::vector<Model*> models, std::vector<Quad*> transparentQuads, std::vector<SmokeParticleSystem*> smokeParticleSystems,
										Framebuffer* frameBufferOpaque, GLSLProgram* defaultShader, 
										GLuint sampler)
{
	Framebuffer* frameBuffer_tmp;

	if (m_numOfDepthPeelPasses>0)
	{
		m_frameBufferPing->bind();
		m_frameBufferPing->bindForRenderPass(m_activeColorAttachments);
			
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// Blitting the opaque scene depth to propperly depth test the transparen against it.
		frameBufferOpaque->bindForReading();

		m_frameBufferPing->bindForWriting();
		glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, 
							GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		m_frameBufferPing->bind();
		m_frameBufferPing->bindForRenderPass(m_activeColorAttachments);

		for (unsigned int i = 0; i < models.size(); i++)
		{
			models[i]->renderTransparent();
		}

		for (unsigned int i = 0; i < transparentQuads.size(); i++)
		{
			transparentQuads[i]->render();
		}

		//m_billboardSmokeParticleShader->use();

		//m_billboardSmokeParticleShader->setUniform("VPMatrix", camera->getVPMatrix());
		//m_billboardSmokeParticleShader->setUniform("worldspaceCameraRightVector", camera->getRightVector());
		//m_billboardSmokeParticleShader->setUniform("worldspaceCameraUpVector", camera->getUpVector());
		//m_billboardSmokeParticleShader->setUniform("VPMatrix", camera->getVPMatrix());
		//m_billboardSmokeParticleShader->setUniform("windowSize", glm::vec2(m_width,m_height));
		for (unsigned int i = 0; i < smokeParticleSystems.size(); i++)
		{
			//m_billboardSmokeParticleShader->setUniform("worldspaceParticleCenter", transparentBillboards[i]->getPosition());
			smokeParticleSystems[i]->render(m_billboardSmokeParticleShader,camera,m_width,m_height);
		}
		defaultShader->use();
		m_frameBufferPing->unbind();

		// Save initial layer.
		m_frameBufferPing->bindForReading();
			
		// Copy FBO color attachment texture to storage texture.
		glBindTexture(GL_TEXTURE_2D, m_depthPeelLayerHandles[0]);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_width, m_height);

		m_frameBufferPing->unbind();

		// Switch shader for depth peeling
		m_depthPeelingShader->use();

		m_depthPeelingShader->setUniform("scaleCoeff", m_globalScale);
		m_depthPeelingShader->setUniform("VPMatrix", camera->getVPMatrix());
		m_depthPeelingShader->setUniform("windowSize", glm::vec2(m_width,m_height));

		m_depthPeelingShaderBillboards->use();

		//m_depthPeelingShaderBillboards->setUniform("VPMatrix", camera->getVPMatrix());
		////m_depthPeelingShaderBillboards->setUniform("worldspaceParticleCenter", glm::vec3(500,200,0));
		//m_depthPeelingShaderBillboards->setUniform("worldspaceCameraRightVector", camera->getRightVector());
		//m_depthPeelingShaderBillboards->setUniform("worldspaceCameraUpVector", camera->getUpVector());
		//m_depthPeelingShaderBillboards->setUniform("VPMatrix", camera->getVPMatrix());
		//m_depthPeelingShaderBillboards->setUniform("windowSize", glm::vec2(m_width,m_height));

		for (unsigned int i = 0; i < models.size(); i++)
		{
			models[i]->setGLSLProgram(m_depthPeelingShader);
		}

		for (unsigned int i = 0; i < transparentQuads.size(); i++)
		{
			transparentQuads[i]->setGLSLProgram(*m_depthPeelingShader);
		}

		//GLSLProgram* tmpBillboardShader = transparentBillboards[0]->getCurrentShaderProgram();

		//for (unsigned int i = 0; i < transparentBillboards.size(); i++)
		//{
		//	transparentBillboards[i]->setGLSLProgram(*m_depthPeelingShaderBillboards);
		//}

		GLuint texturePing = m_frameBufferPing->getColorAttachment(0);
		GLuint texturePong = m_frameBufferPing->getColorAttachment(0);

		for (unsigned int i = 0; i < m_numOfDepthPeelPasses; i++)
		{
			if (i%2==0)
			{
				texturePing = m_frameBufferPong->getColorAttachment(0);
				texturePong = m_frameBufferPing->getDepthStencilTexture();
			} else
			{	
				texturePing = m_frameBufferPing->getColorAttachment(0);
				texturePong = m_frameBufferPong->getDepthStencilTexture();
			}

			// Peeling pass.
			m_frameBufferPong->bind();
			m_frameBufferPong->bindForRenderPass(m_activeColorAttachments);
			
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_LEQUAL);

			// Blitting the opaque scene depth to propperly depth test the transparen against it.
			frameBufferOpaque->bindForReading();

			m_frameBufferPong->bindForWriting();
			glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, 
								GL_DEPTH_BUFFER_BIT, GL_NEAREST);

			m_frameBufferPong->bind();
			m_frameBufferPong->bindForRenderPass(m_activeColorAttachments);

			m_depthPeelingShader->use();

			// Render a layer.
			for (unsigned int j = 0; j < models.size(); j++)
			{
				models[j]->renderTransparentWithAdditionalTextures(m_additionalTextures,sampler);
			}

			for (unsigned int j = 0; j < transparentQuads.size(); j++)
			{
				transparentQuads[j]->renderWithAdditionalTextures(m_additionalTextures,sampler);
			}

			//m_depthPeelingShaderBillboards->use();

			for (unsigned int i = 0; i < smokeParticleSystems.size(); i++)
			{
				//m_depthPeelingShaderBillboards->setUniform("worldspaceParticleCenter", transparentBillboards[i]->getPosition());
				smokeParticleSystems[i]->render(m_depthPeelingShaderSmokeParticles,camera,m_width,m_height,m_additionalTextures,sampler);
			}

			m_frameBufferPong->unbind();

			// Save layer.
			m_frameBufferPong->bindForReading();
			
			// Copy FBO color attachment texture to storage texture.
			glBindTexture(GL_TEXTURE_2D, m_depthPeelLayerHandles[i+1]);
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_width, m_height);

			m_frameBufferPong->unbind();

			// Ping pong
			frameBuffer_tmp = m_frameBufferPing;
			m_frameBufferPing = m_frameBufferPong;
			m_frameBufferPong = frameBuffer_tmp;
			m_additionalTextures[0] = m_frameBufferPing->getDepthStencilTexture();
		}

		for (unsigned int i = 0; i < models.size(); i++)
		{
			models[i]->setGLSLProgram(defaultShader);
		}

		for (unsigned int i = 0; i < transparentQuads.size(); i++)
		{
			transparentQuads[i]->setGLSLProgram(*defaultShader);
		}

		//for (unsigned int i = 0; i < transparentBillboards.size(); i++)
		//{
		//	transparentBillboards[i]->setGLSLProgram(*tmpBillboardShader);
		//}

		// Default pass state.
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		//________________________________________________________________________________________________________________________________________________________________
	}

	m_frameBufferPing->bind();
	m_frameBufferPing->clean();
	m_frameBufferPing->unbind();

	m_frameBufferPong->bind();
	m_frameBufferPong->clean();
	m_frameBufferPong->unbind();

	mf_composite(frameBufferOpaque);

	check_gl_error();
}

void OIT_OVER_DepthPeeling::mf_composite(Framebuffer* frameBufferOpaque)
{
	glClear(GL_COLOR_BUFFER_BIT);//|GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_compositeScreenFillingQuad->getCurrentShaderProgram()->use();
	m_compositeScreenFillingQuad->setTexture(frameBufferOpaque->getColorAttachment(0));
	m_compositeScreenFillingQuad->render();

	for (int i = m_numOfDepthPeelPasses; i >= 0; i--)
	{
		m_compositeScreenFillingQuad->getCurrentShaderProgram()->use();
		m_compositeScreenFillingQuad->setTexture(m_depthPeelLayerHandles[i]);
		m_compositeScreenFillingQuad->render();
	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	// Copy the final image.
	// Copy default FBO color attachment texture to result texture.
	glBindTexture(GL_TEXTURE_2D, m_resultTexture);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_width, m_height);
	glBindTexture(GL_TEXTURE_2D, 0);

	//m_resultTexture = frameBufferOpaque->getColorAttachment(0);

	//m_frameBufferPing->bindForWriting();
	//glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	check_gl_error();
}