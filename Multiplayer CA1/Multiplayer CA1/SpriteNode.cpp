//Alex Nogueira / D00242564 
#include "SpriteNode.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

SpriteNode::SpriteNode(World* world, const sf::Texture& texture) : SceneNode(world), m_sprite(texture)
{
}

SpriteNode::SpriteNode(World* world, const sf::Texture& texture, const sf::IntRect& textureRect) : SceneNode(world), m_sprite(texture, textureRect)
{
}

void SpriteNode::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_sprite, states);
}
