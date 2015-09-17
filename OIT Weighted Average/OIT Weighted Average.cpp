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

void WeightedAverage(GraphicsWindow* window, FreeCamera* camera, 
					 Framebuffer* opaqueFrameBuffer, Framebuffer* accumFrameBuffer, 
					 std::vector<Model*> models, std::vector<Quad*> quads, GLSLProgram* accumTransparencyShader, GLSLProgram* screenFillingQuadShader,
					 GLSLProgram* weightedAverageShader, GLSLProgram* defaultShader,
					 std::vector<unsigned int> activeColorAttachments, std::vector<unsigned int> activeColorAttachmentsTransparent,
					 std::vector<GLuint> additionalTextureHandles, std::vector<GLuint> opaqueTextureHandle,  
					 Quad* screenFillingQuad, GLuint sampler)
{
	const float clearColor = 0.0f;

	// Blitting the opaque scene depth to propperly depth test the transparen against it.
	opaqueFrameBuffer->bindForReading();

	accumFrameBuffer->bindForWriting();
	glBlitFramebuffer(0, 0, window->getWindowWidth(), window->getWindowHeight(), 0, 0, window->getWindowWidth(), window->getWindowHeight(), 
						GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	opaqueFrameBuffer->unbind();

	//_______________________________________________________________________________________________________________________________________________________________________________
	// Acuumulation pass
	accumTransparencyShader->use();
	accumTransparencyShader->setUniform("VPMatrix", camera->getVPMatrix());

	for (int i = 0; i < models.size(); i++)
	{
		models[i]->setGLSLProgram(accumTransparencyShader);
	}

	for (int i = 0; i < quads.size(); i++)
	{
		quads[i]->setGLSLProgram(*accumTransparencyShader);
	}

	accumFrameBuffer->cleanColorAttachment(0, clearColor);
	accumFrameBuffer->cleanColorAttachment(1, clearColor);
	accumFrameBuffer->bind();
	accumFrameBuffer->bindForRenderPass(activeColorAttachmentsTransparent);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
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

	weightedAverageShader->use();

	glDepthMask(GL_FALSE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

	screenFillingQuad->renderWithAdditionalTextures(additionalTextureHandles,sampler);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	//_______________________________________________________________________________________________________________________________________________________________________________

	// For debug!
	//screenFillingQuad->setGLSLProgram(*screenFillingQuadShader);
	//screenFillingQuad->getCurrentShaderProgram()->use();
	//screenFillingQuad->setTexture(opaqueFrameBuffer->getColorAttachment(0));
	//screenFillingQuad->render();
		
	accumFrameBuffer->clean();
}