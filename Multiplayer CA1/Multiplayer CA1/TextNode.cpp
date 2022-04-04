//Alex Nogueira / D00242564 
#include "TextNode.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include "ResourceHolder.hpp"
#include "Utility.hpp"

TextNode::TextNode(World* world, const FontHolder& fonts, const std::string& text)
	: SceneNode(world), m_text(text, fonts.Get(Fonts::Main), 20)
{
}

void TextNode::SetString(const std::string& text)
{
	m_text.setString(text);
	Utility::CentreOrigin(m_text);
}

void TextNode::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_text, states);
}
