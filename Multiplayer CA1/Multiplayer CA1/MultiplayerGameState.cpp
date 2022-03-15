#include "MultiplayerGameState.hpp"
#include "MusicPlayer.hpp"
#include "Utility.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Network/IpAddress.hpp>

#include <fstream>
#include <iostream>
#include <SFML/Network/Packet.hpp>

#include "PickupType.hpp"

sf::IpAddress GetAddressFromFile()
{
	{
		//Try to open existing file ip.txt
		std::ifstream input_file("ip.txt");
		std::string ip_address;
		if (input_file >> ip_address)
		{
			return ip_address;
		}
	}

	//If open/read failed, create a new file
	std::ofstream output_file("ip.txt");
	std::string local_address = "127.0.0.1";
	output_file << local_address;
	return local_address;
}

MultiplayerGameState::MultiplayerGameState(StateStack& stack, Context context, bool is_host)
	: State(stack, context)
	  , m_world(*context.window, *context.fonts, *context.sounds, true)
	  , m_window(*context.window)
	  , m_texture_holder(*context.textures)
	  , m_connected(false)
	  , m_game_server(nullptr)
	  , m_active_state(true)
	  , m_has_focus(true)
	  , m_host(is_host)
	  , m_game_started(false)
	  , m_client_timeout(sf::seconds(2.f))
	  , m_time_since_last_packet(sf::seconds(0.f))
{
	m_broadcast_text.setFont(context.fonts->Get(Fonts::Main));
	m_broadcast_text.setPosition(1024.f / 2, 100.f);

	//We reuse this text for "Attempt to connect" and "Failed to connect" messages
	m_failed_connection_text.setFont(context.fonts->Get(Fonts::Main));
	m_failed_connection_text.setString("Attempting to connect...");
	m_failed_connection_text.setCharacterSize(35);
	m_failed_connection_text.setFillColor(sf::Color::White);
	Utility::CentreOrigin(m_failed_connection_text);
	m_failed_connection_text.setPosition(m_window.getSize().x / 2.f, m_window.getSize().y / 2.f);

	//Render an "establishing connection" frame for user feedback
	m_window.clear(sf::Color::Black);
	m_window.draw(m_failed_connection_text);
	m_window.display();
	m_failed_connection_text.setString("Could not connect to the remote server");
	Utility::CentreOrigin(m_failed_connection_text);

	sf::IpAddress ip;
	if (m_host)
	{
		m_game_server.reset(new GameServer(sf::Vector2f(m_window.getSize()), m_world));
		ip = "127.0.0.1";
	}
	else
	{
		ip = GetAddressFromFile();
	}

	if (m_socket.connect(ip, SERVER_PORT, sf::seconds(5.f)) == sf::TcpSocket::Done)
	{
		m_connected = true;
	}
	else
	{
		m_failed_connection_clock.restart();
	}

	m_socket.setBlocking(false);

	//Play game theme
	context.music->Play(MusicThemes::kGameplayTheme);
}

void MultiplayerGameState::Draw()
{
	if (m_connected)
	{
		m_world.Draw();

		//Show broadcast messages in default view
		m_window.setView(m_window.getDefaultView());

		if (!m_broadcasts.empty())
		{
			m_window.draw(m_broadcast_text);
		}
	}
	else
	{
		m_window.draw(m_failed_connection_text);
	}
}

