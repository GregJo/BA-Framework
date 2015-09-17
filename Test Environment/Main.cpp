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

#include "OIT Test Environment.h"

#include "OIT OVER Depth Peeling Algorithm.h"
#include "OIT Weighted Sum Algorithm.h"
#include "OIT Weighted Average Algorithm.h"
#include "OIT New Coverage Algorithm.h"

#include <iostream>

// Post Process flags for the modelimport.
unsigned int processFlagsOnModelImport = //aiProcess_FlipUVs|
										 aiProcess_Triangulate|
										 aiProcess_JoinIdenticalVertices|
										 aiProcess_SortByPType;

unsigned int numOfDepthPeelPasses=15;

/*
TODO: Scale models/depth (which of both?) according farplane/model bounding box (Maybe both). 
*/

int main()
{
	// Create and initialize window.
	setErrorCallbackAndInit(error_callback);
	// GraphicsWindow* window = new GraphicsWindow(1200,800,"Test Window",NULL,NULL,key_callback);
	std::vector<int> targets;
	std::vector<int> hints;

	GraphicsWindow* window = new GraphicsWindow(1900,1080,"Test Window",NULL,NULL,key_callback);
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

	// Load difference image shader.
	GLSLProgram* differenceImageShader = new GLSLProgram();
	differenceImageShader->initShaderProgram("DifferenceImageVert.glsl","","","","DifferenceImageFrag.glsl");

	// Call here all the content/shader set up functions.
	std::vector<GLSLProgram*> shaders;
	shaders = loadShader();

	std::vector<std::string> fullTexturePaths;
	fullTexturePaths.push_back("../Content/Textures/Particle_Smoke/smoke_particle_grey_base_2.png");
	fullTexturePaths.push_back("../Content/Textures/Particle_Smoke/smoke_particle_grey_base_3.png");
	fullTexturePaths.push_back("../Content/Textures/Particle_Smoke/smoke_particle_white_base.png");

	loadAdditionalTextures(fullTexturePaths);

	std::vector<Quad*> transparentQuads = loadTransparentQuads();
	std::vector<Quad*> opaqueQuads = loadOpaqueQuads();
	std::vector<Model*> models = loadModels(processFlagsOnModelImport);

	//void updateShaderUniforms(std::vector<GLSLProgram*> shader, FreeCamera* camera);

	// The two final textures of two OIT functions (mostly a fast OIT agorithms vs the reference OIT algorithm). These two final textures 
	// may be used to compute a difference image or the squared mean error.
	GLuint finalTextureHandle1 = 0;
	glGenTextures(1,&finalTextureHandle1);

	glBindTexture(GL_TEXTURE_2D, finalTextureHandle1);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, window->getWindowWidth(), window->getWindowHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint finalTextureHandle2 = 0;
	glGenTextures(1,&finalTextureHandle2);

	glBindTexture(GL_TEXTURE_2D, finalTextureHandle2);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, window->getWindowWidth(), window->getWindowHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	std::vector<GLuint> finalTextures;
	finalTextures.push_back(finalTextureHandle1);
	finalTextures.push_back(finalTextureHandle2);

	// Framebuffer for opaque meshes.
	Framebuffer* frameBufferOpaque = new Framebuffer(window->getWindowWidth(),window->getWindowHeight());
	std::vector<unsigned int> activeColorAttachments;
	activeColorAttachments.push_back(0);
	frameBufferOpaque->setColorAttachment(0);
	//frameBufferFinal->setDepthTextureOnly();
	frameBufferOpaque->setDepthStencilTexture();
	frameBufferOpaque->unbind();

	// Create and initialize all the different OIT Algorithms.
	OIT_OVER_DepthPeeling* depthPeelingAlgorithm = new OIT_OVER_DepthPeeling();
	depthPeelingAlgorithm->setNumOfDepthPeelingPasses(numOfDepthPeelPasses);
	depthPeelingAlgorithm->VInit(window);

	OITWeightedSum* weightedSumAlgorithm = new OITWeightedSum();
	weightedSumAlgorithm->VInit(window);

	OITWeightedAverage* weightedAverageAlgorithm = new OITWeightedAverage();
	weightedAverageAlgorithm->VInit(window);

	OITNewCoverage* newCoverageAlgorithm = new OITNewCoverage();
	newCoverageAlgorithm->VInit(window);

	// Create and initialize a screen filling quad.
	Quad* screenFillingQuad = new Quad(glm::vec3(-1.0,1.0,0), glm::vec3(-1.0,-1.0,0), glm::vec3(1.0,-1.0,0), glm::vec3(1.0,1.0,0), glm::vec3(0), 0, 0, 0);
	
	screenFillingQuad->initQuad();
	screenFillingQuad->setGLSLProgram(*shaders[Shader::SCREEN_FILLING_SHADER]);
	
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

		updateShaderUniforms(window, camera, shaders);

		renderOpaqueGeometry(frameBufferOpaque, shaders[Shader::DEFAULT_SHADER], activeColorAttachments, models, opaqueQuads);

		// Chose and use here the OIT algorithms, that should be compared with each other (mostly a fast OIT agorithms vs the reference OIT algorithm).
		//depthPeelingAlgorithm->VAlgorithm(camera, models, transparentQuads, frameBufferOpaque, shaders[Shader::DEFAULT_SHADER], sampler);

		////TODO: Make this following code snipplet a function.
		//shaders[Shader::SCREEN_FILLING_SHADER]->use();
		//screenFillingQuad->setTexture(depthPeelingAlgorithm->VGetResultTextureHandle());
		//screenFillingQuad->render();

		//weightedSumAlgorithm->VAlgorithm(camera, models, transparentQuads, frameBufferOpaque, shaders[Shader::DEFAULT_SHADER], sampler);

		//shaders[Shader::SCREEN_FILLING_SHADER]->use();
		//screenFillingQuad->setTexture(weightedSumAlgorithm->VGetResultTextureHandle());
		//screenFillingQuad->render();

		//weightedAverageAlgorithm->VAlgorithm(camera, models, transparentQuads, frameBufferOpaque, shaders[Shader::DEFAULT_SHADER], sampler);

		//shaders[Shader::SCREEN_FILLING_SHADER]->use();
		//screenFillingQuad->setTexture(weightedAverageAlgorithm->VGetResultTextureHandle());
		//screenFillingQuad->render();

		//newCoverageAlgorithm->VAlgorithm(camera, models, transparentQuads, frameBufferOpaque, shaders[Shader::DEFAULT_SHADER], sampler);

		//shaders[Shader::SCREEN_FILLING_SHADER]->use();
		//screenFillingQuad->setTexture(newCoverageAlgorithm->VGetResultTextureHandle());
		//screenFillingQuad->render();

		depthPeelingAlgorithm->VAlgorithm(camera, models, transparentQuads, frameBufferOpaque, shaders[Shader::DEFAULT_SHADER], sampler);
		finalTextureHandle1 = depthPeelingAlgorithm->VGetResultTextureHandle();

		weightedAverageAlgorithm->VAlgorithm(camera, models, transparentQuads, frameBufferOpaque, shaders[Shader::DEFAULT_SHADER], sampler);
		finalTextureHandle2 = weightedAverageAlgorithm->VGetResultTextureHandle();

		//weightedSumAlgorithm->VAlgorithm(camera, models, transparentQuads, frameBufferOpaque, shaders[Shader::DEFAULT_SHADER], sampler);
		//finalTextureHandle2 = weightedSumAlgorithm->VGetResultTextureHandle();

		//newCoverageAlgorithm->VAlgorithm(camera, models, transparentQuads, frameBufferOpaque, shaders[Shader::DEFAULT_SHADER], sampler);
		//finalTextureHandle2 = newCoverageAlgorithm->VGetResultTextureHandle();

		differenceImageShader->use();

		finalTextures[0]=finalTextureHandle1;
		finalTextures[1]=finalTextureHandle2;

		screenFillingQuad->renderWithAdditionalTextures(finalTextures,sampler);

		window->swapBuffers();

		glfwPollEvents();

		check_gl_error();
	}

	glfwTerminate();
	window->release();
	
	return 0;
}