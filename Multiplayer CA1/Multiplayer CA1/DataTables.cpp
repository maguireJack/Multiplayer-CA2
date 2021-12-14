#include "DataTables.hpp"
#include "PickupType.hpp"
#include "ProjectileType.hpp"
#include "Tank.hpp"
#include "TankType.hpp"

std::vector<TankData> InitializeTankData()
{
	std::vector<TankData> data(static_cast<int>(TankType::kTankTypeCount));

	data[static_cast<int>(TankType::kPlayer1Tank)].m_hitpoints = 100;
	data[static_cast<int>(TankType::kPlayer1Tank)].m_speed = 200.f;
	data[static_cast<int>(TankType::kPlayer1Tank)].m_fire_interval = sf::seconds(1);
	data[static_cast<int>(TankType::kPlayer1Tank)].m_texture = Textures::kPlayer1Tank;

	data[static_cast<int>(TankType::kPlayer2Tank)].m_hitpoints = 100;
	data[static_cast<int>(TankType::kPlayer2Tank)].m_speed = 200.f;
	data[static_cast<int>(TankType::kPlayer2Tank)].m_fire_interval = sf::seconds(1);
	data[static_cast<int>(TankType::kPlayer2Tank)].m_texture = Textures::kPlayer2Tank;

	return data;
}

std::vector<ProjectileData> InitializeProjectileData()
{
	std::vector<ProjectileData> data(static_cast<int>(ProjectileType::kProjectileCount));

	data[static_cast<int>(ProjectileType::kPlayer1Bullet)].m_damage = 10;
	data[static_cast<int>(ProjectileType::kPlayer1Bullet)].m_speed = 300;
	data[static_cast<int>(ProjectileType::kPlayer1Bullet)].m_texture = Textures::kBullet;

	data[static_cast<int>(ProjectileType::kPlayer2Bullet)].m_damage = 10;
	data[static_cast<int>(ProjectileType::kPlayer2Bullet)].m_speed = 300;
	data[static_cast<int>(ProjectileType::kPlayer2Bullet)].m_texture = Textures::kBullet;

	data[static_cast<int>(ProjectileType::kMissile)].m_damage = 200;
	data[static_cast<int>(ProjectileType::kMissile)].m_speed = 150.f;
	data[static_cast<int>(ProjectileType::kMissile)].m_texture = Textures::kMissile;
	return data;
}


std::vector<PickupData> InitializePickupData()
{
	std::vector<PickupData> data(static_cast<int>(PickupType::kPickupCount));

	data[static_cast<int>(PickupType::kHealthRefill)].m_texture = Textures::kHealthRefill;
	data[static_cast<int>(PickupType::kHealthRefill)].m_action = [](Tank& t) {t.Repair(25); };
	/*
	data[static_cast<int>(PickupType::kMissileRefill)].m_texture = Textures::kMissileRefill;
	data[static_cast<int>(PickupType::kMissileRefill)].m_action = std::bind(&Aircraft::CollectMissiles, std::placeholders::_1, 3);

	data[static_cast<int>(PickupType::kFireSpread)].m_texture = Textures::kFireSpread;
	data[static_cast<int>(PickupType::kFireSpread)].m_action = std::bind(&Aircraft::IncreaseSpread, std::placeholders::_1);

	data[static_cast<int>(PickupType::kFireRate)].m_texture = Textures::kFireRate;
	data[static_cast<int>(PickupType::kFireRate)].m_action = std::bind(&Aircraft::IncreaseFireRate, std::placeholders::_1);
	*/
	return data;
}