bool MultiplayerGameState::Update(sf::Time dt)
{
	//Connected to the Server: Handle all the network logic
	if (m_connected)
	{
		m_world.Update(dt);

		//Remove players whose aircraft were destroyed
		bool found_local_plane = false;
		for (auto itr = m_players.begin(); itr != m_players.end();)
		{
			//Check if there are no more local planes for remote clients
			if (itr->first == m_local_player_identifier)
			{
				found_local_plane = true;
			}

			if (!m_world.GetTank(itr->first))
			{
				itr = m_players.erase(itr);

				//No more players left : Mission failed
				if (m_players.empty())
				{
					RequestStackPush(StateID::kGameOver);
				}
			}
			else
			{
				++itr;
			}
		}

		if (!found_local_plane && m_game_started)
		{
			//RequestStackPush(StateID::kGameOver);
		}

		//Only handle the realtime input if the window has focus and the game is unpaused
		if (m_active_state && m_has_focus)
		{
			CommandQueue& commands = m_world.getCommandQueue();
			for (auto& pair : m_players)
			{
				pair.second->HandleRealtimeInput(commands);
			}
		}

		//Always handle the network input
		CommandQueue& commands = m_world.getCommandQueue();
		for (auto& pair : m_players)
		{
			pair.second->HandleRealtimeNetworkInput(commands);
		}

		//Handle messages from the server that may have arrived
		sf::Packet packet;
		if (m_socket.receive(packet) == sf::Socket::Done)
		{
			std::cout << packet << std::endl;
			m_time_since_last_packet = sf::seconds(0.f);
			sf::Int32 packet_type;
			packet >> packet_type;
			HandlePacket(packet_type, packet);
		}
		else
		{
			//Check for timeout with the server
			if (m_time_since_last_packet > m_client_timeout)
			{
				m_connected = false;
				m_failed_connection_text.setString("Lost connection to the server");
				Utility::CentreOrigin(m_failed_connection_text);

				m_failed_connection_clock.restart();
			}
		}

		UpdateBroadcastMessage(dt);

		//Events occurring in the game
		GameActions::Action game_action;
		while (m_world.PollGameAction(game_action))
		{
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Client::PacketType::GameEvent);
			packet << static_cast<sf::Int32>(game_action.type);
			packet << game_action.position.x;
			packet << game_action.position.y;

			m_socket.send(packet);
		}

		//Regular position updates
		if (m_tick_clock.getElapsedTime() > sf::seconds(1.f / 20.f))
		{
			sf::Packet position_update_packet;
			position_update_packet << static_cast<sf::Int32>(Client::PacketType::PositionUpdate);

			if (Tank* tank = m_world.GetTank(m_local_player_identifier))
			{
				position_update_packet << m_local_player_identifier << tank->getPosition().x << tank->getPosition().y <<
					static_cast<sf::Int32>(tank->GetHitPoints()) << static_cast<sf::Int32>(tank->GetAmmo());
			}

			m_socket.send(position_update_packet);
			m_tick_clock.restart();
		}
		m_time_since_last_packet += dt;
	}

		//Failed to connect and waited for more than 5 seconds: Back to menu
	else if (m_failed_connection_clock.getElapsedTime() >= sf::seconds(5.f))
	{
		RequestStackClear();
		RequestStackPush(StateID::kMenu);
	}
	return true;
}

bool MultiplayerGameState::HandleEvent(const sf::Event& event)
{
	//Game input handling
	CommandQueue& commands = m_world.getCommandQueue();

	//Forward events to all players
	for (auto& pair : m_players)
	{
		pair.second->HandleEvent(event, commands);
	}

	if (event.type == sf::Event::KeyPressed)
	{
		//If escape is pressed, show the pause screen
		if (event.key.code == sf::Keyboard::Escape)
		{
			DisableAllRealtimeActions();
			RequestStackPush(StateID::kNetworkPause);
		}
	}
	else if (event.type == sf::Event::GainedFocus)
	{
		m_has_focus = true;
	}
	else if (event.type == sf::Event::LostFocus)
	{
		m_has_focus = false;
	}
	return true;
}

void MultiplayerGameState::OnActivate()
{
	m_active_state = true;
}

void MultiplayerGameState::OnDestroy()
{
	if (!m_host && m_connected)
	{
		//Inform server this client is dying
		sf::Packet packet;
		packet << static_cast<sf::Int32>(Client::PacketType::Quit);
		m_socket.send(packet);
	}
}

void MultiplayerGameState::DisableAllRealtimeActions()
{
	m_active_state = false;
	m_players[m_local_player_identifier]->DisableAllRealtimeActions();
}

void MultiplayerGameState::UpdateBroadcastMessage(sf::Time elapsed_time)
{
	if (m_broadcasts.empty())
	{
		return;
	}

	//Update broadcast timer
	m_broadcast_elapsed_time += elapsed_time;
	if (m_broadcast_elapsed_time > sf::seconds(2.f))
	{
		//If message has expired, remove it
		m_broadcasts.erase(m_broadcasts.begin());

		//Continue to display the next broadcast message
		if (!m_broadcasts.empty())
		{
			m_broadcast_text.setString(m_broadcasts.front());
			Utility::CentreOrigin(m_broadcast_text);
			m_broadcast_elapsed_time = sf::Time::Zero;
		}
	}
}

