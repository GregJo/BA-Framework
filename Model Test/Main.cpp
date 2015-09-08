#pragma once
#include "GraphicsWindow.h"
#include "GLSLProgram.h"
#include "FreeCamera.h"
#include "ModelLoader.h"
#include "Quad.h"
#include "Model.h"
#include "OGLErrorHandling.h"
#include <iostream>

// Post Process flags for the modelimport.
unsigned int processFlagsOnModelImport = //aiProcess_FlipUVs|
										 aiProcess_Triangulate|
										 aiProcess_JoinIdenticalVertices|
										 aiProcess_SortByPType;

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
					0.0004f, 3.0f,
					glm::vec3(0,0,-50), 
					glm::vec3(0,1,0), 
					glm::vec3(0,0,0), 
					true);

	camera->setViewport(window->getWindowWidth(),window->getWindowHeight(),0.5f,0.5f);

	Quad* quad1 = new Quad(glm::vec3(100+100,100+100,-1), glm::vec3(100+100,-100+100,-1), glm::vec3(-100+100,-100+100,-1), glm::vec3(-100+100,100+100,-1), glm::vec3(0), 10, 10, 0);
	quad1->initQuad();
	quad1->initDefaultTexture(255,0,0,125);

	Quad* quad2 = new Quad(glm::vec3(100+100,100+100,-20), glm::vec3(100+100,-100+100,-20), glm::vec3(-100+100,-100+100,-20), glm::vec3(-100+100,100+100,-20), glm::vec3(0), 10, 10, 0);
	quad2->initQuad();
	quad2->initDefaultTexture(0,0,255,125);

	ModelLoader* modelImporter = new ModelLoader();
	modelImporter->importModel("../Models/crytek-sponza/sponza.obj", processFlagsOnModelImport);

	// Create and initialize model
	Model* model = new Model(glm::vec3(0,0,0), "../Models/crytek-sponza/");
	model->loadModel(modelImporter->getScene());

	// Move this to "GraphicsWindow" 
	glfwSetCursorPos(window->getWindowHandle(), (double) (window->getWindowWidth()/2.0), (double) (window->getWindowHeight()/2.0));
	
	// Move this to "Camera"
	glClearColor(0.4f,0.6f,0.94f,1.0f);

	check_gl_error();

	// Render Loop
	while(!window->shouldClose())
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		// Update camera
		camera->camControll(window->getWindowHandle());
		camera->update();
		
		// Update shader uniforms
		model->getCurrentGLSLProgram()->use();
		model->getCurrentGLSLProgram()->setUniform("lightPosition", camera->getCamPos());
		model->getCurrentGLSLProgram()->setUniform("camPosition", camera->getCamPos());
		model->getCurrentGLSLProgram()->setUniform("viewMatrix", camera->getVMatrix());
		model->getCurrentGLSLProgram()->setUniform("normalMatrix", camera->getTranspInvMVMatrix()); // Change this!
		model->getCurrentGLSLProgram()->setUniform("VPMatrix", camera->getVPMatrix());

		model->renderOpaque();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);

		model->renderTransparent();
		quad1->render();
		quad2->render();

		glDisable(GL_BLEND);
		//glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		window->swapBuffers();

		glfwPollEvents();

		check_gl_error();
	}

	glfwTerminate();
	window->release();
	
	return 0;
}