//Alex Nogueira / D00242564
#pragma once
#include <SFML/Graphics/Sprite.hpp>

#include "Animation.hpp"
#include "Entity.hpp"
#include "ProjectileType.hpp"
#include "ResourceIdentifiers.hpp"


class Projectile : public Entity
{
public:
	Projectile(ProjectileType type, const TextureHolder& textures, bool is_ghost);

	virtual unsigned int GetCategory() const override;
	sf::FloatRect GetBoundingRect() const override;
	float GetMaxSpeed() const;
	int GetDamage() const;
	bool IsMissile() const;
	void Destroy() override;
	bool IsDestroyed() const override;
	bool CanDamagePlayers();
	void AppliedPlayerDamage();

private:
	virtual void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;
	virtual void DrawCurrent(sf::RenderTarget&, sf::RenderStates states) const override;
	void PlaySound(CommandQueue& commands, SoundEffect effect, bool global = true);

private:
	Animation m_explosion;
	ProjectileType m_type;
	sf::Sprite m_sprite;
	sf::Vector2f m_target_direction;
	bool m_is_exploding;
	bool m_explode_sound_played;
	bool m_damaged_player;
	bool m_is_ghost;
};

