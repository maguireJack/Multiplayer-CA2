#include "SettingsState.hpp"
#include "Utility.hpp"
#include "ResourceHolder.hpp"
#include "StateStack.hpp"

#include <SFML/Graphics/RenderWindow.hpp>


SettingsState::SettingsState(StateStack& stack, Context context)
	: State(stack, context)
	, m_gui_container()
{
	m_background_sprite.setTexture(context.textures->Get(Textures::kTitleScreen));

	// Build key binding buttons and labels
	for (std::size_t x = 0; x < 2; ++x)
	{
		AddButtonLabel(static_cast<int>(PlayerAction::kPlayerMoveLeft), x, 0, "Move Left", context);
		AddButtonLabel(static_cast<int>(PlayerAction::kPlayerMoveRight	), x, 1, "Move Right", context);
		AddButtonLabel(static_cast<int>(PlayerAction::kPlayerMoveUp), x, 2, "Move Up", context);
		AddButtonLabel(static_cast<int>(PlayerAction::kPlayerMoveDown), x, 3, "Move Down", context);
		AddButtonLabel(static_cast<int>(PlayerAction::kPlayerFire), x, 4, "Fire", context);
	}

	UpdateLabels();

	auto back_button = std::make_shared<GUI::Button>(context);
	back_button->setPosition(80.f, 620.f);
	back_button->SetText("Back");
	back_button->SetCallback(std::bind(&SettingsState::RequestStackPop, this));

	m_gui_container.Pack(back_button);
}

void SettingsState::Draw()
{
	sf::RenderWindow& window = *GetContext().window;

	window.draw(m_background_sprite);
	window.draw(m_gui_container);
}

bool SettingsState::Update(sf::Time)
{
	return true;
}

bool SettingsState::HandleEvent(const sf::Event& event)
{
	bool is_key_binding = false;

	// Iterate through all key binding buttons to see if they are being pressed, waiting for the user to enter a key
	for (std::size_t i = 0; i < 2 * (static_cast<int>(PlayerAction::kActionCount)); ++i)
	{
		if (m_binding_buttons[i]->IsActive())
		{
			is_key_binding = true;
			if (event.type == sf::Event::KeyReleased)
			{
				// Player 1
				if (i < static_cast<int>(PlayerAction::kActionCount))
					GetContext().keys1->AssignKey(static_cast<PlayerAction>(i), event.key.code);

				m_binding_buttons[i]->Deactivate();
			}
			break;
		}
	}

	// If pressed button changed key bindings, update labels; otherwise consider other buttons in container
	if (is_key_binding)
		UpdateLabels();
	else
		m_gui_container.HandleEvent(event);

	return false;
}

void SettingsState::UpdateLabels()
{
	for (std::size_t i = 0; i < static_cast<int>(PlayerAction::kActionCount); ++i)
	{
		auto action = static_cast<PlayerAction>(i);

		// Get keys of both players
		sf::Keyboard::Key key1 = GetContext().keys1->GetAssignedKey(action);

		// Assign both key strings to labels
		m_binding_labels[i]->SetText(Utility::toString(key1));
	}
}

void SettingsState::AddButtonLabel(std::size_t index, std::size_t x, std::size_t y, const std::string& text, Context context)
{
	// For x==0, start at index 0, otherwise start at half of array
	index += static_cast<int>(PlayerAction::kActionCount) * x;

	m_binding_buttons[index] = std::make_shared<GUI::Button>(context);
	m_binding_buttons[index]->setPosition(400.f * x + 80.f, 50.f * y + 300.f);
	m_binding_buttons[index]->SetText(text);
	m_binding_buttons[index]->SetToggle(true);

	m_binding_labels[index] = std::make_shared<GUI::Label>("", *context.fonts);
	m_binding_labels[index]->setPosition(400.f * x + 300.f, 50.f * y + 315.f);

	m_gui_container.Pack(m_binding_buttons[index]);
	m_gui_container.Pack(m_binding_labels[index]);
}


