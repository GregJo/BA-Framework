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

// Post Process flags for the modelimport.
unsigned int processFlagsOnModelImport = //aiProcess_FlipUVs|
										 aiProcess_Triangulate|
										 aiProcess_JoinIdenticalVertices|
										 aiProcess_SortByPType;

/*
C0: Background texture
Ci: current blended geometry (quad) with premultiplied colors
ai: currend blended geometry alpha

Update 1 (03.09.2015, 13:24): Weighted Sum Blending not working propperly yet. High likely issue with blending negative values (look at paper for 2 possible solutions).
Update 2 (03.09.2015, 23:43): Weighted Sum Blending working propperly now. Really bad results with smoke.
Update 3 (06.09.2015, 13:54): Smoke particle content has too high alpha values, even if it doesn't look like it. Else this method works correctly, if all of the quads alphas add up to one.
							  Else for summed alphas > 1 Weighted Sum method results a negative effect, or overshooting on colors. 

TODO's:
- Capsule this blending method into a fuction, so it can be used more comfortable for comparison (most likely in an extra project in an benchmark/comparisson scene).
*/

int main()
{
	// Create and initialize window.
	setErrorCallbackAndInit(error_callback);
	// GraphicsWindow* window = new GraphicsWindow(1200,800,"Test Window",NULL,NULL,key_callback);
	std::vector<int> targets;
	std::vector<int> hints;

	GraphicsWindow* window = new GraphicsWindow(1200,800,"Test Window",NULL,NULL,key_callback);
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

	// Load Weighted Sum shader for alpha blending.
	GLSLProgram* weightedSumShader = new GLSLProgram();
	weightedSumShader->initShaderProgram("WeightedSumVert.glsl","","","","WeightedSumFrag.glsl");

	// Load screen filling quad shader.
	GLSLProgram* screenFillingQuadShader = new GLSLProgram();
	screenFillingQuadShader->initShaderProgram("screenFillingQuadVert.glsl","","","","screenFillingQuadFrag.glsl");

	//________________________________________________________________________________________________________________________________________________________________________________________
	// Content
	TextureManager::GetInstance().loadTexture("../Content/Textures/Particle_Smoke/smoke_particle_white_base.png");
	TextureManager::GetInstance().loadTexture("../Content/Textures/Particle_Smoke/smoke_particle_grey_base_2.png");
	TextureManager::GetInstance().loadTexture("../Content/Textures/Particle_Smoke/smoke_particle_grey_base_3.png");


	//________________________________________________________________________________________________________________________________________________________________________________________
	// Uniformly depth distributed quads.
	glm::vec3 transform(0,0,0);

	Quad* quad1 = new Quad(transform + glm::vec3(100+100,100+100,0), transform + glm::vec3(100+100,-100+100,0), transform + glm::vec3(-100+100,-100+100,0), transform + glm::vec3(-100+100,100+100,0), glm::vec3(0), 10, 10, 0);
	quad1->initQuad();
	quad1->initDefaultTexture(175,0,0,30);
	quad1->setTexture(TextureManager::GetInstance().getTextureHandle("../Content/Textures/Particle_Smoke/smoke_particle_grey_base_2.png"));

	transform = glm::vec3(-1,5,0);

	Quad* quad2 = new Quad(transform + glm::vec3(100+100,100+100,-100), transform + glm::vec3(100+100,-100+100,-100), transform + glm::vec3(-100+100,-100+100,-100), transform + glm::vec3(-100+100,100+100,-100), glm::vec3(0), 10, 10, 0);
	quad2->initQuad();
	quad2->initDefaultTexture(150,55,0,30);
	quad2->setTexture(TextureManager::GetInstance().getTextureHandle("../Content/Textures/Particle_Smoke/smoke_particle_white_base.png"));
	
	transform = glm::vec3(7,-9,0);

	Quad* quad3 = new Quad(transform + glm::vec3(100+100,100+100,100), transform + glm::vec3(100+100,-100+100,100), transform + glm::vec3(-100+100,-100+100,100), transform + glm::vec3(-100+100,100+100,100), glm::vec3(0), 10, 10, 0);
	quad3->initQuad();
	quad3->initDefaultTexture(150,80,0,30);
	quad3->setTexture(TextureManager::GetInstance().getTextureHandle("../Content/Textures/Particle_Smoke/smoke_particle_grey_base_3.png"));

	transform = glm::vec3(-5,2,0);

	Quad* quad4 = new Quad(transform + glm::vec3(100+100,100+100,-150), transform + glm::vec3(100+100,-100+100,-150), transform + glm::vec3(-100+100,-100+100,-150), transform + glm::vec3(-100+100,100+100,-150), glm::vec3(0), 10, 10, 0);
	quad4->initQuad();
	quad4->initDefaultTexture(0,105,150,30);

	transform = glm::vec3(1,-2,0);

	Quad* quad5 = new Quad(transform + glm::vec3(100+100,100+100,150), transform + glm::vec3(100+100,-100+100,150), transform + glm::vec3(-100+100,-100+100,150), transform + glm::vec3(-100+100,100+100,150), glm::vec3(0), 10, 10, 0);
	quad5->initQuad();
	quad5->initDefaultTexture(255,0,255,30);

	// Arbitarily depth distributed quads.
	transform = glm::vec3(-500,0,0);

	Quad* quad6 = new Quad(transform + glm::vec3(100+100,100+100,0), transform + glm::vec3(100+100,-100+100,0), transform + glm::vec3(-100+100,-100+100,0), transform + glm::vec3(-100+100,100+100,0), glm::vec3(0), 10, 10, 0);
	quad6->initQuad();
	quad6->initDefaultTexture(255,0,0,50);
	//quad6->setTexture(TextureManager::GetInstance().getTextureHandle("../Models/Textures/smoke_particle_1.png"));

	Quad* quad7 = new Quad(transform + glm::vec3(100+100,100+100,-100), transform + glm::vec3(100+100,-100+100,-100), transform + glm::vec3(-100+100,-100+100,-100), transform + glm::vec3(-100+100,100+100,-100), glm::vec3(0), 10, 10, 0);
	quad7->initQuad();
	quad7->initDefaultTexture(0,255,0,50);
	//quad7->setTexture(TextureManager::GetInstance().getTextureHandle("../Models/Textures/smoke_particle_2.png"));
	
	Quad* quad8 = new Quad(transform + glm::vec3(100+100,100+100,100), transform + glm::vec3(100+100,-100+100,100), transform + glm::vec3(-100+100,-100+100,100), transform + glm::vec3(-100+100,100+100,100), glm::vec3(0), 10, 10, 0);
	quad8->initQuad();
	quad8->initDefaultTexture(0,255,0,50);
	//quad8->setTexture(TextureManager::GetInstance().getTextureHandle("../Models/Textures/smoke_particle_3.png"));

	Quad* quad9 = new Quad(transform + glm::vec3(100+100,100+100,-150), transform + glm::vec3(100+100,-100+100,-150), transform + glm::vec3(-100+100,-100+100,-150), transform + glm::vec3(-100+100,100+100,-150), glm::vec3(0), 10, 10, 0);
	quad9->initQuad();
	quad9->initDefaultTexture(0,150,150,50);

	Quad* quad10 = new Quad(transform + glm::vec3(100+100,100+100,150), transform + glm::vec3(100+100,-100+100,150), transform + glm::vec3(-100+100,-100+100,150), transform + glm::vec3(-100+100,100+100,150), glm::vec3(0), 10, 10, 0);
	quad10->initQuad();
	quad10->initDefaultTexture(150,0,150,50);

	ModelLoader* modelImporter = new ModelLoader();
	modelImporter->importModel("../Content/Models/crytek-sponza/sponza.obj", processFlagsOnModelImport);

	// Create and initialize model
	Model* model = new Model(glm::vec3(0,0,0), "../Content/Models/crytek-sponza/");
	model->loadModel(modelImporter->getScene());
	//________________________________________________________________________________________________________________________________________________________________________________________

	// Create framebuffer
	Framebuffer* frameBuffer = new Framebuffer(window->getWindowWidth(),window->getWindowHeight());
	
	std::vector<unsigned int> activeColorAttachments;
	activeColorAttachments.push_back(0);

	frameBuffer->setColorAttachment(0);

	//frameBuffer->setDepthAttachment();
	frameBuffer->setDepthStencilTexture();

	frameBuffer->unbind();

	//// Create and initialize screen filling quad.
	//Quad* screenFillingQuad = new Quad(glm::vec3(-1.0,1.0,0), glm::vec3(-1.0,-1.0,0), glm::vec3(1.0,-1.0,0), glm::vec3(1.0,1.0,0), glm::vec3(0), 0, 0, 0);
	//
	//screenFillingQuad->initQuad();
	//screenFillingQuad->setGLSLProgram(*screenFillingQuadShader);
	//screenFillingQuad->setTexture(frameBuffer->getColorAttachment(0));

	// Background Texture for the weighted sum blending shader
	GLuint backgroundTextureHandle = 0;
	glGenTextures(1,&backgroundTextureHandle);

	glBindTexture(GL_TEXTURE_2D, backgroundTextureHandle);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, window->getWindowWidth(), window->getWindowHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Additional textures that shall be passed to the renderinc function to the model. (In this case the depth texture for the depth peeling pass.)
	std::vector<GLuint> addtionalTextures;
	addtionalTextures.push_back(backgroundTextureHandle);

	// Move this to "GraphicsWindow" 
	glfwSetCursorPos(window->getWindowHandle(), (double) (window->getWindowWidth()/2.0), (double) (window->getWindowHeight()/2.0));
	
	// Move this to "Camera"
	//glClearColor(0.4f,0.6f,0.94f,0.0f);
	glClearColor(0.0f,0.0f,0.0f,0.0f);

	// Sampler
	GLuint sampler = 0;
	glGenSamplers(1, &sampler);

	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);  
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

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
		
		frameBuffer->clean();
		frameBuffer->bind();
		frameBuffer->bindForRenderPass(activeColorAttachments);

		// Update shader uniforms
		model->getCurrentGLSLProgram()->use();
		model->getCurrentGLSLProgram()->setUniform("lightPosition", camera->getCamPos());
		model->getCurrentGLSLProgram()->setUniform("camPosition", camera->getCamPos());
		model->getCurrentGLSLProgram()->setUniform("viewMatrix", camera->getVMatrix());
		model->getCurrentGLSLProgram()->setUniform("normalMatrix", camera->getTranspInvMVMatrix()); // Change this!
		model->getCurrentGLSLProgram()->setUniform("VPMatrix", camera->getVPMatrix());

		model->renderOpaque();

		// Copy background to texture
		glBindTexture(GL_TEXTURE_2D, backgroundTextureHandle);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, window->getWindowWidth(), window->getWindowHeight());

		glDisable(GL_CULL_FACE);

		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER,0.2f);
		
		model->renderTransparentWithAdditionalTextures(addtionalTextures,sampler);
		
		glDisable(GL_ALPHA_TEST);

		// update shader uniforms
		weightedSumShader->use();
		weightedSumShader->setUniform("VPMatrix", camera->getVPMatrix());

		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		quad1->renderWithAdditionalTextures(addtionalTextures,sampler);
		quad2->renderWithAdditionalTextures(addtionalTextures,sampler);
		quad3->renderWithAdditionalTextures(addtionalTextures,sampler);
		//quad4->renderWithAdditionalTextures(addtionalTextures,sampler);
		//quad5->renderWithAdditionalTextures(addtionalTextures,sampler);
		quad6->renderWithAdditionalTextures(addtionalTextures,sampler);
		quad7->renderWithAdditionalTextures(addtionalTextures,sampler);
		quad8->renderWithAdditionalTextures(addtionalTextures,sampler);
		quad9->renderWithAdditionalTextures(addtionalTextures,sampler);
		quad10->renderWithAdditionalTextures(addtionalTextures,sampler);

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

		window->swapBuffers();

		glfwPollEvents();

		check_gl_error();
	}

	glfwTerminate();
	window->release();
	
	return 0;
}