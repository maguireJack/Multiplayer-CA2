#pragma once
#include <SFML/Graphics/Sprite.hpp>

#include "TankType.hpp"
#include "Entity.hpp"
#include "ResourceIdentifiers.hpp"
#include "Utility.hpp"

class Tank : public Entity
{
public:
	Tank(TankType type, const TextureHolder& textures);
	unsigned int GetCategory() const override;

	void Fire();
	float GetMaxSpeed();
	void Repair(int health);
	void FaceDirection(Utility::Direction dir);

private:
	void DrawCurrent(sf::RenderTarget&, sf::RenderStates states) const override;
private:
	TankType m_type;
	sf::Sprite m_sprite;

	Command m_fire_command;

	bool m_is_firing;
	sf::Time m_fire_countdown;
};

