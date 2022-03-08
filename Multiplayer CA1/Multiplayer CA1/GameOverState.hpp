//Alex Nogueira / D00242564
#pragma once
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include "Category.hpp"
#include "State.hpp"

class GameOverState : public State
{
public:
	GameOverState(StateStack& stack, Context context, const std::string& text);
	virtual void		Draw();
	virtual bool		Update(sf::Time dt);
	virtual bool		HandleEvent(const sf::Event& event);


private:
	sf::Text			m_game_over_text;
	sf::Time			m_elapsed_time;
};
