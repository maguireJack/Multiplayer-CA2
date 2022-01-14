#pragma once
#include <functional>
#include <vector>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Time.hpp>

#include "ResourceIdentifiers.hpp"

class Aircraft;
class Tank;

struct Direction
{
	Direction(float angle, float distance)
		: m_angle(angle), m_distance(distance)
	{
	}
	float m_angle;
	float m_distance;
};

struct AircraftData
{
	int m_hitpoints;
	float m_speed;
	Textures m_texture;
	sf::Time m_fire_interval;
	std::vector<Direction> m_directions;
};

struct TankData
{
	int m_hitpoints;
	int m_max_hitpoints;
	float m_speed;
	Textures m_texture;
	sf::Time m_fire_interval;
	int m_ammo;
	int m_max_ammo;
};

struct ProjectileData
{
	int m_damage;
	float m_speed;
	Textures m_texture;
};

struct PickupData
{
	std::function<void(Tank&)> m_action;
	Textures m_texture;
};

struct ParticleData
{
	sf::Color						m_color;
	sf::Time						m_lifetime;
};

std::vector<TankData> InitializeTankData();
std::vector<ProjectileData> InitializeProjectileData();
std::vector<PickupData> InitializePickupData();
std::vector<ParticleData> InitializeParticleData();