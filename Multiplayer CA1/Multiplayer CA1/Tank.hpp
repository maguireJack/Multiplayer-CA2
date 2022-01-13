#pragma once
#include <SFML/Graphics/Sprite.hpp>

#include "TankType.hpp"
#include "Entity.hpp"
#include "ProjectileType.hpp"
#include "ResourceIdentifiers.hpp"
#include "Utility.hpp"

enum TankActions
{
	Firing = 1 << 1,
	Hit = 1 << 2,
	Repair = 1 << 3,
	Restock = 1 << 4
};

class Tank : public Entity
{
public:
	Tank(TankType type, const TextureHolder& textures);
	bool IsPlayer1Tank() const;
	unsigned int GetCategory() const override;

	void Fire();
	void CreateProjectile(SceneNode& node, ProjectileType type, const TextureHolder& textures) const;
	float GetMaxSpeed();
	void Repair(int health);
	void Damage(unsigned points) override;
	void Destroy() override;
	void FaceDirection(Utility::Direction dir);
	int GetAmmo() const;
	void ReplenishAmmo();
	bool IsExploding();
	bool IsDestroyed() const override;

	sf::FloatRect GetBoundingRect() const override;
	void ResetToLastPos();

protected:
	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;
	void PlaySound(CommandQueue& commands, SoundEffect effect, bool global = true);
private:
	void DrawCurrent(sf::RenderTarget&, sf::RenderStates states) const override;
private:
	TankType m_type;
	sf::Sprite m_sprite;

	sf::Time m_fire_cooldown;
	sf::Time m_fire_interval;
	Command m_fire_command;
	int m_ammo;
	int m_actions;
	bool m_destroy_sound_played;
	sf::Time m_fire_countdown;

	sf::Vector2f m_last_pos;
};

