//Alex Nogueira / D00242564
#pragma once
#include <SFML/Graphics/Sprite.hpp>

#include "Entity.hpp"
#include "PickupType.hpp"
#include "ResourceIdentifiers.hpp"

class Tank;

class Pickup : public Entity
{
public:
	Pickup(World* world, PickupType type, const TextureHolder& textures);
	virtual unsigned int GetCategory() const override;
	virtual sf::FloatRect GetBoundingRect() const;
	void Apply(Tank& player) const;
	virtual void DrawCurrent(sf::RenderTarget&, sf::RenderStates states) const override;
protected:
	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;
private:
	PickupType m_type;
	sf::Sprite m_sprite;
	bool m_collider_registered;
};

