#include "Networking_Client.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include <BitStream.h>
#include <MessageIdentifiers.h>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

enum MESSAGE_ID
{
	ID_USER_NEW_CLIENT = ID_USER_PACKET_ENUM,
	ID_USER_CLIENT_DISCONNECTED,
	ID_USER_ID,

	ID_USER_CUSTOM_DATA,

	ID_USER_POSITION = ID_USER_CUSTOM_DATA
};

const int SERVER_PORT = 12001;

Networking_Client::Networking_Client()
{

}

Networking_Client::~Networking_Client()
{

}

bool Networking_Client::onCreate(int a_argc, char* a_argv[]) 
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

	m_raknet = RakNet::RakPeerInterface::GetInstance(); 

	RakNet::SocketDescriptor desc; 
	m_raknet->Startup(1, &desc, 1); 

	m_raknet->Connect("127.0.0.1", SERVER_PORT, nullptr, 0); 

	m_myID = -1; 

	return true;
}

void Networking_Client::onUpdate(float a_deltaTime) 
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

	RakNet::Packet* packet = m_raknet->Receive(); 
	while (packet != nullptr)
	{
		//process
		switch (packet->data[0])
		{
		case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				printf("Connected\n");
				m_serverAddress = packet->systemAddress; 
				break; 
			}

		case ID_USER_ID:
			{
						   RakNet::BitStream input(packet->data, packet->length, true);

			}
		};
		//get next
		m_raknet->DeallocatePacket(packet); 
		packet = m_raknet->Receive(); 
	}

	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void Networking_Client::onDraw() 
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

void Networking_Client::onDestroy()
{
	m_raknet->Shutdown(100); 
	RakNet::RakPeerInterface::DestroyInstance(m_raknet); 
	// clean up anything we created
	Gizmos::destroy();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{

	// explicitly control the creation of our application
	Application* app = new Networking_Client();
	
	if (app->create("AIE - Networking_Client",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}