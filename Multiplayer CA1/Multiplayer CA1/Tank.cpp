//Alex Nogueira / D00242564 
#include "Tank.hpp"

#include <iostream>
#include <SFML/Audio/Listener.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "DataTables.hpp"
#include "Projectile.hpp"
#include "ProjectileType.hpp"
#include "ResourceHolder.hpp"
#include "SoundNode.hpp"

namespace
{
	const std::vector<TankData> Table = InitializeTankData();
}

Tank::Tank(TankType type, const TextureHolder& textures, const FontHolder& fonts, bool* is_ghost_world)
	: Entity(Table[static_cast<int>(type)].m_hitpoints)
	  , m_sprite(textures.Get(Table[static_cast<int>(type)].m_texture))
	  , m_type(type)
	  , m_fire_interval(Table[static_cast<int>(type)].m_fire_interval)
	  , m_ammo(Table[static_cast<int>(type)].m_ammo)
	  , m_explosion(textures.Get(Textures::kExplosion))
	  , m_identifier(0)
	  , m_is_ghost_world(is_ghost_world)
	  , m_playerName("Player " + std::to_string(m_identifier), fonts.Get(Fonts::Main), 12)
{
	is_static = false;

	if (type == TankType::kLocalTank)
	{
		m_has_listener = true;
		m_is_local = true;
	}

	//Setup Animation
	m_explosion.SetFrameSize(sf::Vector2i(256, 256));
	m_explosion.SetNumFrames(16);
	m_explosion.SetDuration(sf::seconds(1));
	Utility::CentreOrigin(m_explosion);

	//Setup Sprite
	setScale(5, 5);
	Utility::CentreOrigin(m_sprite);

	FaceDirection(Utility::Down);

	//Setup commands
	m_fire_command.action = [this, &textures](SceneNode& node, sf::Time time)
	{
		CreateProjectile(node, ProjectileType::kPlayerBullet, textures);
	};
	m_fire_command.category = static_cast<int>(Category::Type::kScene);

	m_explosive_fire_command.action = [this, &textures](SceneNode& node, sf::Time time)
	{
		CreateProjectile(node, ProjectileType::kPlayerMissile, textures);
	};
	m_explosive_fire_command.category = static_cast<int>(Category::Type::kScene);
}

void Tank::SetSpawnPos(sf::Vector2f position)
{
	m_spawn_pos = position;
}

bool Tank::IsLocalTank() const
{
	return m_is_local;
}

unsigned Tank::GetCategory() const
{
	if (IsLocalTank())
	{
		if (IsGhost())
		{
			return Category::kLocalGhostTank;
		}
		else
		{
			return Category::kLocalTank;
		}
	}
	else
	{
		if (IsGhost())
		{
			return Category::kEnemyGhostTank;
		}
		else
		{
			return Category::kEnemyTank;
		}
	}
}

int Tank::GetIdentifier() const
{
	return m_identifier;
}

void Tank::SetIdentifier(int identifier)
{
	m_identifier = identifier; 
	m_playerName.setString("Player " + std::to_string(m_identifier));
}


void Tank::SetAmmo(int ammo)
{
	m_ammo = ammo;
}

void Tank::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_playerName, states);

	if (IsDestroyed()) return;

	if (IsExploding())
	{
		target.draw(m_explosion, states);
	}
	else
	{
		if (m_is_ghost)
		{
			if (*m_is_ghost_world)
			{
				target.draw(m_sprite, states);
				target.draw(m_explosive_shots_sprite, states);
			}
			return;
		}

		target.draw(m_sprite, states);
		target.draw(m_explosive_shots_sprite, states);
		//target.draw(m_fire_rate_sprite, states);
	}
}

//Tank will not keep firing when held down
void Tank::Fire()
{
	if (m_ammo > 0)
	{
		m_actions = m_actions | kFiring;
	}
}

void Tank::CreateProjectile(SceneNode& node, ProjectileType type, const TextureHolder& textures, bool isExplosive)
{
	std::unique_ptr<Projectile> projectile(new Projectile(type, textures, IsGhost()));
	sf::Vector2f offset = GetWorldTransform().transformPoint(8.f, -1.f);
	if (type == ProjectileType::kPlayerMissile)
	{
		offset = GetWorldTransform().transformPoint(10.f, -1.f);
	}
	offset -= GetWorldPosition();
	sf::Vector2f velocity(
		std::cosf(Utility::ToRadians(getRotation())) * projectile->GetMaxSpeed(),
		std::sinf(Utility::ToRadians(getRotation())) * projectile->GetMaxSpeed());

	projectile->setPosition(getPosition() + offset);
	projectile->SetVelocity(velocity);
	projectile->setRotation(getRotation() + 90);
	node.AttachChild(std::move(projectile));
}

float Tank::GetMaxSpeed()
{
	return Table[static_cast<int>(m_type)].m_speed;
}

void Tank::Repair(int health)
{
	int maxHealth = Table[static_cast<int>(m_type)].m_max_hitpoints;
	if (GetHitPoints() + health > maxHealth)
	{
		health = maxHealth - GetHitPoints();
	}

	if (health > 0)
	{
		Entity::Repair(health);
		m_actions = m_actions | TankActions::kRepair;
	}
	else m_actions = m_actions | TankActions::kUpgradeFailed;
}

void Tank::Damage(unsigned points)
{
	Entity::Damage(points);

	m_actions = m_actions | kHit;
	if (IsExploding())
		setScale(1, 1);
}

void Tank::Destroy()
{
	m_is_ghost = true;
}

