#include "Tank.hpp"

#include <iostream>
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

Tank::Tank(TankType type, const TextureHolder& textures)
	: Entity(Table[static_cast<int>(type)].m_hitpoints)
	  , m_sprite(textures.Get(Table[static_cast<int>(type)].m_texture))
	  , m_type(type)
	  , m_fire_interval(Table[static_cast<int>(type)].m_fire_interval)
	  , m_ammo(Table[static_cast<int>(type)].m_ammo)
	  , m_explosion(textures.Get(Textures::kExplosion))
{
	//Setup Animation
	m_explosion.SetFrameSize(sf::Vector2i(256, 256));
	m_explosion.SetNumFrames(16);
	m_explosion.SetDuration(sf::seconds(1));
	Utility::CentreOrigin(m_explosion);

	//Setup Sprite
	setScale(5, 5);
	Utility::CentreOrigin(m_sprite);
	if (type == TankType::kPlayer1Tank) FaceDirection(Utility::Down);
	else FaceDirection(Utility::Up);

	//Setup commands
	m_fire_command.action = [this, &textures](SceneNode& node, sf::Time time)
	{
		CreateProjectile(node, GetCategory() == Category::kPlayer1Tank
			                       ? ProjectileType::kPlayer1Bullet
			                       : ProjectileType::kPlayer2Bullet, textures);
	};
	m_fire_command.category = static_cast<int>(Category::Type::kScene);
}

bool Tank::IsPlayer1Tank() const
{
	return m_type == TankType::kPlayer1Tank;
}

unsigned Tank::GetCategory() const
{
	return IsPlayer1Tank() ? Category::kPlayer1Tank : Category::kPlayer2Tank;
}

void Tank::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (IsDestroyed()) return;

	if (IsExploding())
	{
		target.draw(m_explosion, states);
	}
	else
	{
		target.draw(m_sprite, states);
	}
}

//Tank will not keep firing when held down
void Tank::Fire()
{
	if (m_ammo > 0)
	{
		m_actions = m_actions | Firing;
	}
}

void Tank::CreateProjectile(SceneNode& node, ProjectileType type, const TextureHolder& textures) const
{
	std::unique_ptr<Projectile> projectile(new Projectile(type, textures));
	sf::Vector2f offset = GetWorldTransform().transformPoint(5.f, -1.f);
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
	Entity::Repair(health);
	m_actions = m_actions | TankActions::Repair;
}

void Tank::Damage(unsigned points)
{
	Entity::Damage(points);
	m_actions = m_actions | Hit;
	if(IsExploding())
		setScale(1, 1);
}

void Tank::Destroy()
{
	Entity::Destroy();
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
	m_ammo = Table[static_cast<int>(m_type)].m_ammo;
	m_actions = m_actions | Restock;
}

bool Tank::IsExploding() const
{
	return Entity::IsDestroyed() && !m_explosion.IsFinished();
}

bool Tank::IsDestroyed() const
{
	return Entity::IsDestroyed() && m_explosion.IsFinished();
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
	if(IsExploding())
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
	if (m_actions & Firing)
	{
		if (m_fire_cooldown.asSeconds() <= 0.f && m_ammo > 0)
		{
			PlaySound(commands, IsPlayer1Tank() ? SoundEffect::kPlayer1Fire : SoundEffect::kPlayer2Fire);
			commands.Push(m_fire_command);
			m_ammo--;
			m_fire_cooldown = m_fire_interval;
		}
	}
	if (m_actions & Hit)
	{
		PlaySound(commands, IsPlayer1Tank() ? SoundEffect::kPlayer1Hit : SoundEffect::kPlayer2Hit);
	}
	if (m_actions & Restock)
	{
		PlaySound(commands, SoundEffect::kRestock);
	}
	if (m_actions & TankActions::Repair)
	{
		PlaySound(commands, SoundEffect::kRepair);
	}

	m_actions = 0;

	if (m_fire_cooldown.asSeconds() > 0.f) m_fire_cooldown -= dt;
}

void Tank::UpdateExplosion(sf::Time dt, CommandQueue& commands)
{
	if (!m_destroy_sound_played)
	{
		PlaySound(commands, SoundEffect::kExplosion1);
		m_destroy_sound_played = true;
	}

	m_explosion.Update(dt);

	if(m_explosion.IsFinished())
	{
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
