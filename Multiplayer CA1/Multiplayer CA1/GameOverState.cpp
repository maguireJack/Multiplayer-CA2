//Alex Nogueira / D00242564 
#include "GameOverState.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

#include "Category.hpp"
#include "Player.hpp"
#include "ResourceHolder.hpp"
#include "Utility.hpp"

GameOverState::GameOverState(StateStack& stack, Context context)
	: State(stack, context)
	, m_dimmer(context.window->getView().getSize())
	, m_finish_text()
	, m_winner_text()
	, m_instruction_text()
{	
	m_dimmer.setFillColor(sf::Color(0, 0, 0, 150));

	sf::Font& font = context.fonts->Get(Fonts::Main);
	sf::Vector2f viewSize(context.window->getView().getSize());

	m_finish_text.setFont(font);
	m_finish_text.setString("Game Over!");
	m_finish_text.setCharacterSize(70);
	Utility::CentreOrigin(m_finish_text);
	m_finish_text.setPosition(0.5f * viewSize.x, 0.2f * viewSize.y);

	m_winner_text.setFont(font);
	m_winner_text.setString(GetWinnerText(context.player->m_winner));
	m_winner_text.setCharacterSize(70);
	Utility::CentreOrigin(m_winner_text);
	m_winner_text.setPosition(0.5f * viewSize.x, 0.4f * viewSize.y);

	m_instruction_text.setFont(font);
	m_instruction_text.setString("Press Escape to Leave to Menu");
	m_instruction_text.setCharacterSize(50);
	Utility::CentreOrigin(m_instruction_text);
	m_instruction_text.setPosition(0.5f * viewSize.x, 0.6f * viewSize.y);
}

std::string GameOverState::GetWinnerText(Category::Type winner) const
{
	if (winner & Category::kPlayer1Tank && winner & Category::kPlayer2Tank)
	{
		return "It's a Draw!";
	}
	if (winner == Category::kPlayer1Tank)
	{
		return "Player 1 won!";
	}
	if (winner == Category::kPlayer2Tank)
	{
		return "Player 2 won!";
	}

	return "Invalid Winner";
}

void GameOverState::Draw()
{
	sf::RenderWindow& window = *GetContext().window;
	window.setView(window.getDefaultView());

	window.draw(m_dimmer);
	window.draw(m_finish_text);
	window.draw(m_winner_text);
	window.draw(m_instruction_text);
}

bool GameOverState::Update(sf::Time)
{
	return false;
}

bool GameOverState::HandleEvent(const sf::Event& event)
{
	if (event.type != sf::Event::KeyPressed)
		return false;

	if (event.key.code == sf::Keyboard::Escape)
	{
		RequestStackClear();
		RequestStackPush(StateID::kMenu);
	}

	return false;
}