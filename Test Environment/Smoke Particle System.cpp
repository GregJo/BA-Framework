#pragma once
#include "Smoke Particle System.h"
#include "TextureManager.h"

SmokeParticleSystem::SmokeParticleSystem(unsigned int smokeParticlesCount, unsigned int smokeParticlesWidth, unsigned int smokeParticlesHeight, glm::vec3 smokeParticleCenter, 
										 glm::vec3 smokeParticleDirection, float velocity, float lifeSpan) : m_smokeParticlesCount(smokeParticlesCount), 
										 m_smokeParticlesWidth(smokeParticlesWidth), m_smokeParticlesHeight(smokeParticlesHeight)
{
	m_smokeParticleCenter = smokeParticleCenter;
	m_baseSmokeParticleDirection = smokeParticleDirection;
	m_baseSmokeParticleVelocity = velocity;
	m_baseSmokeParticleLifespan = lifeSpan;
}

SmokeParticleSystem::~SmokeParticleSystem()
{
}

void SmokeParticleSystem::init(FreeCamera* camera, std::string smokeParticleTexturePath)
{
	TextureManager::GetInstance().loadTexture(smokeParticleTexturePath);

	for (int i = 0; i < m_smokeParticlesCount; i++)
	{
		Quad* quad = new Quad(glm::vec3(m_smokeParticlesWidth,m_smokeParticlesHeight,0), 
										glm::vec3(m_smokeParticlesWidth,-m_smokeParticlesHeight,0), 
										glm::vec3(-m_smokeParticlesWidth,-m_smokeParticlesHeight,0), 
										glm::vec3(-m_smokeParticlesWidth,m_smokeParticlesHeight,0), 
										m_smokeParticleCenter, 0, 0, 0);

		quad->initQuad();
		quad->setTexture(TextureManager::GetInstance().getTextureHandle(smokeParticleTexturePath));

		m_smokeParticles.push_back(quad);

		// Currently hardcoded min/max, move these as parameters to constructor.
		float min = 0.0f;
		float maxLifeSpan = 100000.0;
		float maxVelocity = 1.0;

		// Reset particle. Direction may stay the same.
		float randomLifeSpanOffset = min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxLifeSpan-min)));
		float randomVelocity = m_baseSmokeParticleVelocity * (min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxVelocity-min))));

		// Currently hardcoded min/max, move these as parameters to constructor.
		float minDirection = -1.0f;
		float maxDirection = 1.0;

		m_smokeParticleRightVector = glm::normalize(glm::cross((camera->getCamPos()-m_globalScale*m_smokeParticleCenter), m_baseSmokeParticleDirection));

		m_relativeXDirectionScale.push_back(minDirection + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxDirection-minDirection))));

		//float x = minDirection + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxDirection-minDirection)));
		//float y = minDirection + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxDirection-minDirection)));
		//float z = minDirection + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxDirection-minDirection)));

		//glm::vec3 smokeParticleDir = m_baseSmokeParticleDirection + glm::vec3(x,y,z);

		m_smokeParticleDirections.push_back(glm::normalize(m_smokeParticleRightVector*m_relativeXDirectionScale.back()+m_baseSmokeParticleDirection));

		m_smokeParticleLifeSpan.push_back(m_globalScale*std::abs(m_baseSmokeParticleLifespan + randomLifeSpanOffset));
		m_smokeParticleVelocities.push_back(std::abs(randomVelocity));
		m_smokeParticleRelativePositions.push_back(glm::vec3(0.0f));
	}

	check_gl_error();
}

void SmokeParticleSystem::setShader(GLSLProgram* shader)
{
	for (int i = 0; i < m_smokeParticles.size(); i++)
	{
		m_smokeParticles[i]->setGLSLProgram(*shader);
	}
}

