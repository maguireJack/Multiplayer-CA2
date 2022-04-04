//Alex Nogueira / D00242564 
#include "SpawnerManager.hpp"
#include "ResourceHolder.hpp"
#include <stdlib.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

SpawnerManager::SpawnerManager(const TextureHolder& textures, sf::Time interval, std::vector<sf::Vector2f> spawn_positions, float base_chance, bool networked)
	: m_interval(interval), m_chance(base_chance), m_cooldown(interval), m_networked(networked)
{
	srand(time(NULL));
	auto m_spawn_health = [&textures, networked](PickupSpawner* pickup_spawner)
	{
		if (networked) pickup_spawner->SpawnPickupNetwork(PickupType::kHealthRefill, textures);
		else pickup_spawner->SpawnPickup(PickupType::kHealthRefill, textures);
	};

	auto m_spawn_ammo = [&textures, networked](PickupSpawner* pickup_spawner)
	{
		if (networked) pickup_spawner->SpawnPickupNetwork(PickupType::kAmmoRefill, textures);
		else pickup_spawner->SpawnPickup(PickupType::kAmmoRefill, textures);
	};

	auto m_spawn_explosive_shots = [&textures, networked](PickupSpawner* pickup_spawner)
	{
		if (networked) pickup_spawner->SpawnPickupNetwork(PickupType::kExplosiveShots, textures);
		else pickup_spawner->SpawnPickup(PickupType::kExplosiveShots, textures);
	};

	auto m_spawn_fire_rate = [&textures, networked](PickupSpawner* pickup_spawner)
	{
		if (networked) pickup_spawner->SpawnPickupNetwork(PickupType::kFireRate, textures);
		else pickup_spawner->SpawnPickup(PickupType::kFireRate, textures);
	};

	m_spawn_actions.emplace_back(m_spawn_health);
	m_spawn_actions.emplace_back(m_spawn_ammo);
	m_spawn_actions.emplace_back(m_spawn_explosive_shots);
	m_spawn_actions.emplace_back(m_spawn_fire_rate);

	SetupSpawners(spawn_positions);
}

void SpawnerManager::SetupSpawners(std::vector<sf::Vector2f> spawn_positions)
{
	for(auto pos : spawn_positions)
	{
		std::unique_ptr<PickupSpawner> spawner(new PickupSpawner());
		spawner->setPosition(pos);
		AttachChild(std::move(spawner));
	}
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
