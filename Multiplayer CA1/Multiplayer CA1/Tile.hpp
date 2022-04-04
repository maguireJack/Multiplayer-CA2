//Alex Nogueira / D00242564
#pragma once
#include <SFML/Graphics/Sprite.hpp>

#include "Entity.hpp"
#include "SceneNode.hpp"

class Tile : public Entity
{
public:
	Tile(World* world, sf::Texture& texture, Category::Type tileCategory);
	unsigned GetCategory() const override;
	sf::FloatRect GetBoundingRect() const override;
private:
	void DrawCurrent(sf::RenderTarget&, sf::RenderStates states) const override;
	sf::Sprite m_sprite;
};

