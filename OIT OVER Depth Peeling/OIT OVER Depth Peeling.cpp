//#pragma once
//#include "GraphicsWindow.h"
//#include "GLSLProgram.h"
//#include "FreeCamera.h"
//#include "ModelLoader.h"
//#include "Quad.h"
//#include "Model.h"
//#include "Framebuffer.h"
//#include "OGLErrorHandling.h"
//#include <iostream>
//
//void OITDepthPeelingRender(GraphicsWindow* window, FreeCamera* camera, 
//						   GLuint* depthPeelLayerHandles, std::vector<Model*> models, 
//						   std::vector<Quad*> transparentQuads, std::vector<GLuint> activeColorAttachments_allFBOs, 
//						   std::vector<GLuint> additionalTextures, Framebuffer* frameBufferOpaque, 
//						   Framebuffer* frameBuffer_1, Framebuffer* frameBuffer_2, Framebuffer* frameBuffer_tmp, 
//						   GLSLProgram* depthPeelingShader, GLSLProgram* defaultShader, 
//						   GLuint FBOTexture_1, GLuint FBOTexture_2, 
//						   unsigned int numOfDepthPeelPasses, GLuint sampler)
//{
//	//Framebuffer* frameBuffer_tmp;
//
//	if (numOfDepthPeelPasses>0)
//	{
//		frameBuffer_1->bind();
//		frameBuffer_1->bindForRenderPass(activeColorAttachments_allFBOs);
//			
//		//glEnable(GL_BLEND);
//		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//		glDisable(GL_CULL_FACE);
//		glEnable(GL_DEPTH_TEST);
//		glDepthMask(GL_TRUE);
//		glDepthFunc(GL_LEQUAL);
//		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
//
//		// Blitting the opaque scene depth to propperly depth test the transparen against it.
//		frameBufferOpaque->bindForReading();
//
//		frameBuffer_1->bindForWriting();
//		glBlitFramebuffer(0, 0, window->getWindowWidth(), window->getWindowHeight(), 0, 0, window->getWindowWidth(), window->getWindowHeight(), 
//							GL_DEPTH_BUFFER_BIT, GL_NEAREST);
//
//		frameBuffer_1->bind();
//		frameBuffer_1->bindForRenderPass(activeColorAttachments_allFBOs);
//
//		//defaultShader->use();
//		//model->renderOpaque();
//
//		for (int i = 0; i < models.size(); i++)
//		{
//			models[i]->renderTransparent();
//		}
//
//		for (int i = 0; i < transparentQuads.size(); i++)
//		{
//			transparentQuads[i]->render();
//		}
//
//		frameBuffer_1->unbind();
//
//		// Save initial layer.
//		frameBuffer_1->bindForReading();
//			
//		// Copy FBO color attachment texture to storage texture.
//		glBindTexture(GL_TEXTURE_2D, depthPeelLayerHandles[0]);
//		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, window->getWindowWidth(), window->getWindowHeight());
//
//		frameBuffer_1->unbind();
//
//		// Switch shader for depth peeling
//		depthPeelingShader->use();
//
//		for (int i = 0; i < models.size(); i++)
//		{
//			models[i]->setGLSLProgram(depthPeelingShader);
//		}
//
//		for (int i = 0; i < transparentQuads.size(); i++)
//		{
//			transparentQuads[i]->setGLSLProgram(*depthPeelingShader);
//		}
//
//		GLuint FBOTexture_1 = frameBuffer_1->getColorAttachment(0);
//		GLuint FBOTexture_2 = frameBuffer_1->getColorAttachment(0);
//
//		for (unsigned int i = 0; i < numOfDepthPeelPasses; i++)
//		{
//			if (i%2==0)
//			{
//				FBOTexture_1 = frameBuffer_2->getColorAttachment(0);
//				FBOTexture_2 = frameBuffer_1->getDepthStencilTexture();
//			} else
//			{	
//				FBOTexture_1 = frameBuffer_1->getColorAttachment(0);
//				FBOTexture_2 = frameBuffer_2->getDepthStencilTexture();
//			}
//
//			// Peeling pass.
//			frameBuffer_2->bind();
//			frameBuffer_2->bindForRenderPass(activeColorAttachments_allFBOs);
//			
//			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
//			glEnable(GL_DEPTH_TEST);
//			glDepthMask(GL_TRUE);
//			glDepthFunc(GL_LEQUAL);
//
//			// Blitting the opaque scene depth to propperly depth test the transparen against it.
//			frameBufferOpaque->bindForReading();
//
//			frameBuffer_2->bindForWriting();
//			glBlitFramebuffer(0, 0, window->getWindowWidth(), window->getWindowHeight(), 0, 0, window->getWindowWidth(), window->getWindowHeight(), 
//								GL_DEPTH_BUFFER_BIT, GL_NEAREST);
//
//			frameBuffer_2->bind();
//			frameBuffer_2->bindForRenderPass(activeColorAttachments_allFBOs);
//
//			depthPeelingShader->use();
//			// Render a layer.
//			//model->renderOpaque();
//
//			for (int i = 0; i < models.size(); i++)
//			{
//				models[i]->renderTransparentWithAdditionalTextures(additionalTextures,sampler);
//			}
//
//			for (int i = 0; i < transparentQuads.size(); i++)
//			{
//				transparentQuads[i]->renderWithAdditionalTextures(additionalTextures,sampler);
//			}
//
//			frameBuffer_2->unbind();
//
//			// Save layer.
//			frameBuffer_2->bindForReading();
//			
//			// Copy FBO color attachment texture to storage texture.
//			glBindTexture(GL_TEXTURE_2D, depthPeelLayerHandles[i+1]);
//			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, window->getWindowWidth(), window->getWindowHeight());
//
//			frameBuffer_2->unbind();
//
//			// Ping pong
//			frameBuffer_tmp = frameBuffer_1;
//			frameBuffer_1 = frameBuffer_2;
//			frameBuffer_2 = frameBuffer_tmp;
//			additionalTextures[0] = frameBuffer_1->getDepthStencilTexture();
//		}
//
//		for (int i = 0; i < models.size(); i++)
//		{
//			models[i]->setGLSLProgram(defaultShader);
//		}
//
//		for (int i = 0; i < transparentQuads.size(); i++)
//		{
//			transparentQuads[i]->setGLSLProgram(*defaultShader);
//		}
//
//		// Default pass state.
//		glDisable(GL_BLEND);
//		glDepthMask(GL_TRUE);
//		glEnable(GL_CULL_FACE);
//		glCullFace(GL_BACK);
//		//________________________________________________________________________________________________________________________________________________________________
//	}
//}
//
//void OITDepthPeelingComposite(Framebuffer* frameBufferOpaque, Quad* screenFillingQuad, 
//							  GLuint* depthPeelLayerHandles, unsigned int numOfDepthPeelPasses)
//{
//	glEnable(GL_DEPTH_TEST);
//	glDepthMask(GL_FALSE);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//	screenFillingQuad->getCurrentShaderProgram()->use();
//	screenFillingQuad->setTexture(frameBufferOpaque->getColorAttachment(0));
//	screenFillingQuad->render();
//
//	//screenFillingQuad->getCurrentShaderProgram()->use();
//	//screenFillingQuad->setTexture(frameBufferTransparent->getColorAttachment(0));
//	//screenFillingQuad->render();
//
//	for (unsigned int i = numOfDepthPeelPasses; i >= 0; i--)
//	{
//		screenFillingQuad->getCurrentShaderProgram()->use();
//		screenFillingQuad->setTexture(depthPeelLayerHandles[i]);
//		screenFillingQuad->render();
//	}
//
//	glDisable(GL_BLEND);
//	glEnable(GL_DEPTH_TEST);
//	glDepthMask(GL_TRUE);
//}
//
//void createDepthPeelLayerTextures(std::vector<GLuint>& depthPeelLayerHandleStorage, int numOfDepthPeelLayers, unsigned int width, unsigned int height)
//{
//	for (int i = 0; i < numOfDepthPeelLayers+1; i++) // "+1" because of the initial render result needs to be stored as well.
//	{
//		GLuint textureHandle = 0;
//		glGenTextures(1,&textureHandle);
//
//		glBindTexture(GL_TEXTURE_2D, textureHandle);
//
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//		depthPeelLayerHandleStorage.push_back(textureHandle);
//
//		check_gl_error();
//	}
//}