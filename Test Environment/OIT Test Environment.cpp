#pragma once

#include "OIT Test Environment.h"

// TODO
std::vector<GLSLProgram*> loadShader()
{
	std::vector<GLSLProgram*> outputShader;

	// Load default shader.
	GLSLProgram* defaultShader = new GLSLProgram();
	defaultShader->initShaderProgram("Vert.glsl","","","","Frag.glsl");

	// Load Weighted Sum shader for alpha blending.
	GLSLProgram* weightedSumShader = new GLSLProgram();
	weightedSumShader->initShaderProgram("WeightedSumVert.glsl","","","","WeightedSumFrag.glsl");

	// Load Depth Peeling shader for the depth peeling reference solution.
	GLSLProgram* depthPeelingShader = new GLSLProgram();
	depthPeelingShader->initShaderProgram("DepthPeelingVert.glsl","","","","DepthPeelingFrag.glsl");

	// Load screen filling quad shader.
	GLSLProgram* screenFillingQuadShader = new GLSLProgram();
	screenFillingQuadShader->initShaderProgram("screenFillingQuadVert.glsl","","","","screenFillingQuadFrag.glsl");

	outputShader.push_back(defaultShader);
	outputShader.push_back(weightedSumShader);
	outputShader.push_back(depthPeelingShader);
	outputShader.push_back(screenFillingQuadShader);

	return outputShader;
}
// TODO
void updateShaderUniforms(GraphicsWindow* window, FreeCamera* camera, std::vector<GLSLProgram*> shader)
{
	// Update shader uniforms
	// Default shader
	shader[0]->use();
	shader[0]->setUniform("lightPosition", camera->getCamPos());
	shader[0]->setUniform("camPosition", camera->getCamPos());
	shader[0]->setUniform("viewMatrix", camera->getVMatrix());
	shader[0]->setUniform("normalMatrix", camera->getTranspInvMVMatrix()); // Change this!
	shader[0]->setUniform("VPMatrix", camera->getVPMatrix());
	// Weighted Sum shader
	shader[1]->use();
	shader[1]->setUniform("VPMatrix", camera->getVPMatrix());
	shader[1]->setUniform("windowSize", glm::vec2(window->getWindowWidth(),window->getWindowHeight()));
	// Depth Peeling shader
	shader[2]->use();
	shader[2]->setUniform("VPMatrix", camera->getVPMatrix());
	shader[2]->setUniform("windowSize", glm::vec2(window->getWindowWidth(),window->getWindowHeight()));

	check_gl_error();
}
// TODO
void loadAdditionalTextures(std::vector<std::string> fullTexturePaths)
{
	for (unsigned int i = 0; i < fullTexturePaths.size(); i++)
	{
		TextureManager::GetInstance().loadTexture(fullTexturePaths[i]);
	}

	check_gl_error();
}
// TODO
// Maybe a few variations of this method, like "loadUniformlyDistributedContent()", or "loadArbitarilyDistributedContent()", for the different 
// testing critria of the OIT algorithms.
std::vector<Quad*> loadTransparentQuads()
{
	std::vector<Quad*> outputTransparentQuads;

	// Uniformly depth distributed, transparent quads.
	glm::vec3 transform(0,0,0);

	Quad* quad1 = new Quad(transform + glm::vec3(100+100,100+100,0), transform + glm::vec3(100+100,-100+100,0), transform + glm::vec3(-100+100,-100+100,0), transform + glm::vec3(-100+100,100+100,0), glm::vec3(0), 10, 10, 0);
	quad1->initQuad();
	quad1->initDefaultTexture(175,0,0,125);
	//quad1->setTexture(TextureManager::GetInstance().getTextureHandle("../Content/Textures/Particle_Smoke/smoke_particle_grey_base_2.png"));

	transform = glm::vec3(-1,5,0);

	Quad* quad2 = new Quad(transform + glm::vec3(100+100,100+100,-100), transform + glm::vec3(100+100,-100+100,-100), transform + glm::vec3(-100+100,-100+100,-100), transform + glm::vec3(-100+100,100+100,-100), glm::vec3(0), 10, 10, 0);
	quad2->initQuad();
	quad2->initDefaultTexture(150,55,0,125);
	//quad2->setTexture(TextureManager::GetInstance().getTextureHandle("../Content/Textures/Particle_Smoke/smoke_particle_white_base.png"));
	
	transform = glm::vec3(7,-9,0);

	Quad* quad3 = new Quad(transform + glm::vec3(100+100,100+100,100), transform + glm::vec3(100+100,-100+100,100), transform + glm::vec3(-100+100,-100+100,100), transform + glm::vec3(-100+100,100+100,100), glm::vec3(0), 10, 10, 0);
	quad3->initQuad();
	quad3->initDefaultTexture(150,80,0,125);
	//quad3->setTexture(TextureManager::GetInstance().getTextureHandle("../Content/Textures/Particle_Smoke/smoke_particle_grey_base_3.png"));

	transform = glm::vec3(-5,2,0);

	Quad* quad4 = new Quad(transform + glm::vec3(100+100,100+100,-150), transform + glm::vec3(100+100,-100+100,-150), transform + glm::vec3(-100+100,-100+100,-150), transform + glm::vec3(-100+100,100+100,-150), glm::vec3(0), 10, 10, 0);
	quad4->initQuad();
	quad4->initDefaultTexture(0,105,150,125);

	transform = glm::vec3(1,-2,0);

	Quad* quad5 = new Quad(transform + glm::vec3(100+100,100+100,150), transform + glm::vec3(100+100,-100+100,150), transform + glm::vec3(-100+100,-100+100,150), transform + glm::vec3(-100+100,100+100,150), glm::vec3(0), 10, 10, 0);
	quad5->initQuad();
	quad5->initDefaultTexture(255,0,255,125);

	outputTransparentQuads.push_back(quad1);
	outputTransparentQuads.push_back(quad2);
	outputTransparentQuads.push_back(quad3);
	outputTransparentQuads.push_back(quad4);
	outputTransparentQuads.push_back(quad5);

	check_gl_error();

	return outputTransparentQuads;
}

