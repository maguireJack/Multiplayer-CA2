//Alex Nogueira / D00242564
#pragma once
#include "SceneNode.hpp"
#include <SFML/Graphics/Sprite.hpp>

class SpriteNode : public SceneNode
{
public:
	explicit SpriteNode(World* world, const sf::Texture& texture);
	SpriteNode(World* world, const sf::Texture& texture, const sf::IntRect& textureRect);

private:
	virtual void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	sf::Sprite m_sprite;
};

