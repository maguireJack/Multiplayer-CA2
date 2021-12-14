#include "Block.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

Block::Block(sf::Texture* texture)
	: Entity(1)
, m_sprite(*texture)
{
	setScale(5, 5);
}

unsigned Block::GetCategory() const
{
	return Category::kTile;
}

sf::FloatRect Block::GetBoundingRect() const
{
	return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}

void Block::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_sprite, states);
}
