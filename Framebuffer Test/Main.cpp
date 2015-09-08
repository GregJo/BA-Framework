#pragma once
#include "GraphicsWindow.h"
#include "GLSLProgram.h"
#include "FreeCamera.h"
#include "ModelLoader.h"
#include "Model.h"
#include "Framebuffer.h"
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

	ModelLoader* modelImporter = new ModelLoader();
	modelImporter->importModel("../Models/crytek-sponza/sponza.obj", processFlagsOnModelImport);

	// Load screen filling quad shader for frame buffer depth texture test
	GLSLProgram* screenFillingQuadShader = new GLSLProgram();
	screenFillingQuadShader->initShaderProgram("screenFillingQuadVert.glsl","","","","screenFillingQuadFrag.glsl");

	// Load MRT shader for frame buffer test
	GLSLProgram* mrtShader = new GLSLProgram();
	mrtShader->initShaderProgram("MRTPassVert.glsl","","","","MRTPassFrag.glsl");

	// Load shader for fame buffer test
	GLSLProgram* shader = new GLSLProgram();
	shader->initShaderProgram("Vert.glsl","","","","Frag.glsl");

	// Create and initialize model
	Model* model = new Model(glm::vec3(0,0,0), "../Models/crytek-sponza/");
	model->setGLSLProgram(shader);
	model->loadModel(modelImporter->getScene());

	// Create framebuffer
	Framebuffer* frameBuffer = new Framebuffer(window->getWindowWidth(),window->getWindowHeight());
	
	std::vector<unsigned int> activeColorAttachments;
	activeColorAttachments.push_back(0);
	activeColorAttachments.push_back(1);
	activeColorAttachments.push_back(2);
	activeColorAttachments.push_back(3);
	activeColorAttachments.push_back(4);

	frameBuffer->setColorAttachment(0);
	frameBuffer->setColorAttachment(1);
	frameBuffer->setColorAttachment(2);
	frameBuffer->setColorAttachment(3);
	frameBuffer->setColorAttachment(4);

	//frameBuffer->setDepthAttachment();
	frameBuffer->setDepthStencilTexture();

	frameBuffer->unbind();

	// Create depth only framebuffer
	Framebuffer* frameBufferDepthOnly = new Framebuffer(window->getWindowWidth(),window->getWindowHeight());

	frameBufferDepthOnly->setDepthTextureOnly();

	frameBufferDepthOnly->unbind();

	// Create and initialize Quad
	Quad* screenFillingQuad = new Quad(glm::vec3(-1.0,1.0,0), glm::vec3(-1.0,-1.0,0), glm::vec3(1.0,-1.0,0), glm::vec3(1.0,1.0,0), glm::vec3(0), 0, 0, 0);
	
	screenFillingQuad->initQuad();
	screenFillingQuad->setGLSLProgram(*screenFillingQuadShader);
	screenFillingQuad->setTexture(frameBuffer->getDepthStencilTexture());

	// Move this to "GraphicsWindow" 
	glfwSetCursorPos(window->getWindowHandle(), (double) (window->getWindowWidth()/2.0), (double) (window->getWindowHeight()/2.0));
	
	std::vector<GLuint> addtionalTextures;
	addtionalTextures.push_back(frameBuffer->getDepthStencilTexture());

	// Sampler
	GLuint sampler = 0;
	glGenSamplers(1, &sampler);

	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);  
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	check_gl_error();
	
	// Move this to "Camera"
	glClearColor(0.4f,0.6f,0.94f,1.0f);

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

		// Use framebuffer
		frameBuffer->clean();
		frameBuffer->bind();
		frameBuffer->bindForRenderPass(activeColorAttachments);

		// Use depth only framebuffer
		//frameBufferDepthOnly->clean();
		//frameBufferDepthOnly->bind();
		//frameBufferDepthOnly->bindForDepthPass();

		// Update shader uniforms
		model->getCurrentGLSLProgram()->use();
		model->getCurrentGLSLProgram()->setUniform("normalMatrix", camera->getTranspInvMVMatrix()); // Change this!
		model->getCurrentGLSLProgram()->setUniform("VPMatrix", camera->getVPMatrix());
		model->getCurrentGLSLProgram()->setUniform("lightPosition", camera->getCamPos());
		model->getCurrentGLSLProgram()->setUniform("camPosition", camera->getCamPos());
		model->getCurrentGLSLProgram()->setUniform("viewMatrix", camera->getVMatrix());
		model->getCurrentGLSLProgram()->setUniform("normalMatrix", camera->getTranspInvMVMatrix()); // Change this!
		model->getCurrentGLSLProgram()->setUniform("VPMatrix", camera->getVPMatrix());

		model->render(Model::RenderMode::COMPLETE,addtionalTextures,sampler);

		//frameBuffer->unbind();
		//frameBufferDepthOnly->unbind();

		//frameBuffer->debugPass(*window, activeColorAttachments);

		frameBuffer->unbind();

		//glActiveTexture(GL_TEXTURE0);
		glDepthMask(GL_FALSE);
		//screenFillingQuad->setTexture(frameBufferDepthOnly->getDepthTextureOnly());
		screenFillingQuad->setTexture(frameBuffer->getColorAttachment(0));
		screenFillingQuad->getCurrentShaderProgram()->use();
		screenFillingQuad->render();
		glDepthMask(GL_TRUE);
		window->swapBuffers();

		glfwPollEvents();

		check_gl_error();
	}

	glfwTerminate();
	window->release();
	
	return 0;
}