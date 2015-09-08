#pragma once
#include "Camera.h"
#include "Logger.h"
#include "glfw3.h"
#include "gtc/matrix_transform.hpp"

Camera::Camera(GLdouble fovy, GLdouble aspect, GLdouble left, GLdouble right, GLdouble near, GLdouble far, glm::vec3 position, 
				glm::vec3 up_dir, glm::vec3 look_point, bool perspective):
																m_fovy(fovy),
																m_aspect(aspect),
																m_left(left),
																m_right(right),
																m_near(near),
																m_far(far),
																m_upDir(glm::normalize(up_dir)),
																m_position(position),
																m_lookPoint(look_point)

{
	if(m_upDir == glm::vec3(0))
	{
		Logger::GetInstance().Log("Camera up vector must not be zero vector.");
		//Either exit(1) or set default values.
		exit(1);
	}

	if(m_position == m_lookPoint)
	{
		Logger::GetInstance().Log("Camera position vector must not be equal to .");
		//Either exit(1) or set default values.
		exit(1);
	}

	m_lookDir = glm::normalize(m_lookPoint-m_position);

	m_rightDir = glm::normalize(glm::cross(m_upDir, m_lookDir));

	m_upPoint = m_position + m_upDir;

	m_rightPoint = m_position + m_rightDir;

	m_view = glm::lookAt(m_position, m_lookPoint, m_upDir);

	if(perspective)
		m_projection = glm::perspective(m_fovy, m_aspect, m_near, m_far);
	else
		m_projection = glm::ortho(m_left, m_right, m_near, m_far);

	m_vp = m_projection * m_view;
}

Camera::Camera(GLdouble left, GLdouble right, GLdouble top, GLdouble bottom, GLdouble near, GLdouble far, glm::vec3 position, glm::vec3 up_dir, glm::vec3 look_point):
																m_left(left),
																m_right(right),
																m_top(top),
																m_bottom(bottom),
																m_near(near),
																m_far(far),
																m_position(position),
																m_upDir(glm::normalize(up_dir))
{
	if(m_upDir == glm::vec3(0))
	{
		Logger::GetInstance().Log("Camera up vector must not be zero vector.");
		//Either exit(1) or set default values.
		exit(1);
	}

	if(m_position == m_lookPoint)
	{
		Logger::GetInstance().Log("Camera position vector must not be equal to .");
		//Either exit(1) or set default values.
		exit(1);
	}

	m_lookDir = m_lookPoint-m_position;

	m_rightDir = glm::normalize(glm::cross(m_upDir, m_lookDir));

	m_upPoint = m_position + m_upDir;

	m_rightPoint = m_position + m_rightDir;

	glm::mat4 view = glm::lookAt(m_position, m_lookPoint, m_upDir);

	m_projection = glm::frustum(m_left, m_right, m_bottom, m_top, m_near, m_far);

	m_vp = m_projection * view;
}

Camera::~Camera()
{}

void Camera::setViewport(unsigned int width, unsigned int height, float pixeloffset_x, float pixeloffset_y)
{
	m_viewPortHeight = height;
	m_viewPortWidth = width;
	m_pixeloffset_x = pixeloffset_x;
	m_pixeloffset_y = pixeloffset_y;

	glViewport(GLint(pixeloffset_x), GLint(pixeloffset_y), width, height);
}

glm::vec3 Camera::getCamPos(){return m_position;}
	
void Camera::update(float time){}
	
void Camera::update(){}
	
void Camera::camControll(GLFWwindow* window){}