#pragma once
#include "GraphicsWindow.h"
#include "GLSLProgram.h"
#include "FreeCamera.h"
#include "ModelLoader.h"
#include "Quad.h"
#include "Model.h"
#include "Framebuffer.h"
#include "TextureManager.h"
#include "OGLErrorHandling.h"
#include <iostream>


void OITNewCoverage(FreeCamera* camera,
					std::vector<Model*> models,
					std::vector<Quad*> quads,
					std::vector<GLuint> activeColorAttachmentsTransparent,
					std::vector<GLuint> opaqueTextureHandle,
					std::vector<GLuint> additionalTextureHandles,
					Quad* screenFillingQuad,
					Framebuffer* accumFrameBuffer,
					Framebuffer* opaqueFrameBuffer,
					GLSLProgram* accumTransparencyRevealageShader,
					GLSLProgram* newOITCoverageShader,
					GLSLProgram* screenFillingQuadShader,
					GLSLProgram* defaultShader,
					GLuint sampler)
{
	const float clearColorWhite = 0.0f;

	//_______________________________________________________________________________________________________________________________________________________________________________
	// Acuumulation pass
	accumTransparencyRevealageShader->use();
	accumTransparencyRevealageShader->setUniform("VPMatrix", camera->getVPMatrix());

	for (int i = 0; i < models.size(); i++)
	{
		models[i]->setGLSLProgram(accumTransparencyRevealageShader);
	}

	for (int i = 0; i < quads.size(); i++)
	{
		quads[i]->setGLSLProgram(*accumTransparencyRevealageShader);
	}

	accumFrameBuffer->clean();
	accumFrameBuffer->bind();
	accumFrameBuffer->bindForRenderPass(activeColorAttachmentsTransparent);
	accumFrameBuffer->cleanColorAttachment(1,clearColorWhite);

	opaqueFrameBuffer->unbind();

	// Blitting the opaque scene depth to propperly depth test the transparen against it.
	opaqueFrameBuffer->bindForReading();

	accumFrameBuffer->bindForWriting();
	//glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, 
	//					GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	opaqueFrameBuffer->unbind();

	accumFrameBuffer->bind();
	accumFrameBuffer->bindForRenderPass(activeColorAttachmentsTransparent);

	glEnable(GL_BLEND);
	glBlendFunci(0, GL_ONE, GL_ONE);
	glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	for (int i = 0; i < models.size(); i++)
	{
		models[i]->renderTransparent();
	}

	for (int i = 0; i < quads.size(); i++)
	{
		quads[i]->render();
	}

	accumFrameBuffer->unbind();

	for (int i = 0; i < models.size(); i++)
	{
		models[i]->setGLSLProgram(defaultShader);
	}

	for (int i = 0; i < quads.size(); i++)
	{
		quads[i]->setGLSLProgram(*defaultShader);
	}

	glDisable(GL_BLEND);
	//glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//_______________________________________________________________________________________________________________________________________________________________________________
	// Final compositing pass

	screenFillingQuad->setGLSLProgram(*screenFillingQuadShader);
	screenFillingQuad->getCurrentShaderProgram()->use();
	screenFillingQuad->renderWithAdditionalTextures(opaqueTextureHandle,sampler);

	newOITCoverageShader->use();

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

	screenFillingQuad->renderWithAdditionalTextures(additionalTextureHandles,sampler);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	//_______________________________________________________________________________________________________________________________________________________________________________

}