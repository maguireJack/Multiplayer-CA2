#pragma once
#include "State.hpp"
#include "Container.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

class MenuState : public State
{
public:
	MenuState(StateStack& stack, Context context);
	virtual void Draw();
	virtual bool Update(sf::Time dt);
	virtual bool HandleEvent(const sf::Event& event);
private:
	void ShowHowToPlayImage();

private:
	sf::Sprite m_background_sprite;
	sf::Sprite m_how_to_play_sprite;
	sf::Texture m_how_to_play_tex;
	sf::Texture m_how_to_play_blink_tex;
	sf::Time m_how_to_play_timer;
	bool m_how_to_play_enabled;
	bool m_how_to_play_blink;
	GUI::Container m_gui_container;
};

