#include "GameState.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include "Player.hpp"

GameState::GameState(StateStack& stack, Context context)
: State(stack, context)
, m_world(*context.window, *context.fonts)
, m_player(*context.player)
{
	BindGui(*context.fonts);
}

void GameState::Draw()
{
	m_world.Draw();
	GetContext().window->draw(m_container);
}

bool GameState::Update(sf::Time dt)
{
	m_world.Update(dt);
	CommandQueue& commands = m_world.getCommandQueue();
	m_player.HandleRealtimeInput(commands);
	UpdateLabels();
	return true;
}

void GameState::UpdateLabels() const
{
	for(auto label : m_bound_labels)
	{
		label.get()->Update();
	}
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

void GameState::BindGui(const FontHolder& fonts)
{
	m_container.setPosition(0, 600);
	CreateLabel(fonts, "P1Health", sf::Vector2f(0, 0), 30, "P1 Health:", [this]
		{
			return std::to_string(m_world.GetPlayer1()->GetHitPoints());
		});
}

void GameState::CreateLabel(const FontHolder& fonts, std::string key, sf::Vector2f position, int text_size,
                            std::string prefix, std::function<std::string()> update_action)
{
	GUI::BoundLabel::Ptr bound_label(new GUI::BoundLabel(fonts, text_size, prefix, update_action));
	bound_label->setPosition(position);
	m_container.Pack(bound_label);
	m_bound_labels.emplace_back(bound_label);
}
