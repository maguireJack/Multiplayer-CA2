//Alex Nogueira / D00242564
#pragma once
#include "PickupSpawner.hpp"
#include "SceneNode.hpp"
#include <SFML/Graphics/Texture.hpp>

class SpawnerManager : public SceneNode
{
public:
	SpawnerManager(const TextureHolder& textures, sf::Time interval, float baseChance, bool networked = false);
	void SetupSpawners();

private:
	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;

private:
	std::vector<std::function<void(PickupSpawner*)>> m_spawn_actions;
	sf::Time m_cooldown;
	sf::Time m_interval;
	float m_chance;
	bool m_networked;
};

