//Alex Nogueira / D00242564 
#include "Pickup.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include "DataTables.hpp"
#include "ResourceHolder.hpp"
#include "Utility.hpp"

namespace
{
	const std::vector<PickupData> Table = InitializePickupData();
}

Pickup::Pickup(World* world, PickupType type, const TextureHolder& textures)
	: Entity(world, 1)
	, m_type(type)
	, m_sprite(textures.Get(Table[static_cast<int>(type)].m_texture))
{
	Utility::CentreOrigin(m_sprite);
	is_static = true;
}

unsigned Pickup::GetCategory() const
{
	return Category::Type::kPickup;
}

sf::FloatRect Pickup::GetBoundingRect() const
{
	return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}

void Pickup::Apply(Tank& player) const
{
	Table[static_cast<int>(m_type)].m_action(player);
}

void Pickup::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_sprite, states);
}
