#include "networking_windows.hpp"

// Standard library includes
#include <iostream>
#include <array>

// Windows library includes
#include <WS2tcpip.h>

// Engine includes
#include "../data/packet.hpp"

// Compile time variables
constexpr u_long NON_BLOCKING_SOCKET_COMMAND = 1ul;

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
    switch (m_role)
    {
        case NetworkRole::None: { break; }
        case NetworkRole::Client:
        {
            if (m_clientToServerConnection.Socket != INVALID_SOCKET)
            {
                closesocket(m_clientToServerConnection.Socket);
                m_clientToServerConnection.Socket = INVALID_SOCKET;
            }

            m_clientToServerConnection.State = NetworkConnectionState::Disconnected;
            break;
        }
        case NetworkRole::Server:
        {
            if (m_serverListenSocket != INVALID_SOCKET)
            {
                closesocket(m_serverListenSocket);
                m_serverListenSocket = INVALID_SOCKET;
            }

            for (NetworkConnection& connection : m_serverToClientConnections)
            {
                if (connection.Socket == INVALID_SOCKET)
                {
                    continue;
                }

                closesocket(connection.Socket);
                connection.Socket = INVALID_SOCKET;
                connection.State = NetworkConnectionState::Disconnected;
            }

            break;
        }
        case NetworkRole::Host: { break; }
    }

    if (m_wsaInitialized)
    {
        WSACleanup();
        m_wsaInitialized = false;
    }
}

// ------------------------
// Private Methods
// ------------------------

void Networking::InitializeAsNone() {}

