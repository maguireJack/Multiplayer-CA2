//Alex Nogueira / D00242564
#pragma once
#include "Entity.hpp"
#include "Pickup.hpp"
#include "PickupType.hpp"
#include "ResourceHolder.hpp"

class PickupSpawner : public SceneNode
{
public:
	PickupSpawner(World* world);
	void SpawnPickup(PickupType type, const TextureHolder& textures);
	void SpawnPickupNetwork(PickupType type, const TextureHolder& textures);
	bool HasPickup() const;
	Pickup* GetPickup();

protected:
	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;

private:
	sf::Time m_current_life;
	sf::Time m_life;
	std::function<void()> kill_action;
	Pickup* m_pickup;
	bool m_notify_pickup_spawn_on_next_update;
	PickupType m_pickup_type;
};

