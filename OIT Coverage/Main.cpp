#pragma once
#include "GraphicsWindow.h"
#include "GLSLProgram.h"
#include "FreeCamera.h"
#include "ModelLoader.h"
#include "Quad.h"
#include "Model.h"
#include "TextureManager.h"
#include "OGLErrorHandling.h"
#include <iostream>

// Post Process flags for the modelimport.
unsigned int processFlagsOnModelImport = //aiProcess_FlipUVs|
										 aiProcess_Triangulate|
										 aiProcess_JoinIdenticalVertices|
										 aiProcess_SortByPType;

/*
Need the Coverage mask, aswell as an alpha coverage mask (not sure yet, wether of the size the pow(multisamplesCount,2), or of the size of the complete multisample 
coverage mask (size of the display window (currently vec2(1200,800)))).
Implementation will be shader based, for complete control, over what will be used and multiplied. Maybe OpenGL based implementation, if I'm totally aware how it works.
(shader based implementation might also be necessary for the weighted sum and the weighted average algorithms)

Current problem: Alpha value that is used to calculate the coverage is also used to blend the samples -> wrong result, quads become even more transparent.

A solution may be found at the super bible, forcing alpha to "1" before blending by glEnable(GL_SAMPLE_ALPHA_TO_ONE), but then how will the alpha mask be created, which 
is normally done by glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE), assuming that everything should be done in one pass.

Update 1 (02.09.2015): OpenGL version is working now! (completely textured quads won't blend (->limitation, or/xor/and OVER assumption violated?))

Content and its propperties, needed to test correctness of result: 
 - arbitarily/uniformly distributed in depth,
 - dense/sparse textures,
 - smoke(/fire) particle system
 - intersecting
*/

