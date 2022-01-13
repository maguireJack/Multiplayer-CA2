#include "Projectile.hpp"

#include <valarray>
#include <SFML/Graphics/RenderTarget.hpp>

#include "DataTables.hpp"
#include "ResourceHolder.hpp"
#include "SoundNode.hpp"
#include "Utility.hpp"

namespace
{
	const std::vector<ProjectileData> Table = InitializeProjectileData();
}

Projectile::Projectile(ProjectileType type, const TextureHolder& textures)
	: Entity(1)
	  , m_type(type)
	  , m_sprite(textures.Get(Table[static_cast<int>(type)].m_texture))
	  , m_explosion(textures.Get(Textures::kExplosion))
{
	//Setup Animation
	m_explosion.SetFrameSize(sf::Vector2i(256, 256));
	m_explosion.SetNumFrames(16);
	m_explosion.SetDuration(sf::seconds(1));
	Utility::CentreOrigin(m_explosion);

	Utility::CentreOrigin(m_sprite);
}

unsigned int Projectile::GetCategory() const
{
	if (m_type == ProjectileType::kPlayer1Bullet || m_type == ProjectileType::kPlayer1Missile)
		return static_cast<int>(Category::kPlayer1Projectile);
	else
		return static_cast<int>(Category::kPlayer2Projectile);
}

sf::FloatRect Projectile::GetBoundingRect() const
{
	if (m_is_exploding)
		return GetWorldTransform().transformRect(sf::FloatRect(m_explosion.GetGlobalBounds().left - m_explosion.GetGlobalBounds().width/2.f, m_explosion.GetGlobalBounds().top - m_explosion.GetGlobalBounds().height/2.f, m_explosion.GetGlobalBounds().width, m_explosion.GetGlobalBounds().height));
	else
		return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}

float Projectile::GetMaxSpeed() const
{
	return Table[static_cast<int>(m_type)].m_speed;
}

int Projectile::GetDamage() const
{
	return Table[static_cast<int>(m_type)].m_damage;
}

bool Projectile::IsMissile() const
{
	return m_type == ProjectileType::kPlayer1Missile || m_type == ProjectileType::kPlayer2Missile;
}

void Projectile::Destroy()
{
	if (!IsMissile())
		Entity::Destroy();
	else
	{
		m_is_exploding = true;
		setScale(0.25f, 0.25f);
	}
}

bool Projectile::IsDestroyed() const
{
	return Entity::IsDestroyed();
}

bool Projectile::CanDamagePlayers()
{
	return !m_damaged_player;
}

void Projectile::AppliedPlayerDamage()
{
	m_damaged_player = true;
}

void Projectile::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
	if (m_is_exploding)
	{
		if(!m_explode_sound_played)
		{
			PlaySound(commands, SoundEffect::kMissileExplosion1);
			m_explode_sound_played = true;
		}
		m_explosion.Update(dt);
		if (m_explosion.IsFinished())
			Entity::Destroy();
	}
	else Entity::UpdateCurrent(dt, commands);
}

void Projectile::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (m_is_exploding)
		target.draw(m_explosion, states);
	else target.draw(m_sprite, states);
}

void Projectile::PlaySound(CommandQueue& commands, SoundEffect effect, bool global)
{
	sf::Vector2f world_position = GetWorldPosition();

	Command command;
	command.category = Category::kSoundEffect;
	command.action = DerivedAction<SoundNode>(
		[effect, world_position, global](SoundNode& node, sf::Time)
		{
			if (global)
				node.PlaySound(effect);
			else
				node.PlaySound(effect, world_position);
		});

	commands.Push(command);
}