std::vector<Quad*> loadOpaqueQuads()
{
	std::vector<Quad*> outputOpaqueQuads;

	return outputOpaqueQuads;
}

void renderOpaqueGeometry( Framebuffer* frameBuffer, GLSLProgram* shader,
							std::vector<GLuint> activeColorAttachments, std::vector<Model*> models, 
							std::vector<Quad*> opaqueQuads)
{
	shader->use();

	frameBuffer->clean();
	frameBuffer->bind();
	frameBuffer->bindForRenderPass(activeColorAttachments);

	glClearColor(0.4f,0.6f,0.94f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// Opaque pass state
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	for (unsigned int i = 0; i < models.size(); i++)
	{
		models[i]->renderOpaque();
	}

	for (unsigned int i = 0; i < opaqueQuads.size(); i++)
	{
		opaqueQuads[i]->render();
	}

	//glDisable(GL_CULL_FACE);

	//glEnable(GL_ALPHA_TEST);
	//glAlphaFunc(GL_GREATER,0.2f);
	//
	//for (unsigned int i = 0; i < models.size(); i++)
	//{
	//	models[i]->renderTransparent();
	//}
	//	
	//glDisable(GL_ALPHA_TEST);

	//glDisable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	frameBuffer->unbind();
}

std::vector<Model*> loadModels(unsigned int processFlagsOnModelImport)
{
	std::vector<Model*> outputModels;

	ModelLoader* modelImporter = new ModelLoader();
	modelImporter->importModel("../Content/Models/crytek-sponza/sponza.obj", processFlagsOnModelImport);

	// Create and initialize model
	Model* model = new Model(glm::vec3(0,0,0), "../Content/Models/crytek-sponza/");
	model->loadModel(modelImporter->getScene());

	outputModels.push_back(model);
	
	return outputModels; 
}