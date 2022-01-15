//Alex Nogueira / D00242564
#pragma once
#include <SFML/Graphics/Sprite.hpp>

#include "Application.hpp"
#include "Component.hpp"

class Image : public GUI::Component
{
public:
	typedef std::shared_ptr<Image> Ptr;
public:
	Image(const FontHolder& fonts, const TextureHolder& textures, Textures texture);
	bool IsSelectable() const override;
	void Select() override;
	void Deselect() override;
	bool IsActive() const override;
	void Activate() override;
	void Deactivate() override;
	void HandleEvent(const sf::Event& event) override;
protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	sf::Sprite m_sprite;
};

