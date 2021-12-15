#pragma once
#include "BoundLabel.hpp"
#include "Container.hpp"
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
	void BindGui(const FontHolder& fonts);
	void CreateLabel(const FontHolder& fonts, std::string key, sf::Vector2f position, int text_size, std::string prefix, std::function<std::string()> update_action);

private:
	World m_world;
	GUI::Container m_container;
	std::vector<GUI::BoundLabel::Ptr> m_bound_labels;
	Player& m_player;
};

