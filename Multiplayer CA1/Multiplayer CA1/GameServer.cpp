#include "GameServer.hpp"

#include <iostream>

#include "NetworkProtocol.hpp"
#include <SFML/System.hpp>

#include <SFML/Network/Packet.hpp>

#include "Aircraft.hpp"
#include "PickupType.hpp"
#include "Utility.hpp"

//It is essential to set the sockets to non-blocking - m_socket.setBlocking(false)
//otherwise the server will hang waiting to read input from a connection

GameServer::RemotePeer::RemotePeer(): m_ready(false), m_timed_out(false)
{
	m_socket.setBlocking(false);
}

GameServer::GameServer(sf::Vector2f battlefield_size, World& world)
	: m_thread(&GameServer::ExecutionThread, this)
	, m_listening_state(false)
	, m_client_timeout(sf::seconds(1.f))
	, m_max_connected_players(15)
	, m_connected_players(0)
	, m_battlefield_rect(0.f, 0.f, 900, 600)
	, m_tank_count(0)
	, m_peers(1)
	, m_tank_identifier_counter(1)
	, m_waiting_thread_end(false)
	, m_last_spawn_time(sf::Time::Zero)
	, m_time_for_next_spawn(sf::seconds(5.f))
	, m_world(&world)

{
	m_listener_socket.setBlocking(false);
	m_peers[0].reset(new RemotePeer());
	m_thread.launch();
}

GameServer::~GameServer()
{
	m_waiting_thread_end = true;
	m_thread.wait();
}

//This is the same as SpawnSelf but indicate that an aircraft from a different client is entering the world

void GameServer::NotifyPlayerSpawn(sf::Int32 aircraft_identifier)
{
	sf::Packet packet;
	//First thing for every packet is what type of packet it is
	packet << static_cast<sf::Int32>(Server::PacketType::PlayerConnect);
	packet << aircraft_identifier << m_tank_info[aircraft_identifier].m_position.x << m_tank_info[aircraft_identifier].
		m_position.y;
	for (std::size_t i = 0; i < m_connected_players; ++i)
	{
		if (m_peers[i]->m_ready)
		{
			m_peers[i]->m_socket.send(packet);
		}
	}
}

//This is the same as PlayerEvent, but for real-time actions. This means that we are changing an ongoing state to either true or false, so we add a Boolean value to the parameters

void GameServer::NotifyPlayerRealtimeChange(sf::Int32 aircraft_identifier, sf::Int32 action, bool action_enabled)
{
	sf::Packet packet;
	//First thing for every packet is what type of packet it is
	packet << static_cast<sf::Int32>(Server::PacketType::PlayerRealtimeChange);
	packet << aircraft_identifier;
	packet << action;
	packet << action_enabled;

	for (std::size_t i = 0; i < m_connected_players; ++i)
	{
		if (m_peers[i]->m_ready)
		{
			m_peers[i]->m_socket.send(packet);
		}
	}
}

//This takes two sf::Int32 variables, the aircraft identifier and the action identifier
//as declared in the Player class. This is used to inform all peers that plane X has
//triggered an action

void GameServer::NotifyPlayerEvent(sf::Int32 aircraft_identifier, sf::Int32 action)
{
	sf::Packet packet;
	//First thing for every packet is what type of packet it is
	packet << static_cast<sf::Int32>(Server::PacketType::PlayerEvent);
	packet << aircraft_identifier;
	packet << action;

	for (std::size_t i = 0; i < m_connected_players; ++i)
	{
		if (m_peers[i]->m_ready)
		{
			m_peers[i]->m_socket.send(packet);
		}
	}
}

void GameServer::SetListening(bool enable)
{
	//Check if the server listening socket is already listening
	if (enable)
	{
		if (!m_listening_state)
		{
			m_listening_state = (m_listener_socket.listen(SERVER_PORT) == sf::TcpListener::Done);
		}
	}
	else
	{
		m_listener_socket.close();
		m_listening_state = false;
	}
}

void GameServer::ExecutionThread()
{
	SetListening(true);

	sf::Time frame_rate = sf::seconds(1.f / 60.f);
	sf::Time frame_time = sf::Time::Zero;
	sf::Time tick_rate = sf::seconds(1.f / 20.f);
	sf::Time tick_time = sf::Time::Zero;
	sf::Clock frame_clock, tick_clock;

	while (!m_waiting_thread_end)
	{
		HandleIncomingConnections();
		HandleIncomingPackets();


		frame_time += frame_clock.getElapsedTime();
		frame_clock.restart();

		tick_time += tick_clock.getElapsedTime();
		tick_clock.restart();

		//Fixed update step
		while (frame_time >= frame_rate)
		{
			frame_time -= frame_rate;
		}

		//Fixed tick step
		while (tick_time >= tick_rate)
		{
			Tick();
			tick_time -= tick_rate;
		}

		//sleep
		sf::sleep(sf::milliseconds(100));
		
	}
}

