#include "GameState.hpp"

#include <iostream>

#include "Player.hpp"

GameState::GameState(StateStack& stack, Context context)
: State(stack, context)
, m_world(*context.window, *context.fonts, *context.sounds, false)
, m_player(nullptr, 1, context.keys1)
{
	m_world.AddTank(1, TankType::kLocalTank);
	m_player.SetMissionStatus(MissionStatus::kMissionRunning);
	// Play game theme
	context.music->Play(MusicThemes::kGameplayTheme);
}

void GameState::Draw()
{
	m_world.Draw();
}

bool GameState::Update(sf::Time dt)
{
	m_world.Update(dt);
	CommandQueue& commands = m_world.getCommandQueue();
	//TODO Add battle royale with cheese
	m_player.HandleRealtimeInput(commands);
	return true;
}

bool GameState::HandleEvent(const sf::Event& event)
{
	CommandQueue& commands = m_world.getCommandQueue();
	m_player.HandleEvent(event, commands);

	//Escape should bring up the Pause Menu
	if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
	{
		RequestStackPush(StateID::kPause);
	}
	return true;
}
