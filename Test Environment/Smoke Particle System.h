#pragma once
#include "Quad.h"
#include "GLSLProgram.h"
#include "OGLErrorHandling.h"
#include "FreeCamera.h"
#include "GraphicsWindow.h"
#include <vector>


class SmokeParticleSystem
{

public:

	SmokeParticleSystem(unsigned int m_smokeParticlesCount, unsigned int smokeParticlesWidth, unsigned int smokeParticlesHeight, glm::vec3 smokeParticleCenter, glm::vec3 smokeParticleDirection, float velocity, float lifeSpan);
	~SmokeParticleSystem();

	void setGlobalScale(float globalScale)
	{
		m_globalScale = globalScale;
	}

	void init(FreeCamera* camera, std::string smokeParticleTexturePath);
	void setShader(GLSLProgram* shader);

	void update(FreeCamera* camera, float time);
	void render(GLSLProgram* shader, FreeCamera* camera, unsigned int windowWidth, unsigned int windowHeight);
	void render(GLSLProgram* shader, FreeCamera* camera, unsigned int windowWidth, unsigned int windowHeight, std::vector<GLuint> additionalTextures, GLuint sampler);

	//void setSmokeParticleScale(float smokeParticleScale)
	//{
	//	m_smokeParticlesScale  = smokeParticleScale;
	//}
	void setSmokeParticleCenter(glm::vec3 smokeParticleCenter);
	void setSmokeParticleDirection(glm::vec3 smokeParticleDirection);

	glm::vec3 getSmokeParticleCenter();
	glm::vec3 getSmokeParticleDirection();
	float getSmokeParticleScale();

	float getSmokeParticleCurrentLifeSpan(unsigned int i)
	{
		return m_smokeParticleLifeSpan[i];
	}
	const unsigned int getSmokeParticleCount()
	{
		return m_smokeParticlesCount;
	}

private:
	float m_globalScale;

	const unsigned int m_smokeParticlesCount;

	//float m_smokeParticlesScale;

	std::vector<Quad*> m_smokeParticles;

	glm::vec3 m_smokeParticleCenter;
	glm::vec3 m_baseSmokeParticleDirection;
	std::vector<float> m_relativeXDirectionScale;
	std::vector<glm::vec3> m_smokeParticleDirections;
	//std::vector<glm::vec3> m_smokeParticleRelativeXDirections;

	std::vector<glm::vec3> m_smokeParticleRelativePositions;

	float m_baseSmokeParticleVelocity;
	std::vector<float> m_smokeParticleVelocities;

	float m_baseSmokeParticleLifespan;
	std::vector<float> m_smokeParticleLifeSpan;

	const int m_smokeParticlesWidth;
	const int m_smokeParticlesHeight;

	glm::vec3 m_smokeParticleRightVector;
};