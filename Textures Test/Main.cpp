#pragma once
#include "GraphicsWindow.h"
#include "GLSLProgram.h"
#include "FreeCamera.h"
#include "Quad.h"
#include "Framebuffer.h"
#include <iostream>

/*
TODO: Test glGetTexImage(...) in this project
*/

int main()
{
	// Create and initialize window.
	setErrorCallbackAndInit(error_callback);
	GraphicsWindow* window = new GraphicsWindow(1200,800,"Test Window",NULL,NULL,key_callback);
	window->makeContextCurrent();

	// Initalize glew.
	initGlew();

	// Create and initialize Camera
	FreeCamera* camera = new FreeCamera(45.0f, 16.0f/9.0f, 
					0.0f, 0.0f, 
					0.1f, 10000.0f, 
					0.0004f, 0.35f,
					glm::vec3(0,0,-50), 
					glm::vec3(0,1,0), 
					glm::vec3(0,0,0), 
					true);

	camera->setViewport(window->getWindowWidth(),window->getWindowHeight(),0.5f,0.5f);

	GLSLProgram* screenFillingQuadShader = new GLSLProgram();
	screenFillingQuadShader->initShaderProgram("screenFillingQuadVert.glsl","","","","screenFillingQuadFrag.glsl");

	// Create and initialize Quad
	Quad* quad = new Quad(glm::vec3(10,-1,10), glm::vec3(10,-1,-10), glm::vec3(-10,-1,-10), glm::vec3(-10,-1,10), glm::vec3(0), 10, 10, 0);
	//Quad* quad = new Quad(glm::vec3(-1.0,0.0,0), glm::vec3(-1.0,-1.0,0), glm::vec3(1.0,-1.0,0), glm::vec3(1.0,1.0,0), glm::vec3(0), 0, 0, 0);
	quad->initQuad();
	//quad->initDefaultTexture(255,255,255,255);
	quad->setTexture("../Content/Textures/Skybox Texture/Above_The_Sea.jpg");

	// Create and initialize a screen filling quad.
	Quad* screenFillingQuad = new Quad(glm::vec3(-1.0,1.0,0), glm::vec3(-1.0,-1.0,0), glm::vec3(1.0,-1.0,0), glm::vec3(1.0,1.0,0), glm::vec3(0), 0, 0, 0);
	
	screenFillingQuad->initQuad();
	screenFillingQuad->setGLSLProgram(*screenFillingQuadShader);

	// Framebuffer for opaque meshes.
	Framebuffer* frameBufferOpaque = new Framebuffer(window->getWindowWidth(),window->getWindowHeight());
	std::vector<unsigned int> activeColorAttachments;
	activeColorAttachments.push_back(0);
	frameBufferOpaque->setColorAttachment(0);
	frameBufferOpaque->setDepthStencilTexture();
	frameBufferOpaque->unbind();

	GLuint textureHandle = 0;
	glGenTextures(1,&textureHandle);

	glBindTexture(GL_TEXTURE_2D, textureHandle);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, window->getWindowWidth(), window->getWindowHeight(), 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

	// Move this to "GraphicsWindow" 
	glfwSetCursorPos(window->getWindowHandle(), (double) (window->getWindowWidth()/2.0), (double) (window->getWindowHeight()/2.0));
	
	// Move this to "Camera"
	glClearColor(0.4f,0.6f,0.94f,1.0f);

	// Render Loop
	while(!window->shouldClose())
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// Update camera
		camera->camControll(window->getWindowHandle());
		camera->update();
		
		// Update shader uniforms
		quad->getCurrentShaderProgram()->use();
		quad->getCurrentShaderProgram()->setUniform("VPMatrix", camera->getVPMatrix());

		frameBufferOpaque->clean();
		frameBufferOpaque->bind();
		frameBufferOpaque->bindForRenderPass(activeColorAttachments);

		quad->render();

		frameBufferOpaque->unbind();

		//screenFillingQuad->getCurrentShaderProgram()->use();
		screenFillingQuadShader->use();
		screenFillingQuad->setTexture(frameBufferOpaque->getColorAttachment(0));

		screenFillingQuad->render();

		frameBufferOpaque->bindForReading();

		glBindTexture(GL_TEXTURE_2D, textureHandle);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, window->getWindowWidth(), window->getWindowHeight());
		glBindTexture(GL_TEXTURE_2D, 0);

		frameBufferOpaque->unbind();

		// Anzahl der Textureunits ermitteln
		int numberOfTextureUnits = 0;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &numberOfTextureUnits);
		GLuint lastTextureUnit = GL_TEXTURE0 + (numberOfTextureUnits - 1);
 
		// letzte texture unit binden, damit man mit der aktuellen textur arbeiten kann
		glActiveTexture(lastTextureUnit);
 
		// Kopiere textur in ein array:
		// ============================================================
		glBindTexture(GL_TEXTURE_2D, textureHandle);
 
		GLint width;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
 
		GLint height;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
 
		float* pixels = new float[(int)width * (int)height * 3];
 
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, pixels);

		glBindTexture(GL_TEXTURE_2D, 0);

		float squaredMeanError = 0;

		for (int i = 0; i < (int)width * (int)height * 3; i++)
		{
			//squaredMeanError += std::pow((1.0f/3.0f*pixels[i]),2.0f);
			squaredMeanError += std::pow((pixels[i]),2.0f);
		}

		squaredMeanError /= ((int)width * (int)height * 3);

		std::cout << "Squared Mean Error: " << squaredMeanError << std::endl;

		delete[] pixels;

		glActiveTexture(GL_TEXTURE0);

		window->swapBuffers();

		glfwPollEvents();
	}

	glfwTerminate();
	window->release();
	
	return 0;
}