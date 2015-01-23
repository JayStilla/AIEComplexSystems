#pragma once

#include "Application.h"
#include <glm/glm.hpp>

// derived application class that wraps up all globals neatly
class Perlin : public Application
{
public:

	Perlin();
	virtual ~Perlin();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::ivec2 m_dimensions; 
	float* m_height; 

	unsigned int m_texture; 

	unsigned int m_vao, m_vbo, m_ibo; 

	unsigned int m_shader; 

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};