#include "MenuState.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

#include "ResourceHolder.hpp"
#include "Utility.hpp"
#include "Button.hpp"


MenuState::MenuState(StateStack& stack, Context context)
	: State(stack, context), m_how_to_play_timer(sf::seconds(0.5))
{
	sf::Texture& texture = context.textures->Get(Textures::kTitleScreen);

	m_how_to_play_tex = context.textures->Get(Textures::kHowToPlay);
	m_how_to_play_blink_tex = context.textures->Get(Textures::kHowToPlayBlink);

	m_background_sprite.setTexture(texture);
	m_how_to_play_sprite.setTexture(m_how_to_play_tex);

	auto play_button = std::make_shared<GUI::Button>(context);
	play_button->setPosition(100, 300);
	play_button->SetText("How to Play");
	play_button->SetCallback([this]()
	{
		ShowHowToPlayImage();
	});

	auto host_play_button = std::make_shared<GUI::Button>(context);
	host_play_button->setPosition(100, 350);
	host_play_button->SetText("Host");
	host_play_button->SetCallback([this]()
	{
		RequestStackPop();
		RequestStackPush(StateID::kHostGame);
	});

	auto join_play_button = std::make_shared<GUI::Button>(context);
	join_play_button->setPosition(100, 400);
	join_play_button->SetText("Join");
	join_play_button->SetCallback([this]()
	{
		RequestStackPop();
		RequestStackPush(StateID::kJoinGame);
	});

	auto exit_button = std::make_shared<GUI::Button>(context);
	exit_button->setPosition(100, 450);
	exit_button->SetText("Exit");
	exit_button->SetCallback([this]()
	{
		RequestStackPop();
	});

	m_gui_container.Pack(play_button);
	m_gui_container.Pack(host_play_button);
	m_gui_container.Pack(join_play_button);
	m_gui_container.Pack(exit_button);

	// Play menu theme
	context.music->Play(MusicThemes::kMenuTheme);
}

void MenuState::Draw()
{
	sf::RenderWindow& window = *GetContext().window;
	window.setView(window.getDefaultView());
	if (!m_how_to_play_enabled)
	{
		window.draw(m_background_sprite);
		window.draw(m_gui_container);
	}
	else
	{
		window.draw(m_how_to_play_sprite);
	}
}

bool MenuState::Update(sf::Time dt)
{
	if (m_how_to_play_enabled)
	{
		m_how_to_play_timer -= dt;

		if (m_how_to_play_timer.asSeconds() < 0)
		{
			if (m_how_to_play_blink) m_how_to_play_sprite.setTexture(m_how_to_play_blink_tex);
			else m_how_to_play_sprite.setTexture(m_how_to_play_tex);

			m_how_to_play_blink = !m_how_to_play_blink;
			m_how_to_play_timer = sf::seconds(0.5);
		}
	}
	return true;
}

bool MenuState::HandleEvent(const sf::Event& event)
{
	if (m_how_to_play_enabled)
	{
		if (event.type == sf::Event::KeyPressed) 
		{
			m_how_to_play_enabled = false;
			m_how_to_play_timer = sf::seconds(0.5);
		}
		return false;
	}
	m_gui_container.HandleEvent(event);
	return false;
}

void MenuState::ShowHowToPlayImage()
{
	m_how_to_play_enabled = true;
}
