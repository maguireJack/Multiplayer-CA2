//Alex Nogueira / D00242564 
#include "Player.hpp"
#include <algorithm>
#include "NetworkProtocol.hpp"
#include <SFML/Network/Packet.hpp>
#include "Tank.hpp"

struct TankMover
{
	TankMover(float vx, float vy, Utility::Direction direction, int id) : velocity(vx, vy), dir(direction), tank_id(id)
	{
		
	}

	void operator()(Tank& tank, sf::Time) const
	{
		if (tank.GetIdentifier() == tank_id) {
			tank.FaceDirection(dir);
			tank.SetVelocity(velocity * tank.GetMaxSpeed());
		}
	}

	Utility::Direction dir;
	int tank_id;
	sf::Vector2f velocity;
};

struct TankFire
{
	TankFire(int identifier) : tank_id(identifier)
	{
		
	}

	void operator() (Tank& tank, sf::Time) const
	{
		if (tank.GetIdentifier() == tank_id)
			tank.Fire();
	}

	int tank_id;
};

Player::Player(sf::TcpSocket* socket, sf::Int32 identifier, const KeyBinding* binding)
	: m_key_binding(binding)
	, m_current_mission_status(MissionStatus::kMissionRunning)
	, m_identifier(identifier)
	, m_socket(socket)
{
	// Set initial action bindings
	InitialiseActions();

	// Assign all categories to player's aircraft
	for (auto& pair : m_action_binding)
		pair.second.category = Category::kPlayer1Tank;
}




void Player::HandleEvent(const sf::Event& event, CommandQueue& commands)
{
	// Event
	if (event.type == sf::Event::KeyPressed)
	{
		PlayerAction action;
		if (m_key_binding && m_key_binding->CheckAction(event.key.code, action) && !IsRealtimeAction(action))
		{
			// Network connected -> send event over network
			if (m_socket)
			{
				sf::Packet packet;
				packet << static_cast<sf::Int32>(Client::PacketType::PlayerEvent);
				packet << m_identifier;
				packet << static_cast<sf::Int32>(action);
				m_socket->send(packet);
			}

			// Network disconnected -> local event
			else
			{
				commands.Push(m_action_binding[action]);
			}
		}
	}

	// Realtime change (network connected)
	if ((event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) && m_socket)
	{
		PlayerAction action;
		if (m_key_binding && m_key_binding->CheckAction(event.key.code, action) && IsRealtimeAction(action))
		{
			// Send realtime change over network
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Client::PacketType::PlayerRealtimeChange);
			packet << m_identifier;
			packet << static_cast<sf::Int32>(action);
			packet << (event.type == sf::Event::KeyPressed);
			m_socket->send(packet);
		}
	}
}

void Player::HandleRealtimeInput(CommandQueue& commands)
{
	// Check if this is a networked game and local player or just a single player game
	if ((m_socket && IsLocal()) || !m_socket)
	{
		// Lookup all actions and push corresponding commands to queue
		std::vector<PlayerAction> activeActions = m_key_binding->GetRealtimeActions();
		for (PlayerAction action : activeActions)
			commands.Push(m_action_binding[action]);
	}
}

void Player::HandleNetworkRealtimeChange(PlayerAction action, bool actionEnabled)
{
	m_action_proxies[action] = actionEnabled;
}

bool Player::IsLocal() const
{
	// No key binding means this player is remote
	return m_key_binding != nullptr;
}

void Player::DisableAllRealtimeActions()
{
	for (auto& action : m_action_proxies)
	{
		sf::Packet packet;
		packet << static_cast<sf::Int32>(Client::PacketType::PlayerRealtimeChange);
		packet << m_identifier;
		packet << static_cast<sf::Int32>(action.first);
		packet << false;
		m_socket->send(packet);
	}
}

void Player::HandleNetworkEvent(PlayerAction action, CommandQueue& commands)
{
	commands.Push(m_action_binding[action]);
}

void Player::HandleRealtimeNetworkInput(CommandQueue& commands)
{
	if (m_socket && !IsLocal())
	{
		// Traverse all realtime input proxies. Because this is a networked game, the input isn't handled directly
		for (auto pair : m_action_proxies)
		{
			if (pair.second && IsRealtimeAction(pair.first))
				commands.Push(m_action_binding[pair.first]);
		}
	}
}

void Player::SetMissionStatus(MissionStatus status)
{
	m_current_mission_status = status;
}

MissionStatus Player::GetMissionStatus() const
{
	return m_current_mission_status;
}


void Player::InitialiseActions()
{
	const float player_speed = 200.f;

	auto moveLeft = DerivedAction<Tank>(TankMover(-1, 0.f, Utility::Left, m_identifier));
	auto moveRight = DerivedAction<Tank>(TankMover(+1, 0.f, Utility::Right, m_identifier));
	auto moveUp = DerivedAction<Tank>(TankMover(0, -1.f, Utility::Up, m_identifier));
	auto moveDown = DerivedAction<Tank>(TankMover(0, 1.f, Utility::Down, m_identifier));
	auto fire = DerivedAction<Tank>(TankFire(m_identifier));

	m_action_binding[PlayerAction::kPlayer1MoveLeft].action = moveLeft;
	m_action_binding[PlayerAction::kPlayer1MoveRight].action = moveRight;
	m_action_binding[PlayerAction::kPlayer1MoveUp].action = moveUp;
	m_action_binding[PlayerAction::kPlayer1MoveDown].action = moveDown;
	m_action_binding[PlayerAction::kPlayer1Fire].action = fire;
	m_action_binding[PlayerAction::kPlayer1MoveLeft].category = Category::kPlayer1Tank;
	m_action_binding[PlayerAction::kPlayer1MoveRight].category = Category::kPlayer1Tank;
	m_action_binding[PlayerAction::kPlayer1MoveUp].category = Category::kPlayer1Tank;
	m_action_binding[PlayerAction::kPlayer1MoveDown].category = Category::kPlayer1Tank;
	m_action_binding[PlayerAction::kPlayer1Fire].category = Category::kPlayer1Tank;
}


bool Player::IsRealtimeAction(PlayerAction action)
{
	switch(action)
	{
	case PlayerAction::kPlayer1MoveLeft:
	case PlayerAction::kPlayer1MoveRight:
	case PlayerAction::kPlayer1MoveUp:
	case PlayerAction::kPlayer1MoveDown:
	case PlayerAction::kPlayer1Fire:
	case PlayerAction::kPlayer2MoveLeft:
	case PlayerAction::kPlayer2MoveRight:
	case PlayerAction::kPlayer2MoveUp:
	case PlayerAction::kPlayer2MoveDown:
	case PlayerAction::kPlayer2Fire:
		return true;
	default:
		return false;
	}
}