// Parameter time in ms.
void SmokeParticleSystem::update(FreeCamera* camera, float time)
{
	for (int i = 0; i < m_smokeParticles.size(); i++)
	{
		m_smokeParticleRightVector = glm::normalize(glm::cross((camera->getCamPos()-m_globalScale*m_smokeParticleCenter), m_globalScale*m_baseSmokeParticleDirection));

		m_smokeParticleDirections[i] = (glm::normalize(m_smokeParticleRightVector*m_relativeXDirectionScale[i]+m_baseSmokeParticleDirection));

		if (m_smokeParticleLifeSpan[i] <= 0.0f)
		{
			float min = 0.0f;
			float maxLifeSpan = 100000.0;
			float maxVelocity = 1.0;

			// Reset particle. Direction may stay the same.
			float randomLifeSpanOffset = min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxLifeSpan-min)));
			float randomVelocity = m_baseSmokeParticleVelocity * (min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxVelocity-min))));
			m_smokeParticleLifeSpan[i] = m_globalScale*std::abs(m_baseSmokeParticleLifespan + randomLifeSpanOffset);
			m_smokeParticleVelocities[i] = std::abs(randomVelocity);
			m_smokeParticles[i]->setPosition(m_globalScale*m_smokeParticleCenter);
			m_smokeParticleRelativePositions[i] = glm::vec3(0);
		}
		m_smokeParticleRelativePositions[i] += m_smokeParticleVelocities[i]*m_smokeParticleDirections[i];
		m_smokeParticles[i]->setPosition(m_globalScale*(m_smokeParticleCenter+m_smokeParticleRelativePositions[i]));
		m_smokeParticleLifeSpan[i] -= time/1000.0f;
	}
}

void SmokeParticleSystem::render(GLSLProgram* shader, FreeCamera* camera, unsigned int windowWidth, unsigned int windowHeight)
{
	//m_smokeParticleRightVector = glm::normalize(glm::cross((camera->getCamPos()-m_smokeParticleCenter), glm::vec3(0,1,0)));

	shader->use();
	shader->setUniform("scaleCoeff", m_globalScale);
	shader->setUniform("VPMatrix", camera->getVPMatrix());
	shader->setUniform("worldspaceCameraRightVector", m_smokeParticleRightVector);
	shader->setUniform("worldspaceCameraUpVector", glm::vec3(0,1,0));
	shader->setUniform("worldspaceParticleCenter", m_globalScale*m_smokeParticleCenter);
	shader->setUniform("windowSize", glm::vec2(windowWidth,windowHeight));
	for (int i = 0; i < m_smokeParticles.size(); i++)
	{
		shader->setUniform("relativeParticleCenterPosition", m_smokeParticleRelativePositions[i]);
		m_smokeParticles[i]->render();
	}
}

void  SmokeParticleSystem::render(GLSLProgram* shader, FreeCamera* camera, unsigned int windowWidth, unsigned int windowHeight, std::vector<GLuint> additionalTextures, GLuint sampler)
{
	shader->use();
	shader->setUniform("scaleCoeff", m_globalScale);
	shader->setUniform("VPMatrix", camera->getVPMatrix());
	shader->setUniform("worldspaceCameraRightVector", m_smokeParticleRightVector);
	shader->setUniform("worldspaceCameraUpVector", glm::vec3(0,1,0));
	shader->setUniform("worldspaceParticleCenter", m_globalScale*m_smokeParticleCenter);
	shader->setUniform("windowSize", glm::vec2(windowWidth,windowHeight));
	for (int i = 0; i < m_smokeParticles.size(); i++)
	{
		shader->setUniform("relativeParticleCenterPosition", m_smokeParticleRelativePositions[i]);
		m_smokeParticles[i]->renderWithAdditionalTextures(additionalTextures, sampler);
	}
}

void SmokeParticleSystem::setSmokeParticleCenter(glm::vec3 smokeParticleCenter)
{
	m_smokeParticleCenter = smokeParticleCenter;
}
void SmokeParticleSystem::setSmokeParticleDirection(glm::vec3 smokeParticleDirection)
{
	m_baseSmokeParticleDirection = smokeParticleDirection;
}

glm::vec3 SmokeParticleSystem::getSmokeParticleCenter()
{
	return m_smokeParticleCenter;
}

glm::vec3 SmokeParticleSystem::getSmokeParticleDirection()
{
	return m_baseSmokeParticleDirection;
}