int main()
{
	// Create and initialize window.
	setErrorCallbackAndInit(error_callback);
	//GraphicsWindow* window = new GraphicsWindow(1200,800,"Test Window",NULL,NULL,key_callback);
	std::vector<int> targets;
	std::vector<int> hints;
	targets.push_back(GLFW_SAMPLES);
	hints.push_back(16);
	GraphicsWindow* window = new GraphicsWindow(1200,800,"Test Window",NULL,NULL,key_callback,targets,hints);
	window->makeContextCurrent();

	// Initalize glew.
	initGlew();

	// Create and initialize Camera
	FreeCamera* camera = new FreeCamera(45.0f, 16.0f/9.0f, 
					0.0f, 0.0f, 
					0.1f, 5000.0f, 
					0.0004f, 3.0f,
					glm::vec3(0,0,-50), 
					glm::vec3(0,1,0), 
					glm::vec3(0,0,0), 
					true);

	camera->setViewport(window->getWindowWidth(),window->getWindowHeight(),0.5f,0.5f);

	//________________________________________________________________________________________________________________________________________________________________________________________
	// Content
	TextureManager::GetInstance().loadTexture("../Content/Textures/Particle_Smoke/smoke_particle_grey_base_2.png");
	TextureManager::GetInstance().loadTexture("../Content/Textures/Particle_Smoke/smoke_particle_grey_base_3.png");
	TextureManager::GetInstance().loadTexture("../Content/Textures/Particle_Smoke/smoke_particle_cyan_base.png");

	//________________________________________________________________________________________________________________________________________________________________________________________
	// Uniformly depth distributed quads.
	Quad* quad1 = new Quad(glm::vec3(100+100,100+100,0), glm::vec3(100+100,-100+100,0), glm::vec3(-100+100,-100+100,0), glm::vec3(-100+100,100+100,0), glm::vec3(0), 10, 10, 0);
	quad1->initQuad();
	quad1->initDefaultTexture(255,0,0,100);
	quad1->setTexture(TextureManager::GetInstance().getTextureHandle("../Content/Textures/Particle_Smoke/smoke_particle_grey_base_2.png"));

	Quad* quad2 = new Quad(glm::vec3(100+100,100+100,-10), glm::vec3(100+100,-100+100,-10), glm::vec3(-100+100,-100+100,-10), glm::vec3(-100+100,100+100,-10), glm::vec3(0), 10, 10, 0);
	quad2->initQuad();
	quad2->initDefaultTexture(0,255,0,100);
	quad2->setTexture(TextureManager::GetInstance().getTextureHandle("../Content/Textures/Particle_Smoke/smoke_particle_grey_base_3.png"));
	
	Quad* quad3 = new Quad(glm::vec3(100+100,100+100,10), glm::vec3(100+100,-100+100,10), glm::vec3(-100+100,-100+100,10), glm::vec3(-100+100,100+100,10), glm::vec3(0), 10, 10, 0);
	quad3->initQuad();
	quad3->initDefaultTexture(0,255,0,150);
	quad3->setTexture(TextureManager::GetInstance().getTextureHandle("../Content/Textures/Particle_Smoke/smoke_particle_cyan_base.png"));

	Quad* quad4 = new Quad(glm::vec3(100+100,100+100,-20), glm::vec3(100+100,-100+100,-20), glm::vec3(-100+100,-100+100,-20), glm::vec3(-100+100,100+100,-20), glm::vec3(0), 10, 10, 0);
	quad4->initQuad();
	quad4->initDefaultTexture(0,150,150,150);

	Quad* quad5 = new Quad(glm::vec3(100+100,100+100,20), glm::vec3(100+100,-100+100,20), glm::vec3(-100+100,-100+100,20), glm::vec3(-100+100,100+100,20), glm::vec3(0), 10, 10, 0);
	quad5->initQuad();
	quad5->initDefaultTexture(150,0,150,150);

	// Arbitarily depth distributed quads.
	glm::vec3 transform(-500,0,0);

	Quad* quad6 = new Quad(transform + glm::vec3(100+100,100+100,0), transform + glm::vec3(100+100,-100+100,0), transform + glm::vec3(-100+100,-100+100,0), transform + glm::vec3(-100+100,100+100,0), glm::vec3(0), 10, 10, 0);
	quad6->initQuad();
	quad6->initDefaultTexture(255,0,0,100);
	quad6->setTexture(TextureManager::GetInstance().getTextureHandle("../Content/Textures/Particle_Smoke/smoke_particle_cyan_base.png"));

	Quad* quad7 = new Quad(transform + glm::vec3(100+100,100+100,-100), transform + glm::vec3(100+100,-100+100,-100), transform + glm::vec3(-100+100,-100+100,-100), transform + glm::vec3(-100+100,100+100,-100), glm::vec3(0), 10, 10, 0);
	quad7->initQuad();
	quad7->initDefaultTexture(0,255,0,100);
	quad7->setTexture(TextureManager::GetInstance().getTextureHandle("../Content/Textures/Particle_Smoke/smoke_particle_cyan_base.png"));
	
	Quad* quad8 = new Quad(transform + glm::vec3(100+100,100+100,10), transform + glm::vec3(100+100,-100+100,10), transform + glm::vec3(-100+100,-100+100,10), transform + glm::vec3(-100+100,100+100,10), glm::vec3(0), 10, 10, 0);
	quad8->initQuad();
	quad8->initDefaultTexture(0,255,0,150);
	quad8->setTexture(TextureManager::GetInstance().getTextureHandle("../Content/Textures/Particle_Smoke/smoke_particle_cyan_base.png"));

	Quad* quad9 = new Quad(transform + glm::vec3(100+100,100+100,-20), transform + glm::vec3(100+100,-100+100,-20), transform + glm::vec3(-100+100,-100+100,-20), transform + glm::vec3(-100+100,100+100,-20), glm::vec3(0), 10, 10, 0);
	quad9->initQuad();
	quad9->initDefaultTexture(0,150,150,150);

	Quad* quad10 = new Quad(transform + glm::vec3(100+100,100+100,150), transform + glm::vec3(100+100,-100+100,150), transform + glm::vec3(-100+100,-100+100,150), transform + glm::vec3(-100+100,100+100,150), glm::vec3(0), 10, 10, 0);
	quad10->initQuad();
	quad10->initDefaultTexture(150,0,150,150);

	//________________________________________________________________________________________________________________________________________________________________________________________

	ModelLoader* modelImporter = new ModelLoader();
	modelImporter->importModel("../Content/Models/crytek-sponza/sponza.obj", processFlagsOnModelImport);

	// Create and initialize model
	Model* model = new Model(glm::vec3(0,0,0), "../Content/Models/crytek-sponza/");
	model->loadModel(modelImporter->getScene());

	// Move this to "GraphicsWindow" 
	glfwSetCursorPos(window->getWindowHandle(), (double) (window->getWindowWidth()/2.0), (double) (window->getWindowHeight()/2.0));
	
	// Move this to "Camera"
	glClearColor(0.4f,0.6f,0.94f,0.0f);
	//glClearColor(0.0f,0.0f,0.0f,0.0f);

	check_gl_error();

	// Render Loop
	while(!window->shouldClose())
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glEnable(GL_MULTISAMPLE);
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

		glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

		glDisable(GL_CULL_FACE);

		model->renderTransparent();

		quad1->render();
		quad2->render();
		quad3->render();
		//quad4->render();
		//quad5->render();
		quad6->render();
		quad7->render();
		quad8->render();
		//quad9->render();
		//quad10->render();

		glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
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