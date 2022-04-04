//Alex Nogueira / D00242564
#pragma once
#include "PickupSpawner.hpp"
#include "SceneNode.hpp"
#include <SFML/Graphics/Texture.hpp>

class SpawnerManager : public SceneNode
{
public:
	SpawnerManager(World* world, const TextureHolder& textures, sf::Time interval, std::vector<sf::Vector2f> spawn_positions, float baseChance, bool networked = false);

private:
	void SetupSpawners(std::vector<sf::Vector2f> spawn_positions);
	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;

private:
	std::vector<std::function<void(PickupSpawner*)>> m_spawn_actions;
	sf::Time m_cooldown;
	sf::Time m_interval;
	float m_chance;
	bool m_networked;
};