void Networking::InitializeAsClient()
{
    // Step 1 - Initializing WSA.
    WORD wVersionRequested = MAKEWORD(m_config.WinSockVersionMajor, m_config.WinSockVersionMinor);
    const i32 wsaError = WSAStartup(wVersionRequested, &m_wsaData);
    if (wsaError != 0)
    {
        std::printf("Client: WSA startup failed: %i\n", wsaError);
        return;
    }
    else 
    {
        std::printf("Client: Initialized WSA.\n");
    }

    // Step 2 - Configuring client socket.
    if ((m_clientToServerConnection.Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
    {
        const i32 error = WSAGetLastError();
        std::printf("Client: Socket configuration failed: %i\n", error);

        WSACleanup();
        return;
    }

    // Step 2b - Making the socket non-blocking.
    u_long nonBlocking = NON_BLOCKING_SOCKET_COMMAND;
    if (ioctlsocket(m_clientToServerConnection.Socket, FIONBIO, &nonBlocking) == SOCKET_ERROR)
    {
        const i32 error = WSAGetLastError();
        std::printf("Client: Failed to make socket non-blocking: %i\n", error);

        closesocket(m_clientToServerConnection.Socket);
        m_clientToServerConnection.Socket = INVALID_SOCKET;

        WSACleanup();
        return;
    }
    else
    {
        std::printf("Client: Configured Socket.\n");
    }

    // Step 3 - Connecting to server.
    sockaddr_in serverAddress = {};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(m_config.ServerPort);

    if (InetPtonA(AF_INET, m_config.IPAddress, &serverAddress.sin_addr) != 1)
    {
        const i32 error = WSAGetLastError();
        std::printf("Client: InetPton failed: %i\n", error);

        closesocket(m_clientToServerConnection.Socket);
        m_clientToServerConnection.Socket = INVALID_SOCKET;

        WSACleanup();
        return;
    }
    else if (connect(m_clientToServerConnection.Socket, reinterpret_cast<const sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        const i32 error = WSAGetLastError();
        if (error == WSAEWOULDBLOCK) // If in the process of connecting.
        {
            // Do NOT close the socket. UpdateClient() will check whether it finishes.
            std::printf("Client: Connection in progress.\n");

            m_clientToServerConnection.State = NetworkConnectionState::Connecting;
        }
        else
        {
            std::printf("Client: Connecting to server failed: %i\n", error);

            closesocket(m_clientToServerConnection.Socket);
            m_clientToServerConnection.Socket = INVALID_SOCKET;

            WSACleanup();
            return;
        }
    }
    else
    {
        m_clientToServerConnection.State = NetworkConnectionState::Connected;
        std::printf("Client: Connected to Server.\n");
        m_wsaInitialized = true;
    }

    //PacketHeader header { PacketHeader::PacketType::Test, sizeof(PacketBody) };
    //PacketBody body { 10, 20, 1.5f, 3.0f };
    //constexpr size_t packetSize = sizeof(PacketHeader) + sizeof(PacketBody);
    //std::array<char, packetSize> packet {};
    //std::memcpy(packet.data(), &header, sizeof(PacketHeader));
    //std::memcpy(packet.data() + sizeof(PacketHeader), &body, sizeof(PacketBody));
    //i32 bytesSent = send(m_serverSocket, packet.data(), static_cast<i32>(packet.size()), 0);
    //if (bytesSent == SOCKET_ERROR)
    //{
    //    std::printf("Failed to send packet: %d\n", WSAGetLastError());
    //}

    //closesocket(m_serverSocket);
    //m_serverSocket = INVALID_SOCKET;
    //WSACleanup();
}

void Networking::InitializeAsServer()
{
    // Step 1 - Initializing WSA.
    WORD wVersionRequested = MAKEWORD(m_config.WinSockVersionMajor, m_config.WinSockVersionMinor);
    const i32 wsaError = WSAStartup(wVersionRequested, &m_wsaData);
    if (wsaError != 0)
    {
        std::printf("Server: WSA startup failed: %i\n", wsaError);
        return;
    }
    else 
    {
        std::printf("Server: WSA initialized.\n");
    }

    // Step 2 - Configuring server socket.
    if ((m_serverListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
    {
        const i32 error = WSAGetLastError();
        std::printf("Server: Socket configuration failed: %i\n", error);

        WSACleanup();
        return;
    }

    // Step 2b - Making the socket non-blocking.
    u_long nonBlocking = NON_BLOCKING_SOCKET_COMMAND;
    if (ioctlsocket(m_serverListenSocket, FIONBIO, &nonBlocking) == SOCKET_ERROR)
    {
        const i32 error = WSAGetLastError();
        std::printf("Server: Failed to make socket non-blocking: %i\n", error);

        closesocket(m_serverListenSocket);
        m_serverListenSocket = INVALID_SOCKET;

        WSACleanup();
        return;
    }
    else
    {
        std::printf("Server: Configured socket.\n");
    }

    // Step 3 - Binding socket to IP address.
    sockaddr_in serverAddress = {};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(m_config.ServerPort);

    if (InetPtonA(AF_INET, m_config.IPAddress, &serverAddress.sin_addr) != 1)
    {
        const i32 error = WSAGetLastError();
        std::printf("Server: InetPton failed: %i\n", error);

        closesocket(m_serverListenSocket);
        m_serverListenSocket = INVALID_SOCKET;

        WSACleanup();
        return;
    }
    else if (bind(m_serverListenSocket, reinterpret_cast<const sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        const i32 error = WSAGetLastError();
        std::printf("Server: Socket binding failed: %i\n", error);

        closesocket(m_serverListenSocket);
        m_serverListenSocket = INVALID_SOCKET;

        WSACleanup();
        return;
    }
    else 
    {
        std::printf("Server: Socket bound.\n");
    }

    // Step 4 - Listen for clients.
    if (listen(m_serverListenSocket, m_config.MaximumConnections) == SOCKET_ERROR)
    {
        const i32 error = WSAGetLastError();
        std::printf("Server: Failed to switch to listening mode: %i\n", error);

        closesocket(m_serverListenSocket);
        m_serverListenSocket = INVALID_SOCKET;

        WSACleanup();
        return;
    }
    else 
    {
        std::printf("Server: Socket switched to listening mode.\n");
        std::printf("Server: Listening for connections...\n");
        m_wsaInitialized = true;
    }
    
    // Step 5 - Accept client(s).
    //std::printf(" -- Server: 5) Accepting Clients -- \n");
    //SOCKET acceptSocket = accept(m_listenSocket, nullptr, nullptr);
    //if (acceptSocket == INVALID_SOCKET)
    //{
    //    std::printf("Accepting client failed: %d\n", WSAGetLastError());
    //    closesocket(m_listenSocket);
    //    m_listenSocket = INVALID_SOCKET;
    //    WSACleanup();
    //    return;
    //}
    //m_clientSockets.push_back(acceptSocket);

    //constexpr size_t packetSize = sizeof(PacketHeader) + sizeof(PacketBody);
    //std::array<char, packetSize> buffer{};
    //const i32 bytesReceived = recv(acceptSocket, buffer.data(), static_cast<i32>(buffer.size()), 0);
    //if (bytesReceived == SOCKET_ERROR)
    //{
    //    std::printf("recv failed: %d\n", WSAGetLastError());
    //    return;
    //}

   /* PacketHeader header{};
    std::memcpy(&header, buffer.data(), sizeof(PacketHeader));
    switch(header.type)
    {
        case PacketHeader::PacketType::Test:
        {
            PacketBody body{};
            std::memcpy(&body, buffer.data() + sizeof(PacketHeader), sizeof(PacketBody));
            std::printf( "%u %u %.2f %.2f\n", body.IntegerA, body.IntegerB, body.FloatA, body.FloatB);
            break;
        }

        case PacketHeader::PacketType::Disconnect: break;
        default: break;
    }

    closesocket(acceptSocket);
    acceptSocket = INVALID_SOCKET;
    WSACleanup();*/
}

void Networking::InitializeAsHost() { }

void Networking::Update() 
{ 
    switch (m_role)
    {
        case NetworkRole::None: { break; }
        case NetworkRole::Client:
        {
            UpdateClient();
            break;
        }
        case NetworkRole::Server:
        {
            UpdateServer();
            break;
        }
        case NetworkRole::Host:
        {
            UpdateHost();
            break;
        }
    }
}

void Networking::UpdateClient() 
{
    switch (m_clientToServerConnection.State)
    {
        case NetworkConnectionState::Connecting:
        {
            AcceptServerConnection();
            break;
        }

        case NetworkConnectionState::Connected:
        {
            // SendLocalInput()
            // ReceiveGameData()
            break;
        }

        case NetworkConnectionState::Disconnected: { break; }
    }
}

void Networking::UpdateServer() 
{
    AcceptClientConnections();

    for (NetworkConnection& client : m_serverToClientConnections)
    {
        if (client.Socket == INVALID_SOCKET)
        {
            continue;
        }

        //ParseInput
        //ReceiveData(client);
        //ParsePackets(client);
    }
}

void Networking::UpdateHost() 
{ 
    // ConvertLocalInput()
    // SendLocalInput()

    //AcceptNewClients();
    //for (ClientConnection& client : m_clients)
    //{
    //    ReceiveData(client);
    //    ParsePackets(client);
    //}

    // ReceiveGameData()
}

void Networking::AcceptServerConnection()
{
    NetworkConnection& connection = m_clientToServerConnection;

    fd_set writeSet{};
    fd_set exceptionSet{};

    FD_ZERO(&writeSet);
    FD_ZERO(&exceptionSet);

    FD_SET(connection.Socket, &writeSet);
    FD_SET(connection.Socket, &exceptionSet);

    timeval timeout{};
        timeout.tv_sec = 0;  // Time out in seconds.
        timeout.tv_usec = 0; // Time out in microseconds.

    const i32 result = select(0, nullptr, &writeSet, &exceptionSet, &timeout);
    if (result == SOCKET_ERROR)
    {
        const i32 error = WSAGetLastError();
        std::printf("Client: select failed: %i\n", error);
        return;
    }

    if (result == 0)
    {
        return; // Stil connecting.
    }

    if (FD_ISSET(connection.Socket, &writeSet))
    {
        std::printf("Client: Connected to server!\n");
        connection.State = NetworkConnectionState::Connected;
        return;
    }

    if (FD_ISSET(connection.Socket, &exceptionSet))
    {
        i32 socketError = 0;
        i32 errorSize = sizeof(socketError);
        getsockopt(connection.Socket, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&socketError), &errorSize);

        std::printf("Client: Connection failed: %i\n", socketError);

        closesocket(connection.Socket);
        connection.Socket = INVALID_SOCKET;
        connection.State = NetworkConnectionState::Disconnected;
    }
}

void Networking::AcceptClientConnections()
{
    NetworkConnection* potentialEmptyConnection = nullptr;

    for (NetworkConnection& client : m_serverToClientConnections)
    {
        if (client.Socket == INVALID_SOCKET)
        {
            potentialEmptyConnection = &client;
            break;
        }
    }

    if (potentialEmptyConnection == nullptr)
    {
        return;
    }

    SOCKET serverToClientSocket = accept(m_serverListenSocket, nullptr, nullptr);
    if (serverToClientSocket == INVALID_SOCKET)
    {
        const i32 error = WSAGetLastError();
        if (error == WSAEWOULDBLOCK)
        {
            return;
        }

        std::printf("Server: Accept failed: %i\n", error);
        return;
    }

    u_long nonBlocking = NON_BLOCKING_SOCKET_COMMAND;
    if (ioctlsocket(serverToClientSocket, FIONBIO, &nonBlocking) == SOCKET_ERROR)
    {
        const i32 error = WSAGetLastError();
        std::printf("Server: Failed to make new runtime socket non-blocking: %i\n", error);

        closesocket(serverToClientSocket);
        serverToClientSocket = INVALID_SOCKET;
        return;
    }

    std::printf("Server: New client connected!\n");
    potentialEmptyConnection->Socket = serverToClientSocket;
    potentialEmptyConnection->State = NetworkConnectionState::Connected;
}