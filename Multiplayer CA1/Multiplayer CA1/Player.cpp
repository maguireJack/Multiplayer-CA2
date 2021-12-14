#include "Player.hpp"
#include <algorithm>
#include "Tank.hpp"

struct TankMover
{
	TankMover(float vx, float vy, Utility::Direction direction) : velocity(vx, vy), dir(direction)
	{
		
	}

	void operator()(Tank& tank, sf::Time) const
	{
		tank.FaceDirection(dir);
		tank.Accelerate(velocity * tank.GetMaxSpeed());
	}

	Utility::Direction dir;
	sf::Vector2f velocity;
};

Player::Player()
{
	//Set initial key bindings
	int playerActionsCount = 5;
	m_key_binding[sf::Keyboard::A] = PlayerAction::kPlayer1MoveLeft;
	m_key_binding[sf::Keyboard::D] = PlayerAction::kPlayer1MoveRight;
	m_key_binding[sf::Keyboard::W] = PlayerAction::kPlayer1MoveUp;
	m_key_binding[sf::Keyboard::S] = PlayerAction::kPlayer1MoveDown;
	m_key_binding[sf::Keyboard::F] = PlayerAction::kPlayer1Fire;

	m_key_binding[sf::Keyboard::Left] = PlayerAction::kPlayer2MoveLeft;
	m_key_binding[sf::Keyboard::Right] = PlayerAction::kPlayer2MoveRight;
	m_key_binding[sf::Keyboard::Up] = PlayerAction::kPlayer2MoveUp;
	m_key_binding[sf::Keyboard::Down] = PlayerAction::kPlayer2MoveDown;
	m_key_binding[sf::Keyboard::Num0] = PlayerAction::kPlayer2Fire;

	//Set initial action bindings
	InitialiseActions();
}


void Player::HandleEvent(const sf::Event& event, CommandQueue& commands)
{
	if(event.type == sf::Event::KeyPressed)
	{
		auto found = m_key_binding.find(event.key.code);
		if(found != m_key_binding.end() && !IsRealtimeAction(found->second))
		{
			commands.Push(m_action_binding[found->second]);
		}
	}
}

void Player::HandleRealtimeInput(CommandQueue& commands)
{
	//Check if any keybinding keys are pressed
	for(auto pair: m_key_binding)
	{
		if(sf::Keyboard::isKeyPressed(pair.first) && IsRealtimeAction(pair.second))
		{
			commands.Push(m_action_binding[pair.second]);
		}
	}
}

void Player::AssignKey(PlayerAction action, sf::Keyboard::Key key)
{
	//Remove all keys that are already bound to action
	for(auto itr = m_key_binding.begin(); itr != m_key_binding.end();)
	{
		if(itr->second == action)
		{
			m_key_binding.erase(itr++);
		}
		else
		{
			++itr;
		}
	}
	m_key_binding[key] = action;
}

sf::Keyboard::Key Player::GetAssignedKey(PlayerAction action) const
{
	for(auto pair : m_key_binding)
	{
		if(pair.second == action)
		{
			return pair.first;
		}
	}
	return sf::Keyboard::Unknown;
}

void Player::InitialiseActions()
{
	const float player_speed = 200.f;

	auto moveLeft = DerivedAction<Tank>(TankMover(-1, 0.f, Utility::Left));
	auto moveRight = DerivedAction<Tank>(TankMover(+1, 0.f, Utility::Right));
	auto moveUp = DerivedAction<Tank>(TankMover(0, -1.f, Utility::Up));
	auto moveDown = DerivedAction<Tank>(TankMover(0, 1.f, Utility::Down));
	auto fire = DerivedAction<Tank>([](Tank& a, sf::Time
		)
		{
			a.Fire();
		});

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

	m_action_binding[PlayerAction::kPlayer2MoveLeft].action = moveLeft;
	m_action_binding[PlayerAction::kPlayer2MoveRight].action = moveRight;
	m_action_binding[PlayerAction::kPlayer2MoveUp].action = moveUp;
	m_action_binding[PlayerAction::kPlayer2MoveDown].action = moveDown;
	m_action_binding[PlayerAction::kPlayer2Fire].action = fire;
	m_action_binding[PlayerAction::kPlayer2MoveLeft].category = Category::kPlayer2Tank;
	m_action_binding[PlayerAction::kPlayer2MoveRight].category = Category::kPlayer2Tank;
	m_action_binding[PlayerAction::kPlayer2MoveUp].category = Category::kPlayer2Tank;
	m_action_binding[PlayerAction::kPlayer2MoveDown].category = Category::kPlayer2Tank;
	m_action_binding[PlayerAction::kPlayer2Fire].category = Category::kPlayer2Tank;
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
