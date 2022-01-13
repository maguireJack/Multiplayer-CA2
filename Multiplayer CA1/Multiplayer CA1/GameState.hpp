#pragma once
#include "BoundLabel.hpp"
#include "Container.hpp"
#include "Image.hpp"
#include "State.hpp"
#include "World.hpp"

class GameState : public State
{
public:
	GameState(StateStack& stack, Context context);
	virtual void Draw();
	virtual bool Update(sf::Time dt);
	void UpdateLabels() const;
	virtual bool HandleEvent(const sf::Event& event);

private:
	void UpdateIcons();
	void BindGui(const FontHolder& fonts);
	void CreatePlayerLabels(const FontHolder& fonts, sf::Vector2f offset, int text_size,
	                       std::string prefix, std::function<std::function<std::string()>(const Tank* const t)> func_factory);
	void CreateLabel(const FontHolder& fonts, sf::Color text_color, sf::Vector2f position, int text_size, std::string prefix, std::function<std::string()> update_action);

private:
	World m_world;
	GUI::Container m_container;
	std::vector<GUI::BoundLabel::Ptr> m_bound_labels;
	Player& m_player;

	sf::FloatRect m_gui_area;
	sf::Vector2f m_gui_center;

	Image::Ptr m_player1_explosion_upgrade_image;
	Image::Ptr m_player2_explosion_upgrade_image;
	Image::Ptr m_player1_fire_rate_upgrade_image;
	Image::Ptr m_player2_fire_rate_upgrade_image;
};

