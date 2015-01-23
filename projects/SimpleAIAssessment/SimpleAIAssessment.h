#pragma once

#include "Application.h"
#include <glm/glm.hpp>

class Agent; 
class Behaviour; 

// derived application class that wraps up all globals neatly
class SimpleAIAssessment : public Application
{
public:

	SimpleAIAssessment();
	virtual ~SimpleAIAssessment();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	Agent* m_agent; 
	Agent* m_runner; 

	Behaviour* m_behaviour; 
	Behaviour* m_behaviour2;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};