#include "Tank.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include "DataTables.hpp"
#include "ResourceHolder.hpp"

namespace
{
	const std::vector<TankData> Table = InitializeTankData();
}

Tank::Tank(TankType type, const TextureHolder& textures)
: Entity(Table[static_cast<int>(type)].m_hitpoints)
, m_sprite(textures.Get(Table[static_cast<int>(type)].m_texture))
, m_type(type)
{
	setScale(5, 5);
	Utility::CentreOrigin(m_sprite);
	if (type == TankType::kPlayer1Tank) FaceDirection(Utility::Down);
	else FaceDirection(Utility::Up);
}

unsigned Tank::GetCategory() const
{
	return m_type == TankType::kPlayer1Tank ? Category::kPlayer1Tank : Category::kPlayer2Tank;
}

void Tank::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_sprite, states);
}

void Tank::Fire()
{
}

float Tank::GetMaxSpeed()
{
	return Table[static_cast<int>(m_type)].m_speed;
}

void Tank::Repair(int health)
{
	Entity::Repair(health);
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
