#include "GameState.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include "Player.hpp"

GameState::GameState(StateStack& stack, Context context)
: State(stack, context)
, m_world(*context.window, *context.fonts)
, m_player(*context.player)
, m_gui_area(0,600, 900, 200)
, m_gui_center(m_gui_area.left + m_gui_area.width/2.f, m_gui_area.top+ m_gui_area.height/2.f)
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
	sf::Vector2f offset(-300, -60);
	m_container.setPosition(m_gui_center);
	CreatePlayerLabels(fonts, offset, 30, "Health:", [this](const Tank * const tank)
	{
		return [tank] { return std::to_string(tank->GetHitPoints()); };
	});
	offset += sf::Vector2f(0, 60);
	CreatePlayerLabels(fonts, offset, 30, "Ammo:", [this](const Tank* const tank)
		{
			return [tank] { return std::to_string(tank->GetAmmo()); };
		});
}

void GameState::CreatePlayerLabels(const FontHolder& fonts, sf::Vector2f offset, int text_size,
                                  std::string prefix, std::function<std::function<std::string()>(const Tank* const t)> func_factory)
{
	CreateLabel(fonts, sf::Color::Blue, offset, text_size, "P1 " + prefix, func_factory(m_world.GetPlayer1()));
	CreateLabel(fonts, sf::Color::Green, sf::Vector2f(-offset.x, offset.y), text_size, "P2 " + prefix, func_factory(m_world.GetPlayer2()));
}

void GameState::CreateLabel(const FontHolder& fonts, sf::Color text_color, sf::Vector2f position, int text_size,
                            std::string prefix, std::function<std::string()> update_action)
{
	GUI::BoundLabel::Ptr bound_label(new GUI::BoundLabel(fonts, text_size, prefix, update_action, text_color));
	bound_label->CentreText();
	bound_label->setPosition(position);
	m_container.Pack(bound_label);
	m_bound_labels.emplace_back(bound_label);
}
