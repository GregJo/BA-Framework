#pragma once

#include "..\..\dependencies\gl\include\glew.h"
#include "..\..\dependencies\glm\glm.hpp"
#include "..\..\dependencies\glm\gtx\transform.hpp"
#include "..\..\dependencies\GLFW\include\GLFW\glfw3.h"

class Camera
{
public:
	//! \brief Camera constructor with symmetrical view frustum (perspective, orthogonal).
	Camera(GLdouble fovy, GLdouble aspect, GLdouble left, GLdouble right, GLdouble near, GLdouble far, glm::vec3 position, 
		glm::vec3 up_dir, glm::vec3 look_point, bool perspective);
	//! \brief Constructor with variable view frustum.
	Camera(GLdouble left, GLdouble right, GLdouble top, GLdouble bottom, GLdouble near, GLdouble far, glm::vec3 position, 
		glm::vec3 up_dir, glm::vec3 look_point);
	~Camera();

	void setViewport(unsigned int width, unsigned int height, float pixeloffset_x, float pixeloffset_y);

	virtual glm::vec3 getCamPos()=0;
	virtual void update(float time);
	virtual void update();
	virtual void camControll(GLFWwindow* window);

	glm::vec3 getUpVector() { return m_upDir; }
	glm::vec3 getRightVector() { return m_rightDir; }

	glm::mat4 getVPMatrix() { return m_vp; }
	glm::mat4 getVMatrix() { return m_view; }
	glm::mat4 getPMatrix() { return m_projection; }
	glm::mat4 getTranspInvMVMatrix() { return glm::transpose(glm::inverse(m_view)); }

protected:

	glm::vec3 m_position;
	glm::vec3 m_lookPoint;
	glm::vec3 m_upPoint;
	glm::vec3 m_rightPoint;

	glm::vec3 m_lookDir;
	glm::vec3 m_upDir;
	glm::vec3 m_rightDir;

	glm::mat4 m_vp;
	glm::mat4 m_mv;
	glm::mat4 m_projection;
	glm::mat4 m_view;

	unsigned int m_viewPortWidth; 
	unsigned int m_viewPortHeight; 

private:
	GLdouble m_left; 
	GLdouble m_right; 
	GLdouble m_bottom; 
	GLdouble m_top;
	GLdouble m_fovy;

	GLdouble m_aspect;
	GLdouble m_near;
	GLdouble m_far;

	GLdouble m_pixeloffset_x; //!< 
	GLdouble m_pixeloffset_y; //!< 
};