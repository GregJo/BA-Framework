#pragma once
#include "GraphicsWindow.h"
#include "GLSLProgram.h"
#include "FreeCamera.h"
#include "Quad.h"

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

	// Create and initialize Quad
	Quad* quad = new Quad(glm::vec3(10,-1,10), glm::vec3(10,-1,-10), glm::vec3(-10,-1,-10), glm::vec3(-10,-1,10), glm::vec3(0), 10, 10, 0);
	//Quad* quad = new Quad(glm::vec3(-1.0,0.0,0), glm::vec3(-1.0,-1.0,0), glm::vec3(1.0,-1.0,0), glm::vec3(1.0,1.0,0), glm::vec3(0), 0, 0, 0);
	quad->initQuad();
	quad->initDefaultTexture(255,255,255,255);
	quad->setTexture("../Models/Textures/smoke_particle.png");

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
		quad->getCurrentShaderProgram()->setUniform("lightPosition", camera->getCamPos());
		quad->getCurrentShaderProgram()->setUniform("camPosition", camera->getCamPos());
		quad->getCurrentShaderProgram()->setUniform("viewMatrix", camera->getVMatrix());
		quad->getCurrentShaderProgram()->setUniform("normalMatrix", glm::mat4(1)); // Change this!
		quad->getCurrentShaderProgram()->setUniform("VPMatrix", camera->getVPMatrix());
		quad->render();

		window->swapBuffers();

		glfwPollEvents();
	}

	glfwTerminate();
	window->release();
	
	return 0;
}