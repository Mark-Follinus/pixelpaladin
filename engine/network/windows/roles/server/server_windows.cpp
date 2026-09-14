#include "server_windows.hpp"

// Standard library includes
#include <cstdio>
#include <utility>

// Windows library includes
#include <WS2tcpip.h>

// Engine includes
#include "network/data/packet.hpp"
#include "network/data/network_codec.hpp"

// Compile time variables
constexpr u_long NON_BLOCKING_SOCKET_COMMAND = 1ul;

namespace pldn
{

// ------------------------
// Public Methods
// ------------------------

	bool NetworkServer::Start() 
    {
        // Early guard incase things change.
        if (m_listeningSocket != INVALID_SOCKET)
        {
            std::printf("Server: Already running.\n");
            return true;
        }

        // Configuring server socket.
        if ((m_listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
        {
            const int error = WSAGetLastError();
            std::printf("Server: Socket configuration failed: %i\n", error);

            return false;
        }

        // Making it non-blocking.
        u_long nonBlocking = NON_BLOCKING_SOCKET_COMMAND;
        if (ioctlsocket(m_listeningSocket, FIONBIO, &nonBlocking) == SOCKET_ERROR)
        {
            const int error = WSAGetLastError();
            std::printf("Server: Failed to make socket non-blocking: %i\n", error);

            closesocket(m_listeningSocket);
            m_listeningSocket = INVALID_SOCKET;

            return false;
        }

        // Binding socket to IP address and port.
        sockaddr_in serverAddress = {};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(m_networkConfig.ServerPort);

        if (InetPtonA(AF_INET, m_networkConfig.IPAddress.c_str(), &serverAddress.sin_addr) != 1)
        {
            const int error = WSAGetLastError();
            std::printf("Server: InetPton failed: %i\n", error);

            closesocket(m_listeningSocket);
            m_listeningSocket = INVALID_SOCKET;

            return false;
        }
        else if (bind(m_listeningSocket, reinterpret_cast<const sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
        {
            const int error = WSAGetLastError();
            std::printf("Server: Socket binding failed: %i\n", error);

            closesocket(m_listeningSocket);
            m_listeningSocket = INVALID_SOCKET;

            return false;
        }

        // Start listening.
        if (listen(m_listeningSocket, static_cast<int>(MAX_CLIENT_CONNECTIONS)) == SOCKET_ERROR)
        {
            const int error = WSAGetLastError();
            std::printf("Server: Failed to switch to listening mode: %i\n", error);

            closesocket(m_listeningSocket);
            m_listeningSocket = INVALID_SOCKET;

            return false;
        }

        std::printf("Server: Successfully started! Listening for connections...\n");
        return true;
    }

	void NetworkServer::Stop() 
    {
        if (m_listeningSocket != INVALID_SOCKET)
        {
            closesocket(m_listeningSocket);
            m_listeningSocket = INVALID_SOCKET;
        }

        for (std::optional<ClientConnection>& client : m_connections)
        {
            client.reset(); // Destroys its Connection and closes its socket.
        }

        m_incomingInputCommands = {};
        m_outgoingGameEvents = {};
    }

	void NetworkServer::Receive() 
    {
        for (std::optional<ClientConnection>& client : m_connections)
        {
            if (!client.has_value())
            {
                continue;
            }

            client->ConnectedPeer.ReceivePackets();

            bool removeClient = false;

            Packet packet{};
            while (client->ConnectedPeer.PollPacket(packet))
            {
                switch (packet.Type)
                {
                    case PacketType::InputCommand:
                    {
                        InputCommand command{};

                        if (!DeserializeInputCommand(packet, command))
                        {
                            std::printf("Server: Invalid InputCommand packet.\n");
                            client->ConnectedPeer.Disconnect();
                            removeClient = true;
                            break;
                        }

                        m_incomingInputCommands.push({client->ID, command });
                        break;
                    }

                    default:
                    {
                        std::printf("Server: Unexpected packet type.\n");
                        break;
                    }
                }

                if (removeClient)
                {
                    break;
                }
            }

            // Poll final packets before removing a closed connection.
            if (!client->ConnectedPeer.IsConnected())
            {
                client.reset();
            }
        }

        AcceptClients();
    }

	void NetworkServer::Send()
    {
        while (!m_outgoingGameEvents.empty())
        {
            GameEvent& event = m_outgoingGameEvents.front();

            for (std::optional<ClientConnection>& client : m_connections)
            {
                if (!client.has_value())
                {
                    continue;
                }

                client->ConnectedPeer.QueuePacket(SerializeGameEvent(event));
            }

            m_outgoingGameEvents.pop();
        }

        for (std::optional<ClientConnection>& client : m_connections)
        {
            if (client.has_value())
            {
                client->ConnectedPeer.FlushPackets();
            }
        }
    }

	bool NetworkServer::PollInputCommand(ClientInputCommand& command) 
    {
        if (m_incomingInputCommands.empty())
        {
            return false;
        }

        command = std::move(m_incomingInputCommands.front());

        m_incomingInputCommands.pop();
        return true;
    }

	void NetworkServer::QueueGameEvent(GameEvent event)
    {
        m_outgoingGameEvents.push(std::move(event));
    }

// ------------------------
// Private Constructors
// ------------------------ 

    NetworkServer::NetworkServer(const std::string& ipAddress, uint16_t port)
        : m_networkConfig({ port, ipAddress })
    {
    }

    NetworkServer::~NetworkServer()
    {
        Stop();
    }

// ------------------------
// Private Methods
// ------------------------

	void NetworkServer::AcceptClients() 
    {
        if (m_listeningSocket == INVALID_SOCKET)
        {
            return;
        }

        int8_t clientIndex = 0;
        std::optional<ClientConnection>* potentialEmptyConnection = nullptr;

        for (std::size_t i = 0; i < m_connections.size(); i++)
        {
            if (!m_connections[i].has_value())
            {
                potentialEmptyConnection = &m_connections[i];
                clientIndex = static_cast<int8_t>(i);
                break;
            }
        }

        if (potentialEmptyConnection == nullptr)
        {
            return;
        }

        SOCKET serverToClientSocket = accept(m_listeningSocket, nullptr, nullptr);
        if (serverToClientSocket == INVALID_SOCKET)
        {
            const int error = WSAGetLastError();
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
            const int error = WSAGetLastError();
            std::printf("Server: Failed to make new runtime socket non-blocking: %i\n", error);

            closesocket(serverToClientSocket);
            serverToClientSocket = INVALID_SOCKET;
            return;
        }

        potentialEmptyConnection->emplace(clientIndex, serverToClientSocket);
        std::printf("Server: New client connected!\n");
    }
}