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
#include "Smoke Particle System.h"

#include "OIT OVER Depth Peeling Algorithm.h"
#include "OIT Weighted Sum Algorithm.h"
#include "OIT Weighted Average Algorithm.h"
#include "OIT New Coverage Algorithm.h"
#include "OIT New Coverage Depth Weights Algorithm.h"

#include <time.h>
#include <iostream>

#define WINDOW_RESOLUTION_WIDTH  1600
#define WINDOW_RESOLUTION_HEIGHT 900

// Post Process flags for the modelimport.
unsigned int processFlagsOnModelImport = //aiProcess_FlipUVs|
										 aiProcess_Triangulate|
										 aiProcess_JoinIdenticalVertices|
										 aiProcess_SortByPType;

unsigned int numOfDepthPeelPasses=15;

clock_t start;
double systemTime;

float globalScaling = 0.125;

/*
TODO: Scale models/depth (which of both?) according farplane/biggest model bounding box (Maybe both).
The unscaled diagonal of the sponza scene, the biggest used model, is 4000.0 units. 
Update 1 (27.09.2015, 01:45): Weird "noise"/wrong calculations on the foliage of the crytec sponza, at the difference image.
Update 2 (02.10.2015, 03:01): Still could not fix the reason for the "noise"/the wrong calculations on the foliage of crytecs sponza, at the difference image.
							  Noticed weird rendering of the foliage, rendering too much around it, despite setting the alpha of the alpha test to 0.0.
							  Furthermore the opaque texture becomes black, after copying from it.
							  After the first pass the copied difference texture does not contain anything.
Update 3 (03.10.2015, 09:45): Weird "noise"/wrong calculations on the foliage of the crytec sponza, at the difference image are caused by the alpha test, 
							  used for the approximative algorithms. Results of OIT New Coverage, etc. algorithms are faulted by the nature of calculating
							  the transparency, e.g. multiplying yellow and green will result in green.
							  Hence maybe take folliage out of consideration, or make it a specific category.
							  (Currently only experimenting with the OIT New Coverage algorithm, alpha > 0.5 of Alpha Test produces already better results.)
*/

