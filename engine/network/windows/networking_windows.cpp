#include "networking_windows.hpp"

// Standard library includes
#include <iostream>
#include <array>

// Windows library includes
#include <WS2tcpip.h>

// Engine includes
#include "../data/packet.hpp"
#include "input/input.hpp"

// Compile time variables
constexpr u_long NON_BLOCKING_SOCKET_COMMAND = 1ul;
constexpr size_t RECEIVE_CHUNK_SIZE_BYTES = 1024;
constexpr size_t PACKET_SIZE = sizeof(PacketHeader) + sizeof(PacketBody);
constexpr size_t MAX_PACKET_PAYLOAD_SIZE = 4096;

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
    }*/
}

void Networking::InitializeAsHost() { }

void Networking::Update(Input* input)
{ 
    switch (m_role)
    {
        case NetworkRole::None: { break; }
        case NetworkRole::Client:
        {
            UpdateClient(input);
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

void Networking::UpdateClient(Input* input)
{
    switch (m_clientToServerConnection.State)
    {
        case NetworkConnectionState::Connecting:
        {
            CheckServerConnection();
            break;
        }

        case NetworkConnectionState::Connected:
        {
            if (input->GetKeyboardKeyUp(Input::KeyboardKey::Space))
            {
                ParseOutgoingData(m_clientToServerConnection);
            }
            SendData(m_clientToServerConnection);

            ReceiveData(m_clientToServerConnection);
            ParseIncomingData(m_clientToServerConnection);

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

        ReceiveData(client);
        ParseIncomingData(client);
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

void Networking::CheckServerConnection()
{
    fd_set writeSet{};
    fd_set exceptionSet{};

    FD_ZERO(&writeSet);
    FD_ZERO(&exceptionSet);

    FD_SET(m_clientToServerConnection.Socket, &writeSet);
    FD_SET(m_clientToServerConnection.Socket, &exceptionSet);

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
        return; // Still connecting.
    }

    if (FD_ISSET(m_clientToServerConnection.Socket, &writeSet))
    {
        std::printf("Client: Connected to server!\n");
        m_clientToServerConnection.State = NetworkConnectionState::Connected;
        return;
    }

    if (FD_ISSET(m_clientToServerConnection.Socket, &exceptionSet))
    {
        i32 socketError = 0;
        i32 errorSize = sizeof(socketError);
        getsockopt(m_clientToServerConnection.Socket, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&socketError), &errorSize);

        std::printf("Client: Connection failed: %i\n", socketError);

        closesocket(m_clientToServerConnection.Socket);
        m_clientToServerConnection.Socket = INVALID_SOCKET;
        m_clientToServerConnection.State = NetworkConnectionState::Disconnected;
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

void Networking::Disconnect(NetworkConnection& connection)
{
    if (connection.Socket != INVALID_SOCKET)
    {
        closesocket(connection.Socket);
    }

    connection.Socket = INVALID_SOCKET;
    connection.State = NetworkConnectionState::Disconnected;
    connection.RawReceiveBuffer.clear();
    connection.RawSendBuffer.clear();
}

void Networking::ParseOutgoingData(NetworkConnection& connection)
{
    PacketHeader header = { PacketHeader::PacketType::Test, sizeof(PacketBody) };
    PacketBody body = { static_cast<float>(rand()) / static_cast<float>(rand()),
                        static_cast<float>(rand()) / static_cast<float>(rand()),
                        static_cast<float>(rand()) / static_cast<float>(rand()),
                        static_cast<float>(rand()) / static_cast<float>(rand())};

    const size_t oldSize = connection.RawSendBuffer.size();
    const size_t packetSize = sizeof(PacketHeader) + sizeof(PacketBody);

    connection.RawSendBuffer.resize(oldSize + packetSize);

    std::memcpy(connection.RawSendBuffer.data() + oldSize, &header, sizeof(PacketHeader));
    std::memcpy(connection.RawSendBuffer.data() + oldSize + sizeof(PacketHeader), &body, sizeof(PacketBody));
}

void Networking::SendData(NetworkConnection& connection)
{
    if (connection.RawSendBuffer.empty())
    {
        return;
    }

    const i32 bytesSent = send(connection.Socket, reinterpret_cast<const char*>(connection.RawSendBuffer.data()), static_cast<i32>(connection.RawSendBuffer.size()), 0);
    if (bytesSent == SOCKET_ERROR)
    {
        const i32 error = WSAGetLastError();
        if (error == WSAEWOULDBLOCK)
        {
            return; // Socket cannot accept more data right now.
        }

        std::printf("ERROR: send() failed: %i\n", error);
        // TODO: disconnect connection.
        return;
    }

    connection.RawSendBuffer.erase(connection.RawSendBuffer.begin(), connection.RawSendBuffer.begin() + bytesSent);
}

void Networking::ReceiveData(NetworkConnection& connection)
{    
    std::array<u8, RECEIVE_CHUNK_SIZE_BYTES> receiveChunk{};

    while (true) // Getting all the available data.
    {
        const i32 bytesReceived = recv(connection.Socket, reinterpret_cast<char*>(receiveChunk.data()), static_cast<i32>(receiveChunk.size()), 0);
        if (bytesReceived == SOCKET_ERROR)
        {
            const i32 error = WSAGetLastError();
            if (error == WSAEWOULDBLOCK)
            {
                return; // Nothing available right now. Totally normal.
            }

            std::printf("ERROR: recv() failed: %i\n", error);
            //Disconnect(connection);
            return;
        }

        if (bytesReceived == 0)
        {
            //Disconnect(connection); // Peer performed an orderly disconnect.
            return; 
        }

        connection.RawReceiveBuffer.insert(connection.RawReceiveBuffer.end(), receiveChunk.begin(), receiveChunk.begin() + bytesReceived);
    }
}

void Networking::ParseIncomingData(NetworkConnection& connection)
{
    const size_t packetHeaderSize = sizeof(PacketHeader);

    while (connection.RawReceiveBuffer.size() >= packetHeaderSize)
    {
        PacketHeader header{};
        std::memcpy(&header, connection.RawReceiveBuffer.data(), packetHeaderSize);

        if (header.payloadSize > MAX_PACKET_PAYLOAD_SIZE)
        {
            std::printf("ERROR: Packet payload too large.\n"); // Probably disconnect this peer because the stream can no longer be trusted.
            //Disconnect(connection);
            return;
        }

        const size_t packetSize = packetHeaderSize + header.payloadSize;
        if (connection.RawReceiveBuffer.size() < packetSize)
        {
            return; // Only part of the packet has arrived.
        }

        switch (header.type)
        {
            case PacketHeader::PacketType::Test:
            {
                if (header.payloadSize != sizeof(PacketBody)) // Malformed packet. Packet will be discarded below.
                {
                    std::printf("ERROR: Invalid Test packet payload size.\n");
                    break; 
                }

                PacketBody body{};
                std::memcpy(&body, connection.RawReceiveBuffer.data() + packetHeaderSize, sizeof(PacketBody));

                // Transform this shit here into something else lol.
                std::printf("%f %f %f %f\n", body.FloatA, body.FloatB, body.FloatC, body.FloatD);

                break;
            }
            case PacketHeader::PacketType::Disconnect: { break; }
            default: break;
        }

        connection.RawReceiveBuffer.erase(connection.RawReceiveBuffer.begin(), connection.RawReceiveBuffer.begin() + packetSize);
    }
}