void MultiplayerGameState::HandlePacket(sf::Int32 packet_type, sf::Packet& packet)
{
	switch (static_cast<Server::PacketType>(packet_type))
	{
		//Send message to all Clients
	case Server::PacketType::BroadcastMessage:
		{
			std::string message;
			packet >> message;
			m_broadcasts.push_back(message);

			//Just added the first message, display immediately
			if (m_broadcasts.size() == 1)
			{
				m_broadcast_text.setString(m_broadcasts.front());
				Utility::CentreOrigin(m_broadcast_text);
				m_broadcast_elapsed_time = sf::Time::Zero;
			}
		}
		break;

		//Sent by the server to spawn player 1 airplane on connect
	case Server::PacketType::SpawnSelf:
		{
			sf::Int32 tank_identifier;
			sf::Vector2f tank_position;
			packet >> tank_identifier >> tank_position.x >> tank_position.y;
			Tank* tank = m_world.AddTank(tank_identifier);
			tank->setPosition(tank_position);
			m_players[tank_identifier].reset(new Player(&m_socket, tank_identifier, GetContext().keys1));
			m_local_player_identifier = tank_identifier;
			m_game_started = true;
		}
		break;

	case Server::PacketType::PlayerConnect:
		{
			sf::Int32 tank_identifier;
			sf::Vector2f aircraft_position;
			packet >> tank_identifier >> aircraft_position.x >> aircraft_position.y;

			Tank* tank = m_world.AddTank(tank_identifier, TankType::kPlayer2Tank);
			tank->setPosition(aircraft_position);
			m_players[tank_identifier].reset(new Player(&m_socket, tank_identifier, nullptr));
		}
		break;

	case Server::PacketType::PlayerDisconnect:
		{
			sf::Int32 tank_identifier;
			packet >> tank_identifier;
			m_world.RemoveTank(tank_identifier);
			m_players.erase(tank_identifier);
		}
		break;

	case Server::PacketType::InitialState:
		{
			sf::Int32 tank_count;
			float world_width, world_height;
			packet >> world_width >> world_height;

			packet >> tank_count;
			for (sf::Int32 i = 0; i < tank_count; ++i)
			{
				sf::Int32 tank_identifier;
				sf::Int32 hitpoints;
				sf::Int32 missile_ammo;
				sf::Vector2f aircraft_position;
				packet >> tank_identifier >> aircraft_position.x >> aircraft_position.y >> hitpoints >> missile_ammo;

				Tank* tank = m_world.AddTank(tank_identifier, TankType::kPlayer2Tank);
				tank->setPosition(aircraft_position);
				tank->SetHitpoints(hitpoints);
				tank->SetAmmo(missile_ammo);

				m_players[tank_identifier].reset(new Player(&m_socket, tank_identifier, nullptr));
			}
		}
		break;

		//Player event, like missile fired occurs
	case Server::PacketType::PlayerEvent:
		{
			sf::Int32 aircraft_identifier;
			sf::Int32 action;
			packet >> aircraft_identifier >> action;

			auto itr = m_players.find(aircraft_identifier);
			if (itr != m_players.end())
			{
				itr->second->HandleNetworkEvent(static_cast<PlayerAction>(action), m_world.getCommandQueue());
			}
		}
		break;

		//Player's movement or fire keyboard state changes
	case Server::PacketType::PlayerRealtimeChange:
		{
			sf::Int32 aircraft_identifier;
			sf::Int32 action;
			bool action_enabled;
			packet >> aircraft_identifier >> action >> action_enabled;

			auto itr = m_players.find(aircraft_identifier);
			if (itr != m_players.end())
			{
				itr->second->HandleNetworkRealtimeChange(static_cast<PlayerAction>(action), action_enabled);
			}
		}
		break;


		//Mission Successfully completed
	case Server::PacketType::MissionSuccess:
		{
			RequestStackPush(StateID::kMissionSuccess);
		}
		break;

		//Pickup created
	case Server::PacketType::SpawnPickup:
		{
			sf::Int32 type;
			sf::Vector2f position;
			packet >> type >> position.x >> position.y;
			m_world.CreatePickup(position, static_cast<PickupType>(type));
		}
		break;

	case Server::PacketType::UpdateClientState:
		{
			sf::Int32 tank_count;
			packet >> tank_count;

			for (sf::Int32 i = 0; i < tank_count; ++i)
			{
				sf::Vector2f tank_position;
				sf::Int32 tank_identifier;
				sf::Int32 tank_hitpoints;
				packet >> tank_identifier >> tank_hitpoints >> tank_position.x >> tank_position.y;

				Tank* tank = m_world.GetTank(tank_identifier);
				bool is_local_tank = m_local_player_identifier == tank_identifier;
				if (tank && !is_local_tank)
				{
					sf::Vector2f interpolated_position = tank->getPosition() + (tank_position - tank->getPosition()) *
						0.1f;
					tank->setPosition(interpolated_position);
					tank->setHitpoints(tank_hitpoints);
				}
			}
		}
		break;
	}
}
