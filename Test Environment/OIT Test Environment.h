#pragma once 

#include "FreeCamera.h"
#include "GLSLProgram.h"
#include "ModelLoader.h"
#include "Quad.h"
#include "Model.h"
#include "Framebuffer.h"
#include "TextureManager.h"
#include "OGLErrorHandling.h"

#include <iostream>


enum Shader
{
	DEFAULT_SHADER, WEIGHTED_SUM_SHADER, DEPTH_PEELING_SHADER, SCREEN_FILLING_SHADER 
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
std::vector<Quad*> loadTransparentQuads();

std::vector<Quad*> loadOpaqueQuads();

void renderOpaqueGeometry( Framebuffer* frameBuffer, GLSLProgram* shader,
							std::vector<GLuint> activeColorAttachments, std::vector<Model*> models, 
							std::vector<Quad*> opaqueQuads);

std::vector<Model*> loadModels(unsigned int processFlagsOnModelImport);