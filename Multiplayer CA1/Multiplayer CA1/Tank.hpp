#pragma once
#include <SFML/Graphics/Sprite.hpp>

#include "TankType.hpp"
#include "Entity.hpp"
#include "ProjectileType.hpp"
#include "ResourceIdentifiers.hpp"
#include "Utility.hpp"

class Tank : public Entity
{
public:
	Tank(TankType type, const TextureHolder& textures);
	unsigned int GetCategory() const override;

	void Fire();
	void CreateProjectile(SceneNode& node, ProjectileType type, const TextureHolder& textures) const;
	float GetMaxSpeed();
	void Repair(int health);
	void FaceDirection(Utility::Direction dir);
	int GetAmmo() const;

	sf::FloatRect GetBoundingRect() const override;
	void ResetToLastPos();

protected:
	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;
private:
	void DrawCurrent(sf::RenderTarget&, sf::RenderStates states) const override;
private:
	TankType m_type;
	sf::Sprite m_sprite;

	sf::Time m_fire_cooldown;
	sf::Time m_fire_interval;
	Command m_fire_command;
	int m_ammo;
	bool m_is_firing;
	sf::Time m_fire_countdown;

	sf::Vector2f m_last_pos;
};

