#pragma once
#include "FreeCamera.h"
#include "Model.h"
#include "Quad.h"
#include "Framebuffer.h"
#include <vector>

class IOITAlgorithm
{
public:
	IOITAlgorithm(void){}
	virtual ~IOITAlgorithm(void){}

	virtual void VInit(GraphicsWindow* window) = 0;
	virtual void VAlgorithm(FreeCamera* camera, std::vector<Model*> models, std::vector<Quad*> transparentQuads, 
							Framebuffer* frameBufferOpaque, GLSLProgram* defaultShader, 
							GLuint sampler) = 0;
	virtual GLuint VGetResultTextureHandle() = 0;
};