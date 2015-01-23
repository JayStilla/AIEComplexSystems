#pragma once

#include "Application.h"
#include <glm/glm.hpp>

#include <RakPeerInterface.h>
#include <map>

// derived application class that wraps up all globals neatly
class Networking_Client : public Application
{
public:

	Networking_Client();
	virtual ~Networking_Client();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	int m_myID;
	std::map<int, glm::vec3> m_players; 

	RakNet::SystemAddress* m_serverAddress; 
	RakNet::RakPeerInterface* m_raknet; 

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};