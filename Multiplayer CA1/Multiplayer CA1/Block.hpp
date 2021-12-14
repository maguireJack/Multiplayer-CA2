#pragma once
#include <SFML/Graphics/Sprite.hpp>

#include "Entity.hpp"

class Block : public Entity
{
public:
	Block(sf::Texture* texture);
	unsigned GetCategory() const override;
	sf::FloatRect GetBoundingRect() const override;
private:
	void DrawCurrent(sf::RenderTarget&, sf::RenderStates states) const override;
	sf::Sprite m_sprite;
};

