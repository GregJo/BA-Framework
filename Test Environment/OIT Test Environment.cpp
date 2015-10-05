#pragma once

#include "OIT Test Environment.h"

// TODO
std::vector<GLSLProgram*> loadShader()
{
	std::vector<GLSLProgram*> outputShader;

	// Load default shader.
	GLSLProgram* defaultShader = new GLSLProgram();
	defaultShader->initShaderProgram("Vert.glsl","","","","Frag.glsl");

	//// Load Weighted Sum shader for alpha blending.
	//GLSLProgram* weightedSumShader = new GLSLProgram();
	//weightedSumShader->initShaderProgram("WeightedSumVert.glsl","","","","WeightedSumFrag.glsl");

	//// Load Depth Peeling shader for the depth peeling reference solution.
	//GLSLProgram* depthPeelingShader = new GLSLProgram();
	//depthPeelingShader->initShaderProgram("DepthPeelingVert.glsl","","","","DepthPeelingFrag.glsl");

	// Load billboard shader.
	GLSLProgram* billboardShader = new GLSLProgram();
	billboardShader->initShaderProgram("BillboardVert.glsl","","","","BillboardFrag.glsl");

	// Load screen filling quad shader.
	GLSLProgram* screenFillingQuadShader = new GLSLProgram();
	screenFillingQuadShader->initShaderProgram("screenFillingQuadVert.glsl","","","","screenFillingQuadFrag.glsl");

	outputShader.push_back(defaultShader);
	//outputShader.push_back(weightedSumShader);
	//outputShader.push_back(depthPeelingShader);
	outputShader.push_back(billboardShader);
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
	//// Weighted Sum shader
	//shader[1]->use();
	//shader[1]->setUniform("VPMatrix", camera->getVPMatrix());
	//shader[1]->setUniform("windowSize", glm::vec2(window->getWindowWidth(),window->getWindowHeight()));
	//// Depth Peeling shader
	//shader[2]->use();
	//shader[2]->setUniform("VPMatrix", camera->getVPMatrix());
	//shader[2]->setUniform("windowSize", glm::vec2(window->getWindowWidth(),window->getWindowHeight()));

	glm::mat4 cameraViewMatrix(camera->getVMatrix());

	//cameraViewMatrix[0][0] = 1;
	//cameraViewMatrix[0][1] = 0;
	//cameraViewMatrix[0][2] = 0;

	//// Column 2:
	//cameraViewMatrix[2][0] = 0;
	//cameraViewMatrix[2][1] = 0;
	//cameraViewMatrix[2][2] = 1;

	shader[1]->use();
	shader[1]->setUniform("VPMatrix", camera->getVPMatrix());
	//shader[1]->setUniform("PMatrix", camera->getPMatrix());
	//shader[1]->setUniform("worldspaceParticleCenter", glm::vec3(300,200,0));
	//shader[1]->setUniform("worldspaceCameraRightVector", glm::vec3(cameraViewMatrix[0][0], cameraViewMatrix[1][0], cameraViewMatrix[2][0]));
	//shader[1]->setUniform("worldspaceCameraUpVector", glm::vec3(cameraViewMatrix[0][1], cameraViewMatrix[1][1], cameraViewMatrix[2][1]));
	shader[1]->setUniform("worldspaceCameraRightVector", camera->getRightVector());
	shader[1]->setUniform("worldspaceCameraUpVector", camera->getUpVector());


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
std::vector<Quad*> loadTransparentQuads(float scale)
{
	std::vector<Quad*> outputTransparentQuads;

	// Nearly uniformly depth distributed, transparent quads (inside Sponza).
	//______________________________________________________________________________________________________________________________________________________________________________________________
	glm::vec3 translate(0,100,0);

	Quad* quad1 = new Quad(translate + glm::vec3(100,100,0), translate + glm::vec3(100,-100,0), translate + glm::vec3(-100,-100,0), translate + glm::vec3(-100,100,0), glm::vec3(0), 10, 10, 0);
	quad1->initQuad();
	quad1->initDefaultTexture(175,0,0,125);
	quad1->setScale(scale);

	translate = glm::vec3(-1,105,0);

	Quad* quad2 = new Quad(translate + glm::vec3(100,100,-100), translate + glm::vec3(100,-100,-100), translate + glm::vec3(-100,-100,-100), translate + glm::vec3(-100,100,-100), glm::vec3(0), 10, 10, 0);
	quad2->initQuad();
	quad2->initDefaultTexture(150,55,0,125);
	quad2->setScale(scale);

	translate = glm::vec3(7,91,0);

	Quad* quad3 = new Quad(translate + glm::vec3(100,100,100), translate + glm::vec3(100,-100,100), translate + glm::vec3(-100,-100,100), translate + glm::vec3(-100,100,100), glm::vec3(0), 10, 10, 0);
	quad3->initQuad();
	quad3->initDefaultTexture(150,80,0,125);
	quad3->setScale(scale);

	translate = glm::vec3(-5,102,0);

	Quad* quad4 = new Quad(translate + glm::vec3(100,100,-150), translate + glm::vec3(100,-100,-150), translate + glm::vec3(-100,-100,-150), translate + glm::vec3(-100,100,-150), glm::vec3(0), 10, 10, 0);
	quad4->initQuad();
	quad4->initDefaultTexture(0,105,150,125);
	quad4->setScale(scale);

	translate = glm::vec3(1,98,0);

	Quad* quad5 = new Quad(translate + glm::vec3(100,100,150), translate + glm::vec3(100,-100,150), translate + glm::vec3(-100,-100,150), translate + glm::vec3(-100,100,150), glm::vec3(0), 10, 10, 0);
	quad5->initQuad();
	quad5->initDefaultTexture(255,0,255,125);
	quad5->setScale(scale);

	//______________________________________________________________________________________________________________________________________________________________________________________________

	// Uniformly depth distributed, displaced transparent quads (outside Sponza, black opaque quad as background).
	//______________________________________________________________________________________________________________________________________________________________________________________________
	translate = glm::vec3(0,2000,-200);
	translate += glm::vec3(-100,-100,0);

	Quad* quad6 = new Quad(translate + glm::vec3(100,100,0), translate + glm::vec3(100,-100,0), translate + glm::vec3(-100,-100,0), translate + glm::vec3(-100,100,0), glm::vec3(0), 10, 10, 0);
	quad6->initQuad();
	quad6->initDefaultTexture(175,0,0,125);
	quad6->setScale(scale);

	translate = glm::vec3(0,2000,-100);
	translate += glm::vec3(-50,-50,0);

	Quad* quad7 = new Quad(translate + glm::vec3(100,100,0), translate + glm::vec3(100,-100,0), translate + glm::vec3(-100,-100,0), translate + glm::vec3(-100,100,0), glm::vec3(0), 10, 10, 0);
	quad7->initQuad();
	quad7->initDefaultTexture(150,55,0,125);
	quad7->setScale(scale);

	translate = glm::vec3(0,2000,0);
	translate += glm::vec3(0,0,0);

	Quad* quad8 = new Quad(translate + glm::vec3(100,100,0), translate + glm::vec3(100,-100,0), translate + glm::vec3(-100,-100,0), translate + glm::vec3(-100,100,0), glm::vec3(0), 10, 10, 0);
	quad8->initQuad();
	quad8->initDefaultTexture(150,80,0,125);
	quad8->setScale(scale);

	translate = glm::vec3(0,2000,100);
	translate += glm::vec3(50,50,0);

	Quad* quad9 = new Quad(translate + glm::vec3(100,100,0), translate + glm::vec3(100,-100,0), translate + glm::vec3(-100,-100,0), translate + glm::vec3(-100,100,0), glm::vec3(0), 10, 10, 0);
	quad9->initQuad();
	quad9->initDefaultTexture(0,105,150,125);
	quad9->setScale(scale);

	translate = glm::vec3(0,2000,200);
	translate += glm::vec3(100,100,0);

	Quad* quad10 = new Quad(translate + glm::vec3(100,100,0), translate + glm::vec3(100,-100,0), translate + glm::vec3(-100,-100,0), translate + glm::vec3(-100,100,0), glm::vec3(0), 10, 10, 0);
	quad10->initQuad();
	quad10->initDefaultTexture(255,0,255,125);
	quad10->setScale(scale);

	//______________________________________________________________________________________________________________________________________________________________________________________________

	// Uniformly depth distributed, transparent quads (outside Sponza, black opaque quad as background, intersecting).
	//______________________________________________________________________________________________________________________________________________________________________________________________
	translate = glm::vec3(1000,2000,-200);

	Quad* quad11 = new Quad(translate + glm::vec3(100,100,0), translate + glm::vec3(100,-100,0), translate + glm::vec3(-100,-100,0), translate + glm::vec3(-100,100,0), glm::vec3(0), 10, 10, 0);
	quad11->initQuad();
	quad11->initDefaultTexture(175,0,0,125);
	quad11->setScale(scale);

	translate = glm::vec3(1000,2000,-100);

	Quad* quad12 = new Quad(translate + glm::vec3(100,100,0), translate + glm::vec3(100,-100,0), translate + glm::vec3(-100,-100,0), translate + glm::vec3(-100,100,0), glm::vec3(0), 10, 10, 0);
	quad12->initQuad();
	quad12->initDefaultTexture(150,55,0,125);
	quad12->setScale(scale);

	translate = glm::vec3(1000,2000,0);

	Quad* quad13 = new Quad(translate + glm::vec3(100,100,0), translate + glm::vec3(100,-100,0), translate + glm::vec3(-100,-100,0), translate + glm::vec3(-100,100,0), glm::vec3(0), 10, 10, 0);
	quad13->initQuad();
	quad13->initDefaultTexture(150,80,0,125);
	quad13->setScale(scale);

	translate = glm::vec3(1000,2000,100);

	Quad* quad14 = new Quad(translate + glm::vec3(100,100,0), translate + glm::vec3(100,-100,0), translate + glm::vec3(-100,-100,0), translate + glm::vec3(-100,100,0), glm::vec3(0), 10, 10, 0);
	quad14->initQuad();
	quad14->initDefaultTexture(0,105,150,125);
	quad14->setScale(scale);

	translate = glm::vec3(1000,2000,200);

	Quad* quad15 = new Quad(translate + glm::vec3(100,100,0), translate + glm::vec3(100,-100,0), translate + glm::vec3(-100,-100,0), translate + glm::vec3(-100,100,0), glm::vec3(0), 10, 10, 0);
	quad15->initQuad();
	quad15->initDefaultTexture(255,0,255,125);
	quad15->setScale(scale);

	translate = glm::vec3(1000,2000,0);

	Quad* quad16 = new Quad(translate + glm::vec3(100,100,200), translate + glm::vec3(100,-100,200), translate + glm::vec3(-100,-100,-200), translate + glm::vec3(-100,100,-200), glm::vec3(0), 10, 10, 0);
	quad16->initQuad();
	quad16->initDefaultTexture(0,255,0,125);
	quad16->setScale(scale);

	//______________________________________________________________________________________________________________________________________________________________________________________________

	//Arbitarily depth distributed, transparent quads (outside Sponza, black opaque quad as background).
	//______________________________________________________________________________________________________________________________________________________________________________________________
	translate = glm::vec3(-1000,2000,-475);
	translate += glm::vec3(-100,-100,0);

	Quad* quad17 = new Quad(translate + glm::vec3(100,100,-300), translate + glm::vec3(100,-100,0), translate + glm::vec3(-100,-100,300), translate + glm::vec3(-100,100,0), glm::vec3(0), 10, 10, 0);
	quad17->initQuad();
	quad17->initDefaultTexture(175,0,0,125);
	quad17->setScale(scale);

	translate = glm::vec3(-1000,2000,-125);
	translate += glm::vec3(-50,-50,0);

	Quad* quad18 = new Quad(translate + glm::vec3(100,100,100), translate + glm::vec3(100,-100,0), translate + glm::vec3(-100,-100,0), translate + glm::vec3(-100,100,100), glm::vec3(0), 10, 10, 0);
	quad18->initQuad();
	quad18->initDefaultTexture(150,55,0,125);
	quad18->setScale(scale);

	translate = glm::vec3(-1000,2000,0);
	translate += glm::vec3(0,0,0);

	Quad* quad19 = new Quad(translate + glm::vec3(100,100,0), translate + glm::vec3(100,-100,300), translate + glm::vec3(-100,-100,300), translate + glm::vec3(-100,100,0), glm::vec3(0), 10, 10, 0);
	quad19->initQuad();
	quad19->initDefaultTexture(150,80,0,125);
	quad19->setScale(scale);

	translate = glm::vec3(-1000,2000,275);
	translate += glm::vec3(50,50,0);

	Quad* quad20 = new Quad(translate + glm::vec3(100,100,0), translate + glm::vec3(100,-100,0), translate + glm::vec3(-100,-100,-200), translate + glm::vec3(-100,100,-200), glm::vec3(0), 10, 10, 0);
	quad20->initQuad();
	quad20->initDefaultTexture(0,105,150,125);
	quad20->setScale(scale);

	translate = glm::vec3(-1000,2000,475);
	translate += glm::vec3(100,100,0);

	Quad* quad21 = new Quad(translate + glm::vec3(100,100,200), translate + glm::vec3(100,-100,200), translate + glm::vec3(-100,-100,0), translate + glm::vec3(-100,100,0), glm::vec3(0), 10, 10, 0);
	quad21->initQuad();
	quad21->initDefaultTexture(255,0,255,125);
	quad21->setScale(scale);

	//______________________________________________________________________________________________________________________________________________________________________________________________
	
	//Uniformly depth distributed, transparent quads (outside Sponza, black opaque quad as background, great depth range).
	//______________________________________________________________________________________________________________________________________________________________________________________________
	translate = glm::vec3(-1200,4000,0);

	Quad* quad22 = new Quad(translate + glm::vec3(0,200,200), translate + glm::vec3(0,200,-200), translate + glm::vec3(0,-200,-200), translate + glm::vec3(0,-200,200), glm::vec3(0), 10, 10, 0);
	quad22->initQuad();
	quad22->initDefaultTexture(175,0,0,125);
	quad22->setScale(scale);

	translate = glm::vec3(-800,4000,0);

	Quad* quad23 = new Quad(translate + glm::vec3(0,200,200), translate + glm::vec3(0,200,-200), translate + glm::vec3(0,-200,-200), translate + glm::vec3(0,-200,200), glm::vec3(0), 10, 10, 0);
	quad23->initQuad();
	quad23->initDefaultTexture(150,55,0,125);
	quad23->setScale(scale);

	translate = glm::vec3(-400,4000,0);

	Quad* quad24 = new Quad(translate + glm::vec3(0,200,200), translate + glm::vec3(0,200,-200), translate + glm::vec3(0,-200,-200), translate + glm::vec3(0,-200,200), glm::vec3(0), 10, 10, 0);
	quad24->initQuad();
	quad24->initDefaultTexture(150,80,0,125);
	quad24->setScale(scale);

	translate = glm::vec3(0,4000,0);

	Quad* quad25 = new Quad(translate + glm::vec3(0,200,200), translate + glm::vec3(0,200,-200), translate + glm::vec3(0,-200,-200), translate + glm::vec3(0,-200,200), glm::vec3(0), 10, 10, 0);
	quad25->initQuad();
	quad25->initDefaultTexture(0,105,150,125);
	quad25->setScale(scale);

	translate = glm::vec3(400,4000,0);

	Quad* quad26 = new Quad(translate + glm::vec3(0,200,200), translate + glm::vec3(0,200,-200), translate + glm::vec3(0,-200,-200), translate + glm::vec3(0,-200,200), glm::vec3(0), 10, 10, 0);
	quad26->initQuad();
	quad26->initDefaultTexture(255,0,255,125);
	quad26->setScale(scale);

	translate = glm::vec3(800,4000,0);

	Quad* quad27 = new Quad(translate + glm::vec3(0,200,200), translate + glm::vec3(0,200,-200), translate + glm::vec3(0,-200,-200), translate + glm::vec3(0,-200,200), glm::vec3(0), 10, 10, 0);
	quad27->initQuad();
	quad27->initDefaultTexture(150,55,0,125);
	quad27->setScale(scale);

	translate = glm::vec3(1200,4000,0);

	Quad* quad28 = new Quad(translate + glm::vec3(0,200,200), translate + glm::vec3(0,200,-200), translate + glm::vec3(0,-200,-200), translate + glm::vec3(0,-200,200), glm::vec3(0), 10, 10, 0);
	quad28->initQuad();
	quad28->initDefaultTexture(150,80,0,125);
	quad28->setScale(scale);

	//______________________________________________________________________________________________________________________________________________________________________________________________

	// Uniformly depth distributed, displaced transparent quads (outside Sponza, black opaque quad as background).
	outputTransparentQuads.push_back(quad1);
	outputTransparentQuads.push_back(quad2);
	outputTransparentQuads.push_back(quad3);
	outputTransparentQuads.push_back(quad4);
	outputTransparentQuads.push_back(quad5);
	// Uniformly depth distributed, displaced transparent quads (outside Sponza, black opaque quad as background).
	outputTransparentQuads.push_back(quad6);
	outputTransparentQuads.push_back(quad7);
	outputTransparentQuads.push_back(quad8);
	outputTransparentQuads.push_back(quad9);
	outputTransparentQuads.push_back(quad10);
	// Uniformly depth distributed, transparent quads (outside Sponza, black opaque quad as background, intersecting).
	outputTransparentQuads.push_back(quad11);
	outputTransparentQuads.push_back(quad12);
	outputTransparentQuads.push_back(quad13);
	outputTransparentQuads.push_back(quad14);
	outputTransparentQuads.push_back(quad15);
	outputTransparentQuads.push_back(quad16);
	//Arbitarily depth distributed, transparent quads (outside Sponza, black opaque quad as background).
	outputTransparentQuads.push_back(quad17);
	outputTransparentQuads.push_back(quad18);
	outputTransparentQuads.push_back(quad19);
	outputTransparentQuads.push_back(quad20);
	outputTransparentQuads.push_back(quad21);
	//Uniformly depth distributed, transparent quads (outside Sponza, black opaque quad as background, great depth range).
	outputTransparentQuads.push_back(quad22);
	outputTransparentQuads.push_back(quad23);
	outputTransparentQuads.push_back(quad24);
	outputTransparentQuads.push_back(quad25);
	outputTransparentQuads.push_back(quad26);
	outputTransparentQuads.push_back(quad27);
	outputTransparentQuads.push_back(quad28);



	check_gl_error();

	return outputTransparentQuads;
}

std::vector<Quad*> loadBillboards(GLSLProgram* billboardShader, float scale)
{
	std::vector<Quad*> outputBillboards;

	// Billboard quad.
	glm::vec3 transform(0.0f, 0.0f, 0.0f);
	Quad* billboardQuad = new Quad(transform + glm::vec3(100,100,0), transform + glm::vec3(100,-100,0), transform + glm::vec3(-100,-100,0), transform + glm::vec3(-100,100,0), glm::vec3(0), 10, 10, 0);
	billboardQuad->initQuad();
	billboardQuad->initDefaultTexture(255,0,0,125);
	billboardQuad->setPosition(glm::vec3(500,200,0));
	billboardQuad->setGLSLProgram(*billboardShader);

	outputBillboards.push_back(billboardQuad);

	check_gl_error();

	return outputBillboards;
}

std::vector<Quad*> loadOpaqueQuads(float scale)
{
	std::vector<Quad*> outputOpaqueQuads;

	glm::vec3 translate = glm::vec3(0,2000.0,1000.0);

	Quad* quad1 = new Quad(translate + glm::vec3(500,500,0), translate + glm::vec3(500,-500,0), translate + glm::vec3(-500,-500,0), translate + glm::vec3(-500,500,0), glm::vec3(0), 10, 10, 0);
	quad1->initQuad();
	quad1->initDefaultTexture(0,0,0,255);
	quad1->setScale(scale);

	translate = glm::vec3(0,2000.0,-1000.0);

	Quad* quad2 = new Quad(translate + glm::vec3(500,500,0), translate + glm::vec3(500,-500,0), translate + glm::vec3(-500,-500,0), translate + glm::vec3(-500,500,0), glm::vec3(0), 10, 10, 0);
	quad2->initQuad();
	quad2->initDefaultTexture(0,0,0,255);
	quad2->setScale(scale);

	translate = glm::vec3(1000,2000.0,1000.0);

	Quad* quad3 = new Quad(translate + glm::vec3(500,500,0), translate + glm::vec3(500,-500,0), translate + glm::vec3(-500,-500,0), translate + glm::vec3(-500,500,0), glm::vec3(0), 10, 10, 0);
	quad3->initQuad();
	quad3->initDefaultTexture(0,0,0,255);
	quad3->setScale(scale);

	translate = glm::vec3(1000,2000.0,-1000.0);

	Quad* quad4 = new Quad(translate + glm::vec3(500,500,0), translate + glm::vec3(500,-500,0), translate + glm::vec3(-500,-500,0), translate + glm::vec3(-500,500,0), glm::vec3(0), 10, 10, 0);
	quad4->initQuad();
	quad4->initDefaultTexture(0,0,0,255);
	quad4->setScale(scale);

	translate = glm::vec3(-1000,2000.0,1000.0);

	Quad* quad5 = new Quad(translate + glm::vec3(500,500,0), translate + glm::vec3(500,-500,0), translate + glm::vec3(-500,-500,0), translate + glm::vec3(-500,500,0), glm::vec3(0), 10, 10, 0);
	quad5->initQuad();
	quad5->initDefaultTexture(0,0,0,255);
	quad5->setScale(scale);

	translate = glm::vec3(-1000,2000.0,-1000.0);

	Quad* quad6 = new Quad(translate + glm::vec3(500,500,0), translate + glm::vec3(500,-500,0), translate + glm::vec3(-500,-500,0), translate + glm::vec3(-500,500,0), glm::vec3(0), 10, 10, 0);
	quad6->initQuad();
	quad6->initDefaultTexture(0,0,0,255);
	quad6->setScale(scale);

	translate = glm::vec3(2000,4000.0,0);

	Quad* quad7 = new Quad(translate + glm::vec3(0,800,800), translate + glm::vec3(0,800,-800), translate + glm::vec3(0,-800,-800), translate + glm::vec3(0,-800,800), glm::vec3(0), 10, 10, 0);
	quad7->initQuad();
	quad7->initDefaultTexture(0,0,0,255);
	quad7->setScale(scale);

	translate = glm::vec3(-2000,4000.0,0);

	Quad* quad8 = new Quad(translate + glm::vec3(0,800,800), translate + glm::vec3(0,800,-800), translate + glm::vec3(0,-800,-800), translate + glm::vec3(0,-800,800), glm::vec3(0), 10, 10, 0);
	quad8->initQuad();
	quad8->initDefaultTexture(0,0,0,255);
	quad8->setScale(scale);

	outputOpaqueQuads.push_back(quad1);
	outputOpaqueQuads.push_back(quad2);
	outputOpaqueQuads.push_back(quad3);
	outputOpaqueQuads.push_back(quad4);
	outputOpaqueQuads.push_back(quad5);
	outputOpaqueQuads.push_back(quad6);
	outputOpaqueQuads.push_back(quad7);
	outputOpaqueQuads.push_back(quad8);

	return outputOpaqueQuads;
}

// Currently hardcoded values for the smoke particle system, such as smoke particle count, etc.
std::vector<SmokeParticleSystem*> loadSmokeParticleSystem(FreeCamera* camera, float scale)
{
	std::vector<SmokeParticleSystem*> outputSmokeParticleSystems;

	SmokeParticleSystem* smokeParticleSystem = new SmokeParticleSystem(30,400,200,glm::vec3(-600,0,-50),glm::vec3(0,1,0),0.25f,1000.0f);
	smokeParticleSystem->setGlobalScale(scale);
	//smokeParticleSystem->setSmokeParticleScale(scale);
	smokeParticleSystem->init(camera, "../Content/Textures/Particle_Smoke/smoke_particle_grey_base_3.png");

	outputSmokeParticleSystems.push_back(smokeParticleSystem);

	return outputSmokeParticleSystems;
}

void renderOpaqueGeometry( Framebuffer* frameBuffer, GLSLProgram* shader,
							std::vector<GLuint> activeColorAttachments, std::vector<Model*> models, 
							std::vector<Quad*> opaqueQuads, std::vector<Quad*> opaqueBillboards,
							float gloabalScale)
{
	shader->use();
	shader->setUniform("scaleCoeff", gloabalScale);

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

	glDisable(GL_CULL_FACE);

	for (unsigned int i = 0; i < opaqueQuads.size(); i++)
	{
		opaqueQuads[i]->render();
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	renderBillboards(shader, activeColorAttachments, opaqueBillboards);
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

void renderBillboards(GLSLProgram* shader, std::vector<GLuint> activeColorAttachments, std::vector<Quad*> billoards)
{
	if (billoards.size()>0)
	{
		billoards[0]->getCurrentShaderProgram()->use();
		billoards[0]->getCurrentShaderProgram()->setUniform("worldspaceParticleCenter", billoards[0]->getPosition());
		for (unsigned int i = 0; i < billoards.size(); i++)
		{
			billoards[i]->render();
		}
	}

	shader->use();
}

std::vector<Model*> loadModels(unsigned int processFlagsOnModelImport, float scale)
{
	std::vector<Model*> outputModels;

	ModelLoader* modelImporter = new ModelLoader();
	modelImporter->importModel("../Content/Models/crytek-sponza/sponza.obj", processFlagsOnModelImport);

	// Create and initialize model
	Model* model = new Model(glm::vec3(0,0,0), "../Content/Models/crytek-sponza/");
	model->loadModel(modelImporter->getScene());
	model->setScale(scale);

	outputModels.push_back(model);
	
	return outputModels; 
}