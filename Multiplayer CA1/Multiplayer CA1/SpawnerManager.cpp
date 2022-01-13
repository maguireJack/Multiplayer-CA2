#include "SpawnerManager.hpp"
#include "ResourceHolder.hpp"
#include <stdlib.h>
#include <cstdlib>
#include <ctime>

SpawnerManager::SpawnerManager(const TextureHolder& textures, sf::Time interval, float base_chance)
	: m_interval(interval), m_chance(base_chance), m_cooldown(interval)
{
	srand(time(NULL));
	auto m_spawn_health = [&textures](PickupSpawner* pickup_spawner)
	{
		pickup_spawner->SpawnPickup(PickupType::kHealthRefill, textures);
	};

	auto m_spawn_ammo = [&textures](PickupSpawner* pickup_spawner)
	{
		pickup_spawner->SpawnPickup(PickupType::kAmmoRefill, textures);
	};

	auto m_spawn_explosive_shots = [&textures](PickupSpawner* pickup_spawner)
	{
		pickup_spawner->SpawnPickup(PickupType::kExplosiveShots, textures);
	};

	auto m_spawn_fire_rate = [&textures](PickupSpawner* pickup_spawner)
	{
		pickup_spawner->SpawnPickup(PickupType::kFireRate, textures);
	};

	m_spawn_actions.emplace_back(m_spawn_health);
	m_spawn_actions.emplace_back(m_spawn_ammo);
	m_spawn_actions.emplace_back(m_spawn_explosive_shots);
	m_spawn_actions.emplace_back(m_spawn_fire_rate);

	SetupSpawners();
}

void SpawnerManager::SetupSpawners()
{
	std::unique_ptr<PickupSpawner> spawner1(new PickupSpawner());
	spawner1->setPosition(0, 0);
	std::unique_ptr<PickupSpawner> spawner2(new PickupSpawner());
	spawner2->setPosition(-365, 0);
	std::unique_ptr<PickupSpawner> spawner3(new PickupSpawner());
	spawner3->setPosition(365, 0);

	AttachChild(std::move(spawner1));
	AttachChild(std::move(spawner2));
	AttachChild(std::move(spawner3));
}

void SpawnerManager::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
	m_cooldown -= dt;
	if (m_cooldown.asSeconds() <= 0.f)
	{
		int rand = std::rand();

		if (rand < RAND_MAX * m_chance)
		{
			int spawner_pos = rand % ChildCount();
			int action_pos = rand % m_spawn_actions.size();
			auto child = static_cast<PickupSpawner*>(GetChild(spawner_pos));
			if (!child->HasPickup())
			{
				m_spawn_actions[action_pos](child);
			}
		}

		m_cooldown = m_interval;
	}
}
