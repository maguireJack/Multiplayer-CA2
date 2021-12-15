#pragma once
#include <SFML/Graphics/Sprite.hpp>

#include "Entity.hpp"
#include "SceneNode.hpp"

class Tile : public Entity
{
public:
	Tile(sf::Texture& texture, Category::Type tileCategory);
	unsigned GetCategory() const override;
	sf::FloatRect GetBoundingRect() const override;
private:
	void DrawCurrent(sf::RenderTarget&, sf::RenderStates states) const override;
	sf::Sprite m_sprite;
};

