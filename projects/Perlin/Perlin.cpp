#include "Perlin.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

Perlin::Perlin()
{

}

Perlin::~Perlin()
{

}

bool Perlin::onCreate(int a_argc, char* a_argv[]) 
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

	Utility::build3DPlane(50, m_vao, m_vbo, m_ibo);

	unsigned int vs = Utility::loadShader("./shaders/3dQuad.vert", GL_VERTEX_SHADER); 
	unsigned int fs = Utility::loadShader("./shaders/redOnly.frag", GL_FRAGMENT_SHADER); 
	unsigned int cs = Utility::loadShader("./shaders/displace.cont", GL_TESS_CONTROL_SHADER);
	unsigned int es = Utility::loadShader("./shaders/displace.eval", GL_TESS_EVALUATION_SHADER);
	m_shader = Utility::createProgram(vs, cs, es, 0, fs); 
	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteShader(cs);
	glDeleteShader(es);

	m_dimensions = glm::ivec2(256, 256); 
	m_height = new float[m_dimensions.x * m_dimensions.y]; 

	int layers = 8; 
	float amplitude = 0.9f;
	glm::vec2 size = 1.0f / glm::vec2(m_dimensions - 1); 

	for (int x = 0; x < m_dimensions.x; ++x)
	{
		for (int y = 0; y < m_dimensions.y; ++y)
		{
			float value = 0; 

			for (int i = 0; i < layers; ++i)
			{
				float freq = (float)glm::pow(2, i); 
				float amp = glm::pow(amplitude, (float)i); 
				value += glm::perlin(glm::vec2(x, y) * size * freq) * amp; 
			}

			m_height[x * m_dimensions.y + y] = value * 0.5f + 0.5f; 
		}
	}

	glGenTextures(1, &m_texture); 
	glBindTexture(GL_TEXTURE_2D, m_texture); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_dimensions.x, m_dimensions.y, 0, GL_RED, GL_FLOAT, m_height); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0); 

	return true;
}

void Perlin::onUpdate(float a_deltaTime) 
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

	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void Perlin::onDraw() 
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	// draw the gizmos from this frame
	//Gizmos::draw(m_projectionMatrix, viewMatrix);

	glUseProgram(m_shader); 

	int location = glGetUniformLocation(m_shader, "projectionViewModel"); 
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m_projectionMatrix * viewMatrix));

	location = glGetUniformLocation(m_shader, "diffuseMap"); 
	glUniform1i(location, 0); 

	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, m_texture); 

	glBindVertexArray(m_vao); 
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	glDrawElements(GL_PATCHES, 6, GL_UNSIGNED_INT, nullptr); 

	// get window dimensions for 2D orthographic projection
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	Gizmos::draw2D(glm::ortho<float>(0, width, 0, height, -1.0f, 1.0f));
}

void Perlin::onDestroy()
{
	glDeleteProgram(m_shader);
	glDeleteVertexArrays(1, &m_vao); 
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
	// clean up anything we created
	Gizmos::destroy();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new Perlin();
	
	if (app->create("AIE - Perlin",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}