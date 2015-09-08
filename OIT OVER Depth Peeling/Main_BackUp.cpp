/* Back up from: 30.08.2015, 16:27 */

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
//// Post Process flags for the modelimport.
//unsigned int processFlagsOnModelImport = //aiProcess_FlipUVs|
//										 aiProcess_Triangulate|
//										 aiProcess_JoinIdenticalVertices|
//										 aiProcess_SortByPType;
//
///*
//Depth peeling [Everitt01] starts by rendering the scene normally with a depth test, which
//returns the nearest fragments to the eye. In a second pass, the depth buffer from the
//previous pass is bound to a fragment shader. To avoid read-modify-write hazards, the source
//and destination depth buffers are swapped every pass (ping ponging). If the fragment depth
//is less or equal to the associated depth from the previous pass, then this fragment is
//discarded and the next layer underneath is returned by the depth test.
//*/
//
///*
//for (i=0; i<num_passes; i++)
//{
//	clear color buffer
//	A = i % 2
//	B = (i+1) % 2
//	depth unit 0:
//	if(i == 0)
//		disable depth test
//	else
//		enable depth test
//		bind buffer A
//		disable depth writes
//		set depth func to GREATER
//	depth unit 1:
//		bind buffer B
//		clear depth buffer
//		enable depth writes
//		enable depth test
//		set depth func to LESS
//	render scene
//	save color buffer RGBA as layer i
//}
//*/
//
///*
//Necessary: Two depth buffers only.
//*/
//
///*
//Found out that:
//-depth only doesn't work propperly
//-with depth/stencil texture the depth values are generated propperly
//-maybe because of wrong texture binding the depth values that reach the shader are wrong (same values as the texSampler texture)
//
//Update 1 (26.08.2015, 17:36):
//-passing the depth/stencil texture works fine, if there is only one texture unit active (but more texture unit must be avaible)
//*/
//
//void createDepthPeelLayerTextures(std::vector<GLuint>& depthPeelLayerHandleStorage, int numOfDepthPeelLayers, unsigned int width, unsigned int height)
//{
//	for (int i = 0; i < numOfDepthPeelLayers; i++)
//	{
//		GLuint textureHandle = 0;
//		glGenTextures(1,&textureHandle);
//
//		glBindTexture(GL_TEXTURE_2D, textureHandle);
//
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//		depthPeelLayerHandleStorage.push_back(textureHandle);
//
//		check_gl_error();
//	}
//}
//
//int main()
//{
//	// Create and initialize window.
//	setErrorCallbackAndInit(error_callback);
//	GraphicsWindow* window = new GraphicsWindow(1200,800,"Test Window",NULL,NULL,key_callback);
//	window->makeContextCurrent();
//
//	// Initalize glew.
//	initGlew();
//
//	// Create and initialize Camera
//	FreeCamera* camera = new FreeCamera(45.0f, 16.0f/9.0f, 
//					0.0f, 0.0f, 
//					0.1f, 10000.0f, 
//					0.0004f, 3.0f,
//					glm::vec3(0,0,-50), 
//					glm::vec3(0,1,0), 
//					glm::vec3(0,0,0), 
//					true);
//
//	camera->setViewport(window->getWindowWidth(),window->getWindowHeight(),0.5f,0.5f);
//
//	// Load screen filling quad shader
//	GLSLProgram* screenFillingQuadShader = new GLSLProgram();
//	screenFillingQuadShader->initShaderProgram("screenFillingQuadVert.glsl","","","","screenFillingQuadFrag.glsl");
//
//	// Load screen filling quad shader for frame buffer depth texture test
//	GLSLProgram* screenFillingQuadDepthVisShader = new GLSLProgram();
//	screenFillingQuadDepthVisShader->initShaderProgram("screenFillingQuadDepthVisVert.glsl","","","","screenFillingQuadDepthVisFrag.glsl");
//
//	// Default shader.
//	GLSLProgram* depthPeelingShader = new GLSLProgram();
//	depthPeelingShader->initShaderProgram("DepthPeelingVert.glsl","","","","DepthPeelingFrag.glsl");
//
//	// Depth peeling shader.
//	GLSLProgram* defaultShader = new GLSLProgram();
//	defaultShader->initShaderProgram("Vert.glsl","","","","Frag.glsl");
//
//	Quad* quad1 = new Quad(glm::vec3(100+100,100+100,-1), glm::vec3(100+100,-100+100,-1), glm::vec3(-100+100,-100+100,-1), glm::vec3(-100+100,100+100,-1), glm::vec3(0), 10, 10, 0);
//	quad1->initQuad();
//	quad1->initDefaultTexture(255,0,0,125);
//
//	Quad* quad2 = new Quad(glm::vec3(100+100,100+100,-50), glm::vec3(100+100,-100+100,-50), glm::vec3(-100+100,-100+100,-50), glm::vec3(-100+100,100+100,-50), glm::vec3(0), 10, 10, 0);
//	quad2->initQuad();
//	quad2->initDefaultTexture(0,255,0,125);
//
//	Quad* quad3 = new Quad(glm::vec3(100+100,100+100,25), glm::vec3(100+100,-100+100,25), glm::vec3(-100+100,-100+100,25), glm::vec3(-100+100,100+100,25), glm::vec3(0), 10, 10, 0);
//	quad3->initQuad();
//	quad3->initDefaultTexture(0,0,255,125);
//
//	Quad* quad4 = new Quad(glm::vec3(100+100,100+100,50), glm::vec3(100+100,-100+100,50), glm::vec3(-100+100,-100+100,50), glm::vec3(-100+100,100+100,-50), glm::vec3(0), 10, 10, 0);
//	quad4->initQuad();
//	quad4->initDefaultTexture(255,255,0,125);
//
//	ModelLoader* modelImporter = new ModelLoader();
//	modelImporter->importModel("../Models/crytek-sponza/sponza.obj", processFlagsOnModelImport);
//
//	// Create and initialize model
//	Model* model = new Model(glm::vec3(0,0,0), "../Models/crytek-sponza/");
//	model->loadModel(modelImporter->getScene());
//
//	// Final framebuffer for compositing.
//	Framebuffer* frameBufferFinal = new Framebuffer(window->getWindowWidth(),window->getWindowHeight());
//	frameBufferFinal->setColorAttachment(0);
//	//frameBufferFinal->setDepthTextureOnly();
//	frameBufferFinal->setDepthStencilTexture();
//	frameBufferFinal->unbind();
//
//	// Create depth only framebuffers for depth peeling ping ponging.
//	Framebuffer* frameBuffer_tmp;
//
//	Framebuffer* frameBuffer_1 = new Framebuffer(window->getWindowWidth(),window->getWindowHeight());
//	frameBuffer_1->setColorAttachment(0);
//	//frameBuffer_1->setColorAttachment(1);
//	//frameBuffer_1->setDepthTextureOnly();
//	frameBuffer_1->setDepthStencilTexture();
//	frameBuffer_1->unbind();
//
//	Framebuffer* frameBuffer_2 = new Framebuffer(window->getWindowWidth(),window->getWindowHeight());
//	frameBuffer_2->setColorAttachment(0);
//	//frameBuffer_2->setColorAttachment(1);
//	//frameBuffer_2->setDepthTextureOnly();
//	frameBuffer_2->setDepthStencilTexture();
//	frameBuffer_2->unbind();
//
//	std::vector<unsigned int> activeColorAttachments_allFBOs;
//	activeColorAttachments_allFBOs.push_back(0);
//	//activeColorAttachments_allFBOs.push_back(1);
//
//	// Create screen filling quad.
//	Quad* screenFillingQuad = new Quad(glm::vec3(-1.0,1.0,0), glm::vec3(-1.0,-1.0,0), glm::vec3(1.0,-1.0,0), glm::vec3(1.0,1.0,0), glm::vec3(0), 0, 0, 0);
//	
//	screenFillingQuad->initQuad();
//	screenFillingQuad->setGLSLProgram(*screenFillingQuadShader);
//	screenFillingQuad->setTexture(frameBuffer_1->getColorAttachment(0));
//
//	// Create screen filling quad. (Currently for Debug)
//	Quad* screenFillingQuad_1 = new Quad(glm::vec3(-1.0,0.5,0), glm::vec3(-1.0,-0.5,0), glm::vec3(0.0,-0.5,0), glm::vec3(0.0,0.5,0), glm::vec3(0), 0, 0, 0);
//	
//	screenFillingQuad_1->initQuad();
//	screenFillingQuad_1->setGLSLProgram(*screenFillingQuadShader);
//	screenFillingQuad_1->setTexture(frameBuffer_1->getColorAttachment(0));
//
//	// Create screen filling quad. Currently for Debug)
//	Quad* screenFillingQuad_2 = new Quad(glm::vec3(0.0,0.5,0), glm::vec3(0.0,-0.5,0), glm::vec3(1.0,-0.5,0), glm::vec3(1.0,0.5,0), glm::vec3(0), 0, 0, 0);
//	
//	screenFillingQuad_2->initQuad();
//	screenFillingQuad_2->setGLSLProgram(*screenFillingQuadDepthVisShader);
//	screenFillingQuad_2->setTexture(frameBuffer_1->getColorAttachment(0));
//
//	// Additional textures that shall be passed to the renderinc function to the model. (In this case the depth texture for the depth peeling pass.)
//	std::vector<GLuint> addtionalTextures;
//	addtionalTextures.push_back(frameBuffer_1->getDepthStencilTexture());
//
//	// Two possibilities:
//	// - for each layer create a texture (store the handles in a vector) afterwards a quad and blend them for final compositing (huge overdraw, 
//	//   also not sure if the correct result is easily achieved)
//	// - for each layer create a texture (store the handles in a vector) and blend them manually in a final compositing pass (requieres one final pass 
//	//   with maximum of 31 textures, because of the sampler limitation)
//	std::vector<GLuint> depthPeelLayerHandles;
//
//	unsigned int numOfDepthPeelPasses=5;
//
//	createDepthPeelLayerTextures(depthPeelLayerHandles, numOfDepthPeelPasses, window->getWindowWidth(), window->getWindowHeight());
//
//	// Move this to "GraphicsWindow" 
//	glfwSetCursorPos(window->getWindowHandle(), (double) (window->getWindowWidth()/2.0), (double) (window->getWindowHeight()/2.0));
//	
//	// Move this to "Camera"
//	//glClearColor(0.4f,0.6f,0.94f,1.0f);
//	glClearColor(1.0f,1.0f,1.0f,0.0f);
//
//	check_gl_error();
//
//	// Sampler
//	GLuint sampler = 0;
//	glGenSamplers(1, &sampler);
//
//	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);  
//	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//
//	// Render Loop
//	while(!window->shouldClose())
//	{
//		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
//
//		// Default state
//		glEnable(GL_DEPTH_TEST);
//		glDepthFunc(GL_LEQUAL);
//		glEnable(GL_CULL_FACE);
//		glCullFace(GL_BACK);
//
//		// Update camera
//		camera->camControll(window->getWindowHandle());
//		camera->update();
//		
//		// Update shader uniforms
//		model->getCurrentGLSLProgram()->use();
//		model->getCurrentGLSLProgram()->setUniform("lightPosition", camera->getCamPos());
//		model->getCurrentGLSLProgram()->setUniform("camPosition", camera->getCamPos());
//		model->getCurrentGLSLProgram()->setUniform("viewMatrix", camera->getVMatrix());
//		model->getCurrentGLSLProgram()->setUniform("normalMatrix", camera->getTranspInvMVMatrix()); // Change this!
//		model->getCurrentGLSLProgram()->setUniform("VPMatrix", camera->getVPMatrix());
//
//		// TODO: Opaque pass.
//
//		frameBufferFinal->bind();
//		frameBufferFinal->bindForRenderPass(activeColorAttachments_allFBOs);
//			
//		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
//
//		// Opaque pass state
//		glEnable(GL_DEPTH_TEST);
//		glDepthFunc(GL_LEQUAL);
//		glEnable(GL_CULL_FACE);
//		glCullFace(GL_BACK);
//
//		model->renderOpaque();
//
//		frameBufferFinal->unbind();
//
//		//________________________________________________________________________________________________________________________________________________________________
//		// Depth peeling (transparent only).
//		// Initial depth peeling pass.
//		// Transparent pass state
//		glEnable(GL_BLEND);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//		glDisable(GL_CULL_FACE);
//
//		frameBuffer_1->bind();
//		frameBuffer_1->bindForRenderPass(activeColorAttachments_allFBOs);
//			
//		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
//		glEnable(GL_DEPTH_TEST);
//		glDepthMask(GL_TRUE);
//		glDepthFunc(GL_LEQUAL);
//
//		defaultShader->use();
//		model->renderTransparent();
//		quad1->render();
//		quad2->render();
//		quad3->render();
//		//quad4->render();
//
//		frameBuffer_1->unbind();
//
//		// Switch shader for depth peeling
//		depthPeelingShader->use();
//
//		depthPeelingShader->setUniform("VPMatrix", camera->getVPMatrix());
//
//		model->setGLSLProgram(depthPeelingShader);
//		quad1->setGLSLProgram(*depthPeelingShader);
//		quad2->setGLSLProgram(*depthPeelingShader);
//		quad3->setGLSLProgram(*depthPeelingShader);
//		//quad4->setGLSLProgram(*depthPeelingShader);
//
//		GLuint FBOTexture_1 = frameBufferFinal->getColorAttachment(0);
//		GLuint FBOTexture_2 = frameBufferFinal->getColorAttachment(0);
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
//			/*
//			// Render pass
//			model->setGLSLProgram(defaultShader);
//			quad1->setGLSLProgram(*defaultShader);
//			quad2->setGLSLProgram(*defaultShader);
//			quad3->setGLSLProgram(*defaultShader);
//			//quad4->setGLSLProgram(*defaultShader);
//
//			frameBuffer_1->bind();
//			frameBuffer_1->bindForRenderPass(activeColorAttachments_allFBOs);
//			
//			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
//			glEnable(GL_DEPTH_TEST);
//			glDepthMask(GL_TRUE);
//			glDepthFunc(GL_LEQUAL);
//
//			defaultShader->use();
//
//			model->renderTransparent();
//			quad1->render();
//			quad2->render();
//			quad3->render();
//			//quad4->render();
//
//			frameBuffer_1->unbind();
//
//			model->setGLSLProgram(depthPeelingShader);
//			quad1->setGLSLProgram(*depthPeelingShader);
//			quad2->setGLSLProgram(*depthPeelingShader);
//			quad3->setGLSLProgram(*depthPeelingShader);
//			//quad4->setGLSLProgram(*depthPeelingShader);
//
//			if (i==0)
//			{
//				glDisable(GL_DEPTH_TEST);
//			} else
//			{
//				glEnable(GL_DEPTH_TEST);	
//			}
//			*/
//			// Peeling pass.
//			frameBuffer_2->bind();
//			frameBuffer_2->bindForRenderPass(activeColorAttachments_allFBOs);
//			
//			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
//			glEnable(GL_DEPTH_TEST);
//			glDepthMask(GL_TRUE);
//			glDepthFunc(GL_LEQUAL);
//
//			depthPeelingShader->use();
//			// Render a layer.
//			model->renderTransparentWithAdditionalTextures(addtionalTextures,sampler);
//			quad1->renderWithAdditionalTextures(addtionalTextures,sampler);
//			quad2->renderWithAdditionalTextures(addtionalTextures,sampler);
//			quad3->renderWithAdditionalTextures(addtionalTextures,sampler);
//			//quad4->renderWithAdditionalTextures(addtionalTextures,sampler);
//
//			frameBuffer_2->unbind();
//
//			// Save layer.
//			frameBuffer_2->bindForReading();
//			
//			glBindTexture(GL_TEXTURE_2D, depthPeelLayerHandles[i]);
//			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, window->getWindowWidth(), window->getWindowHeight());
//
//			frameBuffer_2->unbind();
//
//			// Ping pong
//			frameBuffer_tmp = frameBuffer_1;
//			frameBuffer_1 = frameBuffer_2;
//			frameBuffer_2 = frameBuffer_tmp;
//			addtionalTextures[0]=frameBuffer_1->getDepthStencilTexture();
//		}
//
//		model->setGLSLProgram(defaultShader);
//		quad1->setGLSLProgram(*defaultShader);
//		quad2->setGLSLProgram(*defaultShader);
//		quad3->setGLSLProgram(*defaultShader);
//		//quad4->setGLSLProgram(*defaultShader);
//
//		// Default pass state.
//		glDisable(GL_BLEND);
//		glDepthMask(GL_TRUE);
//		//glEnable(GL_CULL_FACE);
//		//glCullFace(GL_BACK);
//		//________________________________________________________________________________________________________________________________________________________________
//
//		glDisable(GL_DEPTH_TEST);
//		glDepthMask(GL_FALSE);
//		glEnable(GL_BLEND);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//		for (int i = numOfDepthPeelPasses-1; i >= 0; i--)
//		{
//			screenFillingQuad->getCurrentShaderProgram()->use();
//			screenFillingQuad->setTexture(depthPeelLayerHandles[i]);
//			screenFillingQuad->render();
//		}
//		glDisable(GL_BLEND);
//		glDepthMask(GL_TRUE);
//
//		//screenFillingQuad->getCurrentShaderProgram()->use();
//		//screenFillingQuad->setTexture(depthPeelLayerHandles[1]);
//		//screenFillingQuad->render();
//
//		// Debug screen filling quad renders.
//		//screenFillingQuad_1->getCurrentShaderProgram()->use();
//		//screenFillingQuad_1->setTexture(FBOTexture_1);
//		//screenFillingQuad_1->render();
//		//screenFillingQuad_2->getCurrentShaderProgram()->use();
//		//screenFillingQuad_2->setTexture(FBOTexture_2);
//		//screenFillingQuad_2->render();
//
//		frameBuffer_1->bind();
//		frameBuffer_1->clean();
//		frameBuffer_1->unbind();
//
//		frameBuffer_2->bind();
//		frameBuffer_2->clean();
//		frameBuffer_2->unbind();
//
//		window->swapBuffers();
//
//		glfwPollEvents();
//
//		check_gl_error();
//	}
//
//	glfwTerminate();
//	window->release();
//	
//	return 0;
//}
