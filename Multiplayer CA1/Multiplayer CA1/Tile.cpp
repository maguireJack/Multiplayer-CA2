//Alex Nogueira / D00242564 
#include "Tile.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

Tile::Tile(sf::Texture& texture, Category::Type tileCategory)
	: Entity(tileCategory == Category::kDestroyableTile ? 5 : 1)
	  , m_sprite(texture)
{
	m_category = tileCategory;
	setScale(5, 5);
	is_collidable = tileCategory == Category::kDestroyableTile;
}

unsigned Tile::GetCategory() const
{
	return m_category;
}

sf::FloatRect Tile::GetBoundingRect() const
{
	return GetCategory() == Category::kFloorTile ? sf::FloatRect() : GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}

void Tile::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_sprite, states);
}
