//Alex Nogueira / D00242564
#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include <functional>

#include "Animation.hpp"
#include "TankType.hpp"
#include "Entity.hpp"
#include "ProjectileType.hpp"
#include "ResourceIdentifiers.hpp"
#include "Utility.hpp"
#include "Projectile.hpp"

enum TankActions
{
	kFiring = 1 << 1,
	kHit = 1 << 2,
	kRepair = 1 << 3,
	kRestock = 1 << 4,
	kExplosiveUpgrade = 1 << 5,
	kFireRateUpgrade = 1 << 6,
	kUpgradeFailed = 1 << 7
};

class Tank : public Entity
{
public:
	Tank(TankType type, const TextureHolder& textures, const FontHolder& fonts, bool* is_ghost_world);
	void SetSpawnPos(sf::Vector2f position);
	bool IsLocalTank() const;
	unsigned int GetCategory() const override;
	int GetIdentifier() const;
	void SetIdentifier(int identifier);

	void Fire();
	bool IsGhost() const;
	void CreateProjectile(SceneNode& node, ProjectileType type, const TextureHolder& textures, bool isExplosive = false);
	float GetMaxSpeed();
	void Repair(int health);
	void Damage(unsigned points) override;
	void Destroy() override;
	void GetExplosiveShots();
	void GetIncreasedFireRate();
	void FaceDirection(Utility::Direction dir);
	int GetAmmo() const;
	void ReplenishAmmo();
	bool IsExploding() const;
	bool IsDestroyed() const override;
	bool HasExplosiveShotsUpgrade() const;
	bool HasFireRateUpgrade() const;
	void SetAmmo(int ammo);
	void SetHitpoints(int damage);
	void TurnToGhost();
	void SetMapBounds(sf::FloatRect bounds);

	sf::FloatRect GetBoundingRect() const override;
	void ResetToLastPos();

protected:
	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;
	void PlaySound(CommandQueue& commands, SoundEffect effect, bool global = false);
private:
	void UpdateTank(sf::Time dt, CommandQueue& commands);
	void UpdateExplosion(sf::Time dt, CommandQueue& commands);
	void DrawCurrent(sf::RenderTarget&, sf::RenderStates states) const override;

public:
	std::function<void()> OnFinishExploding;

private:
	TankType m_type;
	sf::Sprite m_sprite;
	sf::Sprite m_explosive_shots_sprite;
	sf::Sprite m_fire_rate_sprite;
	Animation m_explosion;

	sf::Time m_fire_cooldown;
	sf::Time m_fire_interval;
	Command m_fire_command;
	Command m_explosive_fire_command;
	int m_ammo;
	int m_actions;
	bool m_destroy_sound_played;
	sf::Time m_fire_countdown;

	sf::Time m_explosive_shot_countdown;
	sf::Time m_fire_rate_countdown;

	sf::Vector2f m_last_pos;
	int m_identifier;

	bool m_has_listener;
	bool m_is_local;
	bool m_is_ghost = false;
	bool* m_is_ghost_world;
	sf::Vector2f m_spawn_pos;
	std::vector<Projectile*> projectiles;
	sf::Text m_playerName;
	sf::FloatRect m_bounds;
};

