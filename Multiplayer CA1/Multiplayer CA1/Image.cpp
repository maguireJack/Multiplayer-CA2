//Alex Nogueira / D00242564 
#include "Image.hpp"
#include <SFML/Graphics/RenderTarget.hpp>


Image::Image(const FontHolder& fonts, const TextureHolder& textures, Textures texture)
: m_sprite(textures.Get(texture))
{
}

bool Image::IsActive() const
{
	return Component::IsActive();
}

bool Image::IsSelectable() const
{
	return false;
}

void Image::Select()
{
	Component::Select();
}

void Image::Deselect()
{
	Component::Deselect();
}

void Image::Activate()
{
	Component::Activate();
}

void Image::Deactivate()
{
	Component::Deactivate();
}

void Image::HandleEvent(const sf::Event& event)
{
}

void Image::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (IsActive()) {
		states.transform *= getTransform();
		target.draw(m_sprite, states);
	}
}
