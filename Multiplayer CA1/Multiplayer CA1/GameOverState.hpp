#pragma once
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include "Category.hpp"
#include "State.hpp"

class GameOverState : public State
{
public:
	GameOverState(StateStack& stack, Context context);
	std::string GetWinnerText(Category::Type winner) const;

	virtual void		Draw();
	virtual bool		Update(sf::Time dt);
	virtual bool		HandleEvent(const sf::Event& event);


private:
	sf::RectangleShape  m_dimmer;
	sf::Text			m_finish_text;
	sf::Text			m_winner_text;
	sf::Text			m_instruction_text;
};