bool Tank::IsGhost() const
{
	return m_is_ghost;
}

void Tank::TurnToGhost()
{
	m_is_ghost = true;
	m_sprite.setColor(sf::Color(255, 255, 255, 100));
}

void Tank::SetHitpoints(int damage)
{
	Entity::SetHitpoints(damage);
}


void Tank::GetExplosiveShots()
{
	m_explosive_shot_countdown = sf::seconds(10);
	m_actions = m_actions | kExplosiveUpgrade;
}

void Tank::GetIncreasedFireRate()
{
	m_fire_rate_countdown = sf::seconds(10);
	m_actions = m_actions | kFireRateUpgrade;
}

void Tank::FaceDirection(Utility::Direction dir)
{
	switch (dir)
	{
	case Utility::Right:
		setRotation(0);
		break;
	case Utility::Up:
		setRotation(-90.f);
		break;
	case Utility::Left:
		setRotation(180.f);
		break;
	case Utility::Down:
		setRotation(90.f);
		break;
	}
}

int Tank::GetAmmo() const
{
	return m_ammo;
}

void Tank::ReplenishAmmo()
{
	int ammoToAdd = Table[static_cast<int>(m_type)].m_ammo;
	int maxAmmo = Table[static_cast<int>(m_type)].m_max_ammo;

	if (m_ammo + ammoToAdd > maxAmmo)
	{
		ammoToAdd = maxAmmo - m_ammo;
	}

	if (ammoToAdd > 0)
	{
		m_ammo += ammoToAdd;
		m_actions = m_actions | kRestock;
	}
	else
	{
		m_actions = m_actions | kUpgradeFailed;
	}
}

bool Tank::IsExploding() const
{
	bool exploding = Entity::IsDestroyed() && !m_explosion.IsFinished();
	return exploding;
}

bool Tank::IsDestroyed() const
{
	return false;
}

bool Tank::HasExplosiveShotsUpgrade() const
{
	return m_explosive_shot_countdown.asSeconds() > 0;
}

bool Tank::HasFireRateUpgrade() const
{
	return m_fire_rate_countdown.asSeconds() > 0;
}

sf::FloatRect Tank::GetBoundingRect() const
{
	return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}

void Tank::ResetToLastPos()
{
	setPosition(m_last_pos);
}

void Tank::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
	if (IsExploding())
	{
		UpdateExplosion(dt, commands);
	}
	else
	{
		UpdateTank(dt, commands);
	}
}

void Tank::UpdateTank(sf::Time dt, CommandQueue& commands)
{
	m_last_pos = getPosition();
	Entity::UpdateCurrent(dt, commands);
	if (m_actions & kFiring)
	{
		if (m_fire_cooldown.asSeconds() <= 0.f && m_ammo > 0)
		{
			if (m_explosive_shot_countdown.asSeconds() > 0)
			{
				PlaySound(commands, IsLocalTank()
					                    ? SoundEffect::kPlayer1FireMissile
					                    : SoundEffect::kPlayer2FireMissile);
				commands.Push(m_explosive_fire_command);
			}
			else
			{
				PlaySound(commands, IsLocalTank() ? SoundEffect::kPlayer1Fire : SoundEffect::kPlayer2Fire);
				commands.Push(m_fire_command);
			}

			m_ammo--;

			if (HasFireRateUpgrade())
			{
				m_fire_cooldown = sf::seconds(m_fire_interval.asSeconds() / 2.f);
			}
			else m_fire_cooldown = m_fire_interval;
		}
	}
	if (m_actions & kHit)
	{
		PlaySound(commands, IsLocalTank() ? SoundEffect::kPlayer1Hit : SoundEffect::kPlayer2Hit);
	}
	if (m_actions & kRestock)
	{
		PlaySound(commands, SoundEffect::kRestock);
	}
	if (m_actions & TankActions::kRepair)
	{
		PlaySound(commands, SoundEffect::kRepair);
	}
	if (m_actions & TankActions::kExplosiveUpgrade)
	{
		PlaySound(commands, SoundEffect::kMissileUpgrade);
	}
	if (m_actions & TankActions::kFireRateUpgrade)
	{
		PlaySound(commands, SoundEffect::kFireRateUpgrade);
	}
	if (m_actions & TankActions::kUpgradeFailed)
	{
		PlaySound(commands, SoundEffect::kHealthFull);
	}

	m_actions = 0;

	if (m_fire_cooldown.asSeconds() > 0.f) m_fire_cooldown -= dt;
	if (HasExplosiveShotsUpgrade()) m_explosive_shot_countdown -= dt;
	if (HasFireRateUpgrade()) m_fire_rate_countdown -= dt;

	if (m_has_listener)
	{
		sf::Listener::setPosition(sf::Vector3f(m_last_pos.x, 0, m_last_pos.y));
	}
}

void Tank::UpdateExplosion(sf::Time dt, CommandQueue& commands)
{
	if (!m_destroy_sound_played)
	{
		PlaySound(commands, SoundEffect::kTankExplosion1);
		m_destroy_sound_played = true;
	}

	m_explosion.Update(dt);

	if (m_explosion.IsFinished())
	{
		m_explosion.Restart();
		if(!m_is_ghost)
		{
			TurnToGhost();
		}
		Repair(100);
		m_ammo = 10;
		setPosition(m_spawn_pos);
		setScale(5, 5);
		OnFinishExploding();
	}
}

void Tank::PlaySound(CommandQueue& commands, SoundEffect effect, bool global)
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
