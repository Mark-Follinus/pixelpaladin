#include "networking_windows.hpp"

// Standard library includes
#include <iostream>

// Windows library includes
#include <WinSock2.h>
#include <WS2tcpip.h>

// ------------------------
// Public Methods
// ------------------------

// ------------------------
// Private Constructors
// ------------------------ 
Networking::Networking(const Networking::NetworkRole role, const NetworkConfig& config) : m_role(role), m_config(config)
{
    switch (m_role)
    {
        case NetworkRole::None: { break; }
        case NetworkRole::Client:
        {
            InitializeAsClient();
            break;
        }
        case NetworkRole::Server:
        {
            InitializeAsServer();
            break;
        }
        case NetworkRole::Host:
        {
            InitializeAsHost();
            break;
        }
    }
}

Networking::~Networking()
{

}

// ------------------------
// Private Methods
// ------------------------

void Networking::InitializeAsNone()
{
}

void Networking::InitializeAsClient()
{
}

void Networking::InitializeAsServer()
{
    SOCKET serverSocket, acceptSocket;
    short port = 55555;
    WSADATA wsaData;

    WORD wVersionRequested = MAKEWORD(2, 2);
    if (int wsaError = WSAStartup(wVersionRequested, &wsaData) != 0)
    {
        std::printf("nahhh\n");
    }


}

void Networking::InitializeAsHost() { }

void Networking::Update() { }

void Networking::UpdateClient() { }

void Networking::UpdateServer() { }