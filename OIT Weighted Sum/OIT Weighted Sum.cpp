#include "GraphicsWindow.h"
#include "FreeCamera.h"
#include "Quad.h"
#include "Model.h"
#include "Framebuffer.h"

void OITWeightedSum(GraphicsWindow* window, FreeCamera* camera, 
					Framebuffer* frameBuffer, GLSLProgram* weightedSumShader,
					std::vector<Model*> transparentModels, std::vector<Quad*> transparentQuads, 
					std::vector<GLuint> additionalTextures, GLuint backgroundTextureHandle, 
					GLuint sampler)
{
	// Copy background to texture
	glBindTexture(GL_TEXTURE_2D, backgroundTextureHandle);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, window->getWindowWidth(), window->getWindowHeight());

	glDisable(GL_CULL_FACE);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.2f);
	
	for (int i = 0; i < transparentModels.size(); i++)
	{
		transparentModels[i]->renderTransparentWithAdditionalTextures(additionalTextures,sampler);
	}
		
	glDisable(GL_ALPHA_TEST);

	weightedSumShader->use();

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	for (int i = 0; i < transparentQuads.size(); i++)
	{
		transparentQuads[i]->renderWithAdditionalTextures(additionalTextures,sampler);
	}

	frameBuffer->unbind();
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//screenFillingQuad->getCurrentShaderProgram()->use();
	//screenFillingQuad->render();

	frameBuffer->bindForReading();

	glBlitFramebuffer(0, 0, window->getWindowWidth(), window->getWindowHeight(),
						0, 0, window->getWindowWidth(), window->getWindowHeight(), 
						GL_COLOR_BUFFER_BIT, GL_NEAREST);

	frameBuffer->unbind();
}