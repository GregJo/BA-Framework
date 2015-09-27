#pragma once
#define _USE_MATH_DEFINES 
#include "FreeCamera.h"
#include "OGLErrorHandling.h"
#include "gtc/matrix_transform.hpp"
#include "gtx/rotate_vector.hpp"

#include <iostream>

FreeCamera::FreeCamera(GLdouble fovy, GLdouble aspect, 
						GLdouble left, GLdouble right, 
						GLdouble near, GLdouble far, 
						float sensitivity_rotate, 
						float sensitivity_move, 
						glm::vec3 position, glm::vec3 up_dir, 
						glm::vec3 look_point, bool perspective)
	: Camera(fovy, aspect, left, right, near, far, position, up_dir, look_point, perspective), 
	m_alpha(0),
	m_beta(0),
	m_sensitivity_rotate(sensitivity_rotate),
	m_sensitivity_move(sensitivity_move),
	m_move_vec(glm::vec3(0))
{
	m_lookDir = glm::normalize(look_point-position);
	m_alpha = atan2(m_lookDir.z,m_lookDir.x);
	m_beta = acos(m_lookDir.y);
}

FreeCamera::FreeCamera(GLdouble left, GLdouble right, 
						GLdouble top, GLdouble bottom, 
						GLdouble near, GLdouble far, 
						float sensitivity_rotate, 
						float sensitivity_move, 
						glm::vec3 position, glm::vec3 up_dir, 
						glm::vec3 look_point)
	: Camera(left, right, top, bottom, near, far, position, up_dir, look_point),
	m_alpha(0),
	m_beta(0),
	m_sensitivity_rotate(sensitivity_rotate),
	m_move_vec(glm::vec3(0))
{
	m_lookDir = glm::normalize(look_point-position);
	m_alpha = atan2(m_lookDir.x,m_lookDir.z);
	m_beta = acos(m_lookDir.y);
}

FreeCamera::~FreeCamera()
{
	Camera::~Camera();
}

float clip(float n, float lower, float upper) 
{
	return std::max(lower, std::min(n, upper));
}

void FreeCamera::camRotation(GLFWwindow* window)
{
	double currentX = 0, currentY = 0;
	glfwGetCursorPos(window, &currentX, &currentY);

	double delta_X = (currentX - (double) (m_viewPortWidth /2.0)); 
	double delta_Y = (currentY - (double) (m_viewPortHeight /2.0));

	m_alpha += m_sensitivity_rotate * M_PI * delta_X;
	m_beta += m_sensitivity_rotate * M_PI * delta_Y;

	//std::cout<< "Y-Beta: " << m_beta <<std::endl;

	m_beta = clip(float(m_beta),0.14f,3.0f);

	glfwSetCursorPos(window, (double) m_viewPortWidth /2.0, (double) m_viewPortHeight /2.0);
}

void FreeCamera::camTranslation(GLFWwindow* window)
{
	int left = glfwGetKey(window, GLFW_KEY_LEFT);
	int right = glfwGetKey(window, GLFW_KEY_RIGHT);
	int up = glfwGetKey(window, GLFW_KEY_UP);
	int down = glfwGetKey(window, GLFW_KEY_DOWN);

	m_move_vec = glm::vec3(0);

	if (up == GLFW_PRESS)
		m_move_vec += (m_lookDir*m_sensitivity_move);
	if (down == GLFW_PRESS)
		m_move_vec -= (m_lookDir*m_sensitivity_move);
	if (left == GLFW_PRESS)
		m_move_vec += (m_rightDir*m_sensitivity_move);
	if (right == GLFW_PRESS)
		m_move_vec -= (m_rightDir*m_sensitivity_move);
}

void FreeCamera::camControll(GLFWwindow* window)
{
	camRotation(window);
	camTranslation(window);
}

void FreeCamera::setCursorPos(GLFWwindow* window, double setX, double setY)
{
	glfwSetCursorPos(window, setX, setY);
}

void FreeCamera::setCursorPos(GLFWwindow* window)
{
	glfwSetCursorPos(window, (double) m_viewPortWidth /2, (double) m_viewPortHeight /2);
}

void FreeCamera::update()
{
	m_position += m_move_vec;

	m_lookDir.x = float(sin(m_beta) * cos(m_alpha));
	m_lookDir.y = float(cos(m_beta));
	m_lookDir.z = float(sin(m_beta) * sin(m_alpha));
	m_lookDir = glm::normalize(m_lookDir);

	float camOrientation1 = glm::dot(glm::normalize(glm::abs(glm::vec3(m_lookDir.x,0,m_lookDir.z))),m_lookDir);
	float camOrientation2 = glm::dot(glm::normalize(glm::vec3(0,1,0)),m_lookDir);

	//std::cout<< "Dot lookDirXZ-lookDir: " << camOrientation1 <<std::endl;
	//std::cout<< "Dot up-lookDir:        " << camOrientation2 <<std::endl<<std::endl;

	if(camOrientation1 >= 0.0f||camOrientation2 >= 0.0f)
	{
		m_rightDir = glm::normalize(-glm::cross(m_lookDir,glm::vec3(0,1,0)));

		m_upDir = glm::normalize(glm::cross(m_lookDir,m_rightDir));
		//m_up_dir = glm::vec3(0,1,0);
	}
	else 
	{
		m_rightDir = glm::normalize(-glm::cross(m_lookDir,glm::vec3(0,1,0)));

		m_upDir = glm::normalize(glm::cross(m_lookDir,m_rightDir));
		//m_up_dir = glm::vec3(0,-1,0);
	}
	glm::mat4 view = glm::lookAt(m_position, (m_position+m_lookDir), m_upDir);

	m_vp = m_projection * view;

	// Calculate the inverted X, Z rotation view matrix.
	glm::mat4 inverseXZRotationView = glm::inverse(glm::lookAt(m_position, (m_position+glm::vec3(m_lookDir.x,0.0f,m_lookDir.z)), m_upDir));

	m_inverseXZRotationVP = m_projection * inverseXZRotationView;

	check_gl_error();
}