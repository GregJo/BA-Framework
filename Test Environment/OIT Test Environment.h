#pragma once 

#include "FreeCamera.h"
#include "GLSLProgram.h"
#include "ModelLoader.h"
#include "Quad.h"
#include "Model.h"
#include "Smoke Particle System.h"
#include "Framebuffer.h"
#include "TextureManager.h"
#include "OGLErrorHandling.h"

#include <iostream>


enum Shader
{
	DEFAULT_SHADER, /*WEIGHTED_SUM_SHADER, DEPTH_PEELING_SHADER,*/ BILLBOARD_SHADER, SCREEN_FILLING_SHADER 
};

// TODO
std::vector<GLSLProgram*> loadShader();
// TODO
void updateShaderUniforms(GraphicsWindow* window, FreeCamera* camera, std::vector<GLSLProgram*> shader);
// TODO
void loadAdditionalTextures(std::vector<std::string> fullTexturePaths);
// TODO
// Maybe a few variations of this method, like "loadUniformlyDistributedContent()", or "loadArbitarilyDistributedContent()", for the different 
// testing critria of the OIT algorithms.
std::vector<Quad*> loadTransparentQuads(float scale);

std::vector<Quad*> loadBillboards(GLSLProgram* billboardShader, float scale);

std::vector<SmokeParticleSystem*> loadSmokeParticleSystem(FreeCamera* camera, float scale);

std::vector<Quad*> loadOpaqueQuads(float scale);

void renderOpaqueGeometry( Framebuffer* frameBuffer, GLSLProgram* shader,
							std::vector<GLuint> activeColorAttachments, std::vector<Model*> models, 
							std::vector<Quad*> opaqueQuads, std::vector<Quad*> opaqueBillboards,
							float gloabalScale);
void renderBillboards(GLSLProgram* shader, std::vector<GLuint> activeColorAttachments, std::vector<Quad*> billoards);

std::vector<Model*> loadModels(unsigned int processFlagsOnModelImport, float scale);