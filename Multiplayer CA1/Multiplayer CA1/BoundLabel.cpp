#include "BoundLabel.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

GUI::BoundLabel::BoundLabel(const FontHolder& fonts, int characterSize, std::string prefix, std::function<std::string()> update_action)
	: Label(prefix + " " + update_action(), fonts, characterSize)
, m_prefix(prefix)
, m_update_action(update_action)
{
}

void GUI::BoundLabel::Update()
{
	SetText(m_prefix + " " + m_update_action());
}
