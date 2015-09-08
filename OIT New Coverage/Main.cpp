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
Two textures:
- accumTexture: accumulate all transparent materials (quads) as well as their alphas, by additive blending in the first pass.
- revealageTexture: accumulate background one minus alpha blending over all layers

Update 1 (08.09.2015, 00:06): New coverage method is working propperly now.

TODO's:
- Capsule this blending method into a fuction, so it can be used more comfortable for comparison (most likely in an extra project in an benchmark/comparisson scene).
- Add propper depth test to transparent layer rendering. Currently the depth buffer textures of the additional frame buffers, where transparent layers are rendered to, 
  are empty at start and thus have no depth information of the opaque scene.  
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
					0.0004f, 3.0f,
					glm::vec3(0,0,-50), 
					glm::vec3(0,1,0), 
					glm::vec3(0,0,0), 
					true);

	camera->setViewport(window->getWindowWidth(),window->getWindowHeight(),0.5f,0.5f);

	// Load accumulation shader for accumulating all the transparent matrials, as well as their alphas.
	GLSLProgram* defaultShader = new GLSLProgram();
	defaultShader->initShaderProgram("Vert.glsl","","","","Frag.glsl");

	// Load screen filling quad shader.
	GLSLProgram* screenFillingQuadShader = new GLSLProgram();
	screenFillingQuadShader->initShaderProgram("screenFillingQuadVert.glsl","","","","screenFillingQuadFrag.glsl");

	// Load accumulation shader for accumulating all the transparent matrials, as well as their alphas.
	GLSLProgram* accumTransparencyRevealageShader = new GLSLProgram();
	accumTransparencyRevealageShader->initShaderProgram("AccumTransparencyRevealageVert.glsl","","","","AccumTransparencyRevealageFrag.glsl");

	// Load Weighted Average shader, which will be used for final compositing (a variation of screen filling quad shader using multiple textures).
	GLSLProgram* newOITCoverageShader = new GLSLProgram();
	newOITCoverageShader->initShaderProgram("NewOITCoverageVert.glsl","","","","NewOITCoverageFrag.glsl");

	// Create screen filling quad.
	Quad* screenFillingQuad = new Quad(glm::vec3(-1.0,1.0,0), glm::vec3(-1.0,-1.0,0), glm::vec3(1.0,-1.0,0), glm::vec3(1.0,1.0,0), glm::vec3(0), 0, 0, 0);
	
	screenFillingQuad->initQuad();
	screenFillingQuad->setGLSLProgram(*newOITCoverageShader);

	TextureManager::GetInstance().loadTexture("../Content/Textures/Particle_Smoke/smoke_particle_red_base.png");
	TextureManager::GetInstance().loadTexture("../Content/Textures/Particle_Smoke/smoke_particle_grey_base_2.png");
	TextureManager::GetInstance().loadTexture("../Content/Textures/Particle_Smoke/smoke_particle_grey_base_3.png");

	Quad* quad1 = new Quad(glm::vec3(100+100,100+100,-1), glm::vec3(100+100,-100+100,-1), glm::vec3(-100+100,-100+100,-1), glm::vec3(-100+100,100+100,-1), glm::vec3(0), 10, 10, 0);
	quad1->initQuad();
	quad1->initDefaultTexture(255,0,0,89);
	quad1->setTexture(TextureManager::GetInstance().getTextureHandle("../Content/Textures/Particle_Smoke/smoke_particle_grey_base_3.png"));

	Quad* quad2 = new Quad(glm::vec3(100+100,100+100,-20), glm::vec3(100+100,-100+100,-20), glm::vec3(-100+100,-100+100,-20), glm::vec3(-100+100,100+100,-20), glm::vec3(0), 10, 10, 0);
	quad2->initQuad();
	quad2->initDefaultTexture(0,255,0,89);
	quad2->setTexture(TextureManager::GetInstance().getTextureHandle("../Content/Textures/Particle_Smoke/smoke_particle_grey_base_2.png"));

	Quad* quad3 = new Quad(glm::vec3(100+100,100+100,20), glm::vec3(100+100,-100+100,20), glm::vec3(-100+100,-100+100,20), glm::vec3(-100+100,100+100,20), glm::vec3(0), 10, 10, 0);
	quad3->initQuad();
	quad3->initDefaultTexture(0,0,255,89);
	quad3->setTexture(TextureManager::GetInstance().getTextureHandle("../Content/Textures/Particle_Smoke/smoke_particle_red_base.png"));

	Quad* quad4 = new Quad(glm::vec3(100+100,100+100,-40), glm::vec3(100+100,-100+100,-40), glm::vec3(-100+100,-100+100,-40), glm::vec3(-100+100,100+100,-40), glm::vec3(0), 10, 10, 0);
	quad4->initQuad();
	quad4->initDefaultTexture(255,0,127,50);

	Quad* quad5 = new Quad(glm::vec3(100+100,100+100,40), glm::vec3(100+100,-100+100,40), glm::vec3(-100+100,-100+100,40), glm::vec3(-100+100,100+100,40), glm::vec3(0), 10, 10, 0);
	quad5->initQuad();
	quad5->initDefaultTexture(255,127,0,50);

	ModelLoader* modelImporter = new ModelLoader();
	modelImporter->importModel("../Content/Models/crytek-sponza/sponza.obj", processFlagsOnModelImport);

	// Create and initialize model
	Model* model = new Model(glm::vec3(0,0,0), "../Content/Models/crytek-sponza/");
	model->loadModel(modelImporter->getScene());

	// Create framebuffer
	Framebuffer* accumFrameBuffer = new Framebuffer(window->getWindowWidth(),window->getWindowHeight());
	
	std::vector<unsigned int> activeColorAttachments;
	activeColorAttachments.push_back(0);
	activeColorAttachments.push_back(1);

	accumFrameBuffer->setColorAttachment(0);
	accumFrameBuffer->setColorAttachment(1);

	//frameBuffer->setDepthAttachment();
	accumFrameBuffer->setDepthStencilTexture();

	accumFrameBuffer->unbind();

	// Additional textures to pass for the second transparency render pass.
	std::vector<GLuint> additionalTextureHandles;
	additionalTextureHandles.push_back(accumFrameBuffer->getColorAttachment(0));
	additionalTextureHandles.push_back(accumFrameBuffer->getColorAttachment(1));

	// Move this to "GraphicsWindow" 
	glfwSetCursorPos(window->getWindowHandle(), (double) (window->getWindowWidth()/2.0), (double) (window->getWindowHeight()/2.0));
	
	// Move this to "Camera"
	//glClearColor(0.4f,0.6f,0.94f,0.0f);
	glClearColor(0.0f,0.0f,0.0f,0.0f);

	const float clearColor = 1.0f;

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
		glClearColor(0.0f,0.0f,0.0f,0.0f);
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
		//quad4->render();
		//quad5->render();

		//_______________________________________________________________________________________________________________________________________________________________________________
		// Acuumulation pass
		accumTransparencyRevealageShader->use();
		accumTransparencyRevealageShader->setUniform("VPMatrix", camera->getVPMatrix());

		model->setGLSLProgram(accumTransparencyRevealageShader);
		quad1->setGLSLProgram(*accumTransparencyRevealageShader);
		quad2->setGLSLProgram(*accumTransparencyRevealageShader);
		quad3->setGLSLProgram(*accumTransparencyRevealageShader);
		quad4->setGLSLProgram(*accumTransparencyRevealageShader);
		quad5->setGLSLProgram(*accumTransparencyRevealageShader);

		accumFrameBuffer->clean();
		accumFrameBuffer->bind();
		accumFrameBuffer->bindForRenderPass(activeColorAttachments);

		accumFrameBuffer->cleanColorAttachment(1,clearColor);

		glEnable(GL_BLEND);
		glBlendFunci(0, GL_ONE, GL_ONE);
		glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
		//glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);

		model->renderTransparent();
		quad1->render();
		quad2->render();
		quad3->render();
		//quad4->render();
		//quad5->render();

		accumFrameBuffer->unbind();

		model->setGLSLProgram(defaultShader);
		quad1->setGLSLProgram(*defaultShader);
		quad2->setGLSLProgram(*defaultShader);
		quad3->setGLSLProgram(*defaultShader);
		quad4->setGLSLProgram(*defaultShader);
		quad5->setGLSLProgram(*defaultShader);

		glDisable(GL_BLEND);
		//glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		//_______________________________________________________________________________________________________________________________________________________________________________
		// Final compositing pass
		newOITCoverageShader->use();

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
		//screenFillingQuad->setTexture(accumFrameBuffer->getColorAttachment(1));
		//screenFillingQuad->render();

		window->swapBuffers();

		glfwPollEvents();

		check_gl_error();
	}

	glfwTerminate();
	window->release();
	
	return 0;
}