void GameServer::Tick()
{
	UpdateClientState();

	//Check if the game is over = all planes position.y < offset
	bool all_aircraft_done = false;

	//TODO Win condition

	if (all_aircraft_done)
	{
		sf::Packet mission_success_packet;
		mission_success_packet << static_cast<sf::Int32>(Server::PacketType::MissionSuccess);
		SendToAll(mission_success_packet);
	}
	if (m_connected_players > 12 || m_clock.getElapsedTime().asSeconds() > 20 && !has_sent_timer)
	{
		has_sent_timer = true;
		sf::Packet start_counter_packet;
		start_counter_packet << static_cast<sf::Int32>(Server::PacketType::StartTimer);
		SendToAll(start_counter_packet);
		SetListening(false);
		
		
	}

	//TODO do we really need this ?
	//Remove tanks that have been destroyed

}

sf::Time GameServer::Now() const
{
	return m_clock.getElapsedTime();
}

void GameServer::HandleIncomingPackets()
{
	bool detected_timeout = false;

	for (PeerPtr& peer : m_peers)
	{
		if (peer->m_ready)
		{
			sf::Packet packet;
			while (peer->m_socket.receive(packet) == sf::Socket::Done)
			{
				//Interpret the packet and react to it
				HandleIncomingPacket(packet, *peer, detected_timeout);

				peer->m_last_packet_time = Now();
				packet.clear();
			}

			if (Now() > peer->m_last_packet_time + m_client_timeout)
			{
				peer->m_timed_out = true;
				detected_timeout = true;
			}
		}
	}

	if (detected_timeout)
	{
		HandleDisconnections();
	}
}

void GameServer::HandleIncomingPacket(sf::Packet& packet, RemotePeer& receiving_peer, bool& detected_timeout)
{
	sf::Int32 packet_type;
	packet >> packet_type;

	switch (static_cast<Client::PacketType>(packet_type))
	{
	case Client::PacketType::Quit:
		{
			receiving_peer.m_timed_out = true;
			detected_timeout = true;
		}
		break;

	case Client::PacketType::PlayerEvent:
		{
			sf::Int32 aircraft_identifier;
			sf::Int32 action;
			packet >> aircraft_identifier >> action;
			NotifyPlayerEvent(aircraft_identifier, action);
		}
		break;

	case Client::PacketType::PlayerRealtimeChange:
		{
			sf::Int32 aircraft_identifier;
			sf::Int32 action;
			bool action_enabled;
			packet >> aircraft_identifier >> action >> action_enabled;
			NotifyPlayerRealtimeChange(aircraft_identifier, action, action_enabled);
		}
		break;

	case Client::PacketType::PositionUpdate:
		{
			sf::Int32 num_aircraft = 1;


			for (sf::Int32 i = 0; i < num_aircraft; ++i)
			{
				sf::Int32 aircraft_identifier;
				sf::Int32 aircraft_hitpoints;
				sf::Int32 missile_ammo;
				sf::Vector2f aircraft_position;
				packet >> aircraft_identifier >> aircraft_position.x >> aircraft_position.y >> aircraft_hitpoints >>
					missile_ammo;
				m_tank_info[aircraft_identifier].m_position = aircraft_position;
				m_tank_info[aircraft_identifier].m_hitpoints = aircraft_hitpoints;
				m_tank_info[aircraft_identifier].m_ammo = missile_ammo;
			/*	std::cout << aircraft_identifier << "," << m_tank_info[aircraft_identifier].m_position.x << "," <<
					m_tank_info[aircraft_identifier].m_position.y << "," << m_tank_info[aircraft_identifier].m_hitpoints
					<< "," << m_tank_info[aircraft_identifier].m_ammo << std::endl;*/
			}
		}
		break;

	case Client::PacketType::GameEvent:
		{
			sf::Int32 action;
			float x;
			float y;
			sf::Int32 pickup_type_int;

			packet >> action;
			packet >> x;
			packet >> y;
			packet >> pickup_type_int;

			//No need to check if host sent it -> only host can send Pickup Spawn notifications
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Server::PacketType::SpawnPickup);
			packet << pickup_type_int;
			packet << x;
			packet << y;

			SendToAll(packet);
		}
	}
}