int main()
{
	start = clock();
	// Create and initialize window.
	setErrorCallbackAndInit(error_callback);
	// GraphicsWindow* window = new GraphicsWindow(1200,800,"Test Window",NULL,NULL,key_callback);
	std::vector<int> targets;
	std::vector<int> hints;

	GraphicsWindow* window = new GraphicsWindow(WINDOW_RESOLUTION_WIDTH,WINDOW_RESOLUTION_HEIGHT,"Test Window",NULL,NULL,key_callback);
	window->makeContextCurrent();

	// Initalize glew.
	initGlew();

	// Create and initialize Camera
	FreeCamera* camera = new FreeCamera(45.0f, 16.0f/9.0f, 
					0.0f, 0.0f, 
					0.1f, 4000.0f * globalScaling, 
					0.0004f, 5.0f * globalScaling,
					glm::vec3(0,0,-50) * globalScaling, 
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

	//// Smoke particle system test.
	////_________________________________________________________________________________________________________________________________________________________________

	//std::vector<SmokeParticleSystem*> smokeParticleSystems;

	//GLSLProgram* smkParticleSystemShader = new GLSLProgram();
	//smkParticleSystemShader->initShaderProgram("BillboardSmokeParticleVert.glsl","","","","BillboardSmokeParticleFrag.glsl");

	//SmokeParticleSystem* smkParticleSystem = new SmokeParticleSystem(10,400,200,glm::vec3(-300,25,0),glm::vec3(0,1,0),0.25f,1000.0f);
	//smkParticleSystem->init(camera, "../Content/Textures/Particle_Smoke/smoke_particle_grey_base_3.png");

	//smokeParticleSystems.push_back(smkParticleSystem);

	////_________________________________________________________________________________________________________________________________________________________________

	loadAdditionalTextures(fullTexturePaths);

	std::vector<Quad*> opaqueBillboards;
	std::vector<Quad*> transparentBillboards = loadBillboards(shaders[BILLBOARD_SHADER],globalScaling);
	std::vector<Quad*> transparentQuads = loadTransparentQuads(globalScaling);
	std::vector<Quad*> opaqueQuads = loadOpaqueQuads(globalScaling);
	std::vector<Model*> models = loadModels(processFlagsOnModelImport, globalScaling);
	std::vector<SmokeParticleSystem*> smokeParticleSystems = loadSmokeParticleSystem(camera,globalScaling);

	//void updateShaderUniforms(std::vector<GLSLProgram*> shader, FreeCamera* camera);

	// The two final textures of two OIT functions (mostly a fast OIT agorithms vs the reference OIT algorithm). These two final textures 
	// may be used to compute a difference image or the squared mean error.
	
	GLuint squaredMeanErrorTextureHandle = 0;
	glGenTextures(1,&squaredMeanErrorTextureHandle);

	glBindTexture(GL_TEXTURE_2D, squaredMeanErrorTextureHandle);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, window->getWindowWidth(), window->getWindowHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint finalTextureHandle1 = 0;
	glGenTextures(1,&finalTextureHandle1);

	glBindTexture(GL_TEXTURE_2D, finalTextureHandle1);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, window->getWindowWidth(), window->getWindowHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint finalTextureHandle2 = 0;
	glGenTextures(1,&finalTextureHandle2);

	glBindTexture(GL_TEXTURE_2D, finalTextureHandle2);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, window->getWindowWidth(), window->getWindowHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

	glBindTexture(GL_TEXTURE_2D, 0);

	std::vector<GLuint> finalTextures;
	finalTextures.push_back(finalTextureHandle1);
	finalTextures.push_back(finalTextureHandle2);

	// The difference image data as input for calculating the squared mean error.
	unsigned char* differenceImageOITAlgorithms = new unsigned char[window->getWindowWidth()*window->getWindowHeight()*3];

	// Framebuffer for opaque meshes.
	Framebuffer* frameBufferOpaque = new Framebuffer(window->getWindowWidth(),window->getWindowHeight());
	std::vector<unsigned int> activeColorAttachments;
	activeColorAttachments.push_back(0);
	frameBufferOpaque->setColorAttachment(0);
	frameBufferOpaque->setDepthStencilTexture();
	frameBufferOpaque->unbind();

	// Framebuffer for opaque meshes.
	Framebuffer* frameBufferSquaredMeanError = new Framebuffer(window->getWindowWidth(),window->getWindowHeight());
	frameBufferSquaredMeanError->setColorAttachment(0);
	frameBufferSquaredMeanError->setDepthStencilTexture();
	frameBufferSquaredMeanError->unbind();

	// Create and initialize all the different OIT Algorithms.
	OIT_OVER_DepthPeeling* depthPeelingAlgorithm = new OIT_OVER_DepthPeeling();
	depthPeelingAlgorithm->setNumOfDepthPeelingPasses(numOfDepthPeelPasses);
	depthPeelingAlgorithm->VInit(window);
	depthPeelingAlgorithm->setGlobalScale(globalScaling);

	OITWeightedSum* weightedSumAlgorithm = new OITWeightedSum();
	weightedSumAlgorithm->setGlobalScale(globalScaling);
	weightedSumAlgorithm->VInit(window);

	OITWeightedAverage* weightedAverageAlgorithm = new OITWeightedAverage();
	weightedAverageAlgorithm->setGlobalScale(globalScaling);
	weightedAverageAlgorithm->VInit(window);

	OITNewCoverage* newCoverageAlgorithm = new OITNewCoverage();
	newCoverageAlgorithm->setGlobalScale(globalScaling);
	newCoverageAlgorithm->VInit(window);

	OITNewCoverageDepthWeights* newCoverageDepthWeightsAlgorithm = new OITNewCoverageDepthWeights();
	newCoverageDepthWeightsAlgorithm->setGlobalScale(globalScaling);
	newCoverageDepthWeightsAlgorithm->VInit(window);

	// Create and initialize a screen filling quad.
	Quad* screenFillingQuad = new Quad(glm::vec3(-1.0,1.0,0), glm::vec3(-1.0,-1.0,0), glm::vec3(1.0,-1.0,0), glm::vec3(1.0,1.0,0), glm::vec3(0), 0, 0, 0);
	
	screenFillingQuad->initQuad();
	screenFillingQuad->setGLSLProgram(*shaders[SCREEN_FILLING_SHADER]);
	
	// Sampler
	GLuint sampler = 0;
	glGenSamplers(1, &sampler);

	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);  
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	check_gl_error();

	glClearColor(0.4f,0.6f,0.94f,0.0f);

	// Render Loop
	while(!window->shouldClose())
	{
		systemTime = clock();

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		// Update camera
		camera->camControll(window->getWindowHandle());
		camera->update();

		updateShaderUniforms(window, camera, shaders);

		renderOpaqueGeometry(frameBufferOpaque, shaders[DEFAULT_SHADER], activeColorAttachments, models, opaqueQuads, opaqueBillboards, globalScaling);

		//shaders[SCREEN_FILLING_SHADER]->use();

		//screenFillingQuad->setTexture(frameBufferOpaque->getColorAttachment(0));
		//screenFillingQuad->render();

		//shaders[DEFAULT_SHADER]->use();

		// Smoke particle system test.
		//_________________________________________________________________________________________________________________________________________________________________
		//frameBufferOpaque->bind();
		//frameBufferOpaque->bindForRenderPass(activeColorAttachments);

		////glDisable(GL_DEPTH_TEST);
		//glDepthMask(GL_FALSE);
		//glDisable(GL_CULL_FACE);

		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glEnable(GL_ALPHA_TEST);
		//glAlphaFunc(GL_GREATER,0.2f);

		//smkParticleSystem->update(camera, systemTime);

		//smkParticleSystem->render(smkParticleSystemShader, camera);

		//glDisable(GL_BLEND);
		//glDisable(GL_ALPHA_TEST);
		////glEnable(GL_DEPTH_TEST);
		//glDepthMask(GL_TRUE);
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);

		//frameBufferOpaque->unbind();

		//shaders[DEFAULT_SHADER]->use();
		//_________________________________________________________________________________________________________________________________________________________________

		for (int i = 0; i < smokeParticleSystems.size(); i--)
		{
			smokeParticleSystems[i]->update(camera, systemTime);
		}

		//// Testing billboard rendering
		//glDisable(GL_DEPTH_TEST);
		//glDisable(GL_CULL_FACE);

		//frameBufferOpaque->bind();
		//frameBufferOpaque->bindForRenderPass(activeColorAttachments);

		//billboards[0]->getCurrentShaderProgram()->use();
		//billboards[0]->render();

		//frameBufferOpaque->unbind();

		//glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);

		//glUseProgram(shaders[DEFAULT_SHADER]->getHandle());

		// Chose and use here the OIT algorithms, that should be compared with each other (mostly a fast OIT agorithms vs the reference OIT algorithm).
		//depthPeelingAlgorithm->VAlgorithm(camera, models, transparentQuads, frameBufferOpaque, shaders[DEFAULT_SHADER], sampler);

		////TODO: Make this following code snipplet a function.
		//shaders[SCREEN_FILLING_SHADER]->use();
		//screenFillingQuad->setTexture(depthPeelingAlgorithm->VGetResultTextureHandle());
		//screenFillingQuad->render();

		//weightedSumAlgorithm->VAlgorithm(camera, models, transparentQuads, smokeParticleSystems, frameBufferOpaque, shaders[Shader::DEFAULT_SHADER], sampler);

		//shaders[Shader::SCREEN_FILLING_SHADER]->use();
		//screenFillingQuad->setTexture(weightedSumAlgorithm->VGetResultTextureHandle());
		//screenFillingQuad->render();

		//weightedAverageAlgorithm->VAlgorithm(camera, models, transparentQuads, smokeParticleSystems, frameBufferOpaque, shaders[Shader::DEFAULT_SHADER], sampler);

		//shaders[Shader::SCREEN_FILLING_SHADER]->use();
		//screenFillingQuad->setTexture(weightedAverageAlgorithm->VGetResultTextureHandle());
		//screenFillingQuad->render();

		//newCoverageAlgorithm->VAlgorithm(camera, models, transparentQuads, smokeParticleSystems, frameBufferOpaque, shaders[Shader::DEFAULT_SHADER], sampler);

		//shaders[Shader::SCREEN_FILLING_SHADER]->use();
		//screenFillingQuad->setTexture(newCoverageAlgorithm->VGetResultTextureHandle());
		//screenFillingQuad->render(); 

		//newCoverageDepthWeightsAlgorithm->VAlgorithm(camera, models, transparentQuads, smokeParticleSystems, frameBufferOpaque, shaders[Shader::DEFAULT_SHADER], sampler);

		//shaders[Shader::SCREEN_FILLING_SHADER]->use();
		//screenFillingQuad->setTexture(newCoverageDepthWeightsAlgorithm->VGetResultTextureHandle());
		//screenFillingQuad->render(); 

		depthPeelingAlgorithm->VAlgorithm(camera, models, transparentQuads, frameBufferOpaque, shaders[Shader::DEFAULT_SHADER], sampler);
		finalTextureHandle1 = depthPeelingAlgorithm->VGetResultTextureHandle();

		//weightedAverageAlgorithm->VAlgorithm(camera, models, transparentQuads, frameBufferOpaque, shaders[Shader::DEFAULT_SHADER], sampler);
		//finalTextureHandle1 = weightedAverageAlgorithm->VGetResultTextureHandle();

		//weightedSumAlgorithm->VAlgorithm(camera, models, transparentQuads, frameBufferOpaque, shaders[Shader::DEFAULT_SHADER], sampler);
		//finalTextureHandle1 = weightedSumAlgorithm->VGetResultTextureHandle();

		newCoverageAlgorithm->VAlgorithm(camera, models, transparentQuads, frameBufferOpaque, shaders[Shader::DEFAULT_SHADER], sampler);
		finalTextureHandle2 = newCoverageAlgorithm->VGetResultTextureHandle();

		//frameBufferSquaredMeanError->clean();
		//frameBufferSquaredMeanError->bind();
		//frameBufferSquaredMeanError->bindForRenderPass(activeColorAttachments);

		differenceImageShader->use();

		finalTextures[0]=finalTextureHandle1;
		finalTextures[1]=finalTextureHandle2;

		screenFillingQuad->renderWithAdditionalTextures(finalTextures,sampler);

		//frameBufferSquaredMeanError->unbind();

		//frameBufferSquaredMeanError->bindForReading();

		glBindTexture(GL_TEXTURE_2D, squaredMeanErrorTextureHandle);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, window->getWindowWidth(), window->getWindowHeight());
		glBindTexture(GL_TEXTURE_2D, 0);

		/*frameBufferSquaredMeanError->unbind();*/

		// Anzahl der Textureunits ermitteln
		int numberOfTextureUnits = 0;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &numberOfTextureUnits);
		GLuint lastTextureUnit = GL_TEXTURE0 + (numberOfTextureUnits - 1);
 
		// letzte texture unit binden, damit man mit der aktuellen textur arbeiten kann
		glActiveTexture(lastTextureUnit);
 
		// Kopiere textur in ein array:
		// ============================================================
		glBindTexture(GL_TEXTURE_2D, squaredMeanErrorTextureHandle);
 
		GLint width;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
 
		GLint height;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
 
		float* pixels = new float[(int)width * (int)height * 3];
 
		// "4" -> 32 bits pre channel = 4 bytes per channel.
		glPixelStorei(GL_PACK_ALIGNMENT, 4);

		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, pixels);

		glBindTexture(GL_TEXTURE_2D, 0);

		float squaredMeanError = 0.0f;

		for (int i = 0; i < (int)width * (int)height * 3; i++)
		{
			squaredMeanError += std::pow((1.0f/3.0f*pixels[i]),2.0f);
		}

		delete[] pixels;

		squaredMeanError /= float(window->getWindowWidth()*window->getWindowHeight()*3);

		std::cout << "Squared Mean Error: " << squaredMeanError << std::endl;

		glActiveTexture(GL_TEXTURE0);

		window->swapBuffers();

		glfwPollEvents();

		check_gl_error();
	}

	glfwTerminate();
	window->release();
	
	return 0;
}