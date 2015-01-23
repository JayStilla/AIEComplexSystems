#include "SimpleAIAssessment.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>

#include "Agent.h"



#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

SimpleAIAssessment::SimpleAIAssessment()
{

}

SimpleAIAssessment::~SimpleAIAssessment()
{

}

bool SimpleAIAssessment::onCreate(int a_argc, char* a_argv[]) 
{
	// initialise the Gizmos helper class
	Gizmos::create();

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );

	// get window dimensions to calculate aspect ratio
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);

	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, width / (float)height, 0.1f, 1000.0f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	m_agent = new Agent();
	m_runner = new Agent();

	m_agent->enemy = m_runner; 
	m_runner->enemy = m_agent; 

	m_agent->hp = 100;
	m_runner->hp = 100;

	m_agent->setPosition(glm::vec3(12, 0, 12));
	m_agent->setTarget(glm::vec3(12, 0, 12));

	m_runner->setPosition(glm::vec3(0, 0, 0));
	m_runner->setTarget(glm::vec3(0, 0, 0));

	Behaviour* seek = new SeekTarget(10); 
	Behaviour* random = new RandomiseTarget(10); 
	Behaviour* within = new WithinRange(0.5f); 

	//Generates wander nodes
	Sequence* seq = new Sequence(); 
	seq->addChild(within); 
	seq->addChild(random);  

	//attack target
	Behaviour* withinEnemy = new WithinEnemyRange(5); 
	Behaviour* seekEnemy = new SeekEnemy(4, 0.05f);
	Behaviour* flee = new FleeEnemy(6); 

	Sequence* att = new Sequence(); 
	att->addChild(withinEnemy); 
	att->addChild(seekEnemy); 

	Selector* root = new Selector();
	root->addChild(seq);
	root->addChild(att); 
	root->addChild(seek);

	Selector* root2 = new Selector();
	root2->addChild(seq);
	root2->addChild(flee);
	root2->addChild(att);
	root2->addChild(seek);

	m_behaviour = root;
	m_behaviour2 = root2;

	m_agent->setBehaviour(m_behaviour);
	m_runner->setBehaviour(m_behaviour2);


	return true;
}

void SimpleAIAssessment::onUpdate(float a_deltaTime) 
{
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	// clear all gizmos from last frame
	Gizmos::clear();
	
	// add an identity matrix gizmo
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );

	// add a 20x20 grid on the XZ-plane
	for ( int i = 0 ; i < 21 ; ++i )
	{
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10), 
						 i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i), 
						 i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	// TELEPORT OUT OF SIGHT
	if (m_agent->hp <= 0) {
		m_agent->dead = true;
		m_agent->setPosition(glm::vec3(999, 999, 999));
	}

	if (m_runner->hp <= 0) {
		m_runner->dead = true;
		m_runner->setPosition(glm::vec3(-999, -999, -999));
	}


	m_agent->update(a_deltaTime);
	m_runner->update(a_deltaTime);

	Gizmos::addAABBFilled(m_agent->getPosition(),
		glm::vec3(0.5f), glm::vec4(1, 0, 0, 1));

	if (m_runner->hp > 0)
	{
		Gizmos::addAABBFilled(m_runner->getPosition() + glm::vec3(0, 0, 0),
			glm::vec3(0.5f), glm::vec4(0, 0, 1, 1));
	}


	// TARGETS
	Gizmos::addAABBFilled(m_agent->getTarget(),
		glm::vec3(0.1f), glm::vec4(1, 0, 0, 1));

	Gizmos::addAABBFilled(m_runner->getTarget(),
		glm::vec3(0.1f), glm::vec4(1, 0, 0, 1));

	// quit our application when escape is pressed
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();

	if (m_runner->hp < 100)
		m_runner->hp += .003f;
	//cout << m_runner->hp << endl;

	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void SimpleAIAssessment::onDraw() 
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	// draw the gizmos from this frame
	Gizmos::draw(m_projectionMatrix, viewMatrix);

	// get window dimensions for 2D orthographic projection
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	Gizmos::draw2D(glm::ortho<float>(0, width, 0, height, -1.0f, 1.0f));
}

void SimpleAIAssessment::onDestroy()
{
	// clean up anything we created
	Gizmos::destroy();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new SimpleAIAssessment();
	
	if (app->create("AIE - SimpleAIAssessment",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}