void GameServer::HandleIncomingConnections()
{
	if (!m_listening_state)
	{
		return;
	}

	if (m_listener_socket.accept(m_peers[m_connected_players]->m_socket) == sf::TcpListener::Done)
	{
		//Order the new client to spawn its player 1
		m_tank_info[m_tank_identifier_counter].m_position = m_world->GetTankSpawn(m_tank_identifier_counter);
		m_tank_info[m_tank_identifier_counter].m_hitpoints = 100;
		m_tank_info[m_tank_identifier_counter].m_ammo = 2;

		sf::Packet packet;
		packet << static_cast<sf::Int32>(Server::PacketType::SpawnSelf);
		packet << m_tank_identifier_counter;
		packet << m_tank_info[m_tank_identifier_counter].m_position.x;
		packet << m_tank_info[m_tank_identifier_counter].m_position.y;

		m_peers[m_connected_players]->m_tank_identifiers.emplace_back(m_tank_identifier_counter);

		BroadcastMessage("New player");
		InformWorldState(m_peers[m_connected_players]->m_socket);
		NotifyPlayerSpawn(m_tank_identifier_counter++);

		m_peers[m_connected_players]->m_socket.send(packet);
		m_peers[m_connected_players]->m_ready = true;
		m_peers[m_connected_players]->m_last_packet_time = Now();

		m_tank_count++;
		m_connected_players++;

		if (m_connected_players >= m_max_connected_players)
		{
			SetListening(false);
		}
		else
		{
			m_peers.emplace_back(PeerPtr(new RemotePeer()));
		}
	}
}

void GameServer::HandleDisconnections()
{
	for (auto itr = m_peers.begin(); itr != m_peers.end();)
	{
		if ((*itr)->m_timed_out)
		{
			//Inform everyone of a disconnection, erase
			for (sf::Int32 identifer : (*itr)->m_tank_identifiers)
			{
				SendToAll((sf::Packet() << static_cast<sf::Int32>(Server::PacketType::PlayerDisconnect) << identifer));
				m_tank_info.erase(identifer);
			}

			m_connected_players--;
			m_tank_count -= (*itr)->m_tank_identifiers.size();

			itr = m_peers.erase(itr);

			//If the number of peers has dropped below max_connections
			if (m_connected_players < m_max_connected_players)
			{
				m_peers.emplace_back(PeerPtr(new RemotePeer()));
				SetListening(true);
			}

			BroadcastMessage("A player has disconnected");
		}
		else
		{
			++itr;
		}
	}
}

void GameServer::InformWorldState(sf::TcpSocket& socket)
{
	sf::Packet packet;
	packet << static_cast<sf::Int32>(Server::PacketType::InitialState);
	packet << m_battlefield_rect.width << m_battlefield_rect.height;
	packet << static_cast<sf::Int32>(m_tank_count);

	for (std::size_t i = 0; i < m_connected_players; ++i)
	{
		if (m_peers[i]->m_ready)
		{
			for (sf::Int32 identifier : m_peers[i]->m_tank_identifiers)
			{
				packet << identifier << m_tank_info[identifier].m_position.x << m_tank_info[identifier].m_position.y <<
					m_tank_info[identifier].m_hitpoints << m_tank_info[identifier].m_ammo;
			}
		}
	}

	socket.send(packet);
}

void GameServer::BroadcastMessage(const std::string& message)
{
	sf::Packet packet;
	packet << static_cast<sf::Int32>(Server::PacketType::BroadcastMessage);
	packet << message;
	for (std::size_t i = 0; i < m_connected_players; ++i)
	{
		if (m_peers[i]->m_ready)
		{
			m_peers[i]->m_socket.send(packet);
		}
	}
}

void GameServer::SendToAll(sf::Packet& packet)
{
	for (PeerPtr& peer : m_peers)
	{
		if (peer->m_ready)
		{
			peer->m_socket.send(packet);
		}
	}
}

void GameServer::UpdateClientState()
{
	sf::Packet update_client_state_packet;
	update_client_state_packet << static_cast<sf::Int32>(Server::PacketType::UpdateClientState);
	update_client_state_packet << static_cast<sf::Int32>(m_tank_count);

	for (const auto& tank : m_tank_info)
	{
		update_client_state_packet << tank.first << tank.second.m_hitpoints << tank.second.m_position.x << tank.second.
			m_position.y;
		//std::cout << "Server:" << tank.second.m_position.x << ", " << tank.second.m_position.y << std::endl;
	}

	SendToAll(update_client_state_packet);
}
