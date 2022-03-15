//Alex Nogueira / D00242564 
#include "BoundLabel.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

GUI::BoundLabel::BoundLabel(const FontHolder& fonts, int characterSize, std::string prefix, std::function<std::string()> update_action, sf::Color text_color)
	: Label(prefix + " " + update_action(), fonts, characterSize)
, m_prefix(prefix)
, m_update_action(update_action)
{
	m_text.setOutlineColor(sf::Color::Black);
	m_text.setOutlineThickness(5);
	m_text.setFillColor(text_color);
}

void GUI::BoundLabel::Update()
{
	SetText(m_prefix + " " + m_update_action());
}
