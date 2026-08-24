#include "networking_windows.hpp"

// Standard library includes
#include <iostream>

// Windows library includes
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
    // Step 1 - Initializing WSA.
    std::printf(" -- Client: 1) Initializing WSA -- \n");
    WSADATA wsaData = {};
    WORD wVersionRequested = MAKEWORD(m_config.WinSockVersionMajor, m_config.WinSockVersionMinor);
    const i32 wsaError = WSAStartup(wVersionRequested, &wsaData);
    if (wsaError != 0)
    {
        std::printf("WSA startup failed: %i\n", wsaError);
        return;
    }

    // Step 2 - Configuring client socket.
    std::printf(" -- Client: 2) Configuring Socket -- \n");
    m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_serverSocket == INVALID_SOCKET)
    {
        std::printf("Socket configuration failed: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    // Step 3 - Connecting to server.
    std::printf(" -- Client: 3) Connecting to Server -- \n");
    sockaddr_in serverAddress = {};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(m_config.ServerPort);
    if (InetPtonA(AF_INET, m_config.IPAddress, &serverAddress.sin_addr) != 1)
    {
        std::printf("InetPton failed: %d\n", WSAGetLastError());
        closesocket(m_serverSocket);
        m_serverSocket = INVALID_SOCKET;
        WSACleanup();
        return;
    }
    else if (connect(m_serverSocket, reinterpret_cast<const sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        std::printf("Connecting to server failed: %d\n", WSAGetLastError());
        closesocket(m_serverSocket);
        m_serverSocket = INVALID_SOCKET;
        WSACleanup();
        return;
    }
    std::printf(" -- AYO WE MADE OUT THE HOOD -- \n");
}

void Networking::InitializeAsServer()
{
    // Step 1 - Initializing WSA.
    std::printf(" -- Server: 1) Initializing WSA -- \n");
    WSADATA wsaData = {};
    WORD wVersionRequested = MAKEWORD(m_config.WinSockVersionMajor, m_config.WinSockVersionMinor);
    const i32 wsaError = WSAStartup(wVersionRequested, &wsaData);
    if (wsaError != 0)
    {
        std::printf("WSA startup failed: %i\n", wsaError);
        return;
    }

    // Step 2 - Configuring server socket.
    std::printf(" -- Server: 2) Configuring Socket -- \n");
    m_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_listenSocket == INVALID_SOCKET)
    {
        std::printf("Socket configuration failed: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    // Step 3 - Binding socket to IP address.
    std::printf(" -- Server: 3) Binding Socket -- \n");
    sockaddr_in serverAddress = {};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(m_config.ServerPort);
    if (InetPtonA(AF_INET, m_config.IPAddress, &serverAddress.sin_addr) != 1)
    {
        std::printf("InetPton failed: %d\n", WSAGetLastError());
        closesocket(m_listenSocket);
        m_listenSocket = INVALID_SOCKET;
        WSACleanup();
        return;
    }
    else if (bind(m_listenSocket, reinterpret_cast<const sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        std::printf("Socket binding failed: %d\n", WSAGetLastError());
        closesocket(m_listenSocket);
        m_listenSocket = INVALID_SOCKET;
        WSACleanup();
        return;
    }

    // Step 4 - Listen for clients.
    std::printf(" -- Server: 4) Listening for Clients -- \n");
    if (listen(m_listenSocket, m_config.MaximumConnections) == SOCKET_ERROR)
    {
        std::printf("Listening for client failed: %d\n", WSAGetLastError());
        closesocket(m_listenSocket);
        m_listenSocket = INVALID_SOCKET;
        WSACleanup();
        return;
    }
    
    // Step 5 - Accept client(s).
    std::printf(" -- Server: 5) Accepting Clients -- \n");
    SOCKET acceptSocket = accept(m_listenSocket, nullptr, nullptr);
    if (acceptSocket == INVALID_SOCKET)
    {
        std::printf("Accepting client failed: %d\n", WSAGetLastError());
        closesocket(m_listenSocket);
        m_listenSocket = INVALID_SOCKET;
        WSACleanup();
        return;
    }

    m_clientSockets.push_back(acceptSocket);
    std::printf(" -- WAZZUP -- \n");
}

void Networking::InitializeAsHost() { }

void Networking::Update() { }

void Networking::UpdateClient() { }

void Networking::UpdateServer() { }