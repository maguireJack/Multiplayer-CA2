#include "PickupSpawner.hpp"

PickupSpawner::PickupSpawner()
	: m_life(sf::seconds(10))
{
}

void PickupSpawner::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
	if (HasPickup()) {
		m_current_life -= dt;
		if (m_current_life.asSeconds() <= 0.f)
		{
			static_cast<Pickup*>(GetChild(0))->Destroy();
		}
	}
}

void PickupSpawner::SpawnPickup(PickupType type, const TextureHolder& textures)
{
	m_current_life = m_life;
	auto p = std::make_unique<Pickup>(type, textures);
	AttachChild(std::move(p));
}

bool PickupSpawner::HasPickup() const
{
	return ChildCount() > 0;
}
