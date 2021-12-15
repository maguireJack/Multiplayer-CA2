#pragma once
#include "Entity.hpp"
#include "Pickup.hpp"
#include "PickupType.hpp"

class PickupSpawner : public SceneNode
{
public:
	PickupSpawner();
	void SpawnPickup(PickupType type, const TextureHolder& textures);
	bool HasPickup() const;

protected:
	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;

private:
	sf::Time m_current_life;
	sf::Time m_life;
	std::function<void()> kill_action;
};

