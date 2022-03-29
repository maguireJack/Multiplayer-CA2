//Alex Nogueira / D00242564 
#include "DataTables.hpp"

#include "ParticleType.hpp"
#include "PickupType.hpp"
#include "ProjectileType.hpp"
#include "Tank.hpp"
#include "TankType.hpp"

std::vector<TankData> InitializeTankData()
{
	std::vector<TankData> data(static_cast<int>(TankType::kTankTypeCount));

	data[static_cast<int>(TankType::kLocalTank)].m_hitpoints = 100;
	data[static_cast<int>(TankType::kLocalTank)].m_max_hitpoints = 200;
	data[static_cast<int>(TankType::kLocalTank)].m_ammo = 10;
	data[static_cast<int>(TankType::kLocalTank)].m_max_ammo = 20;
	data[static_cast<int>(TankType::kLocalTank)].m_speed = 200.f;
	data[static_cast<int>(TankType::kLocalTank)].m_fire_interval = sf::seconds(1);
	data[static_cast<int>(TankType::kLocalTank)].m_texture = Textures::kPlayer1Tank;

	data[static_cast<int>(TankType::kEnemyTank)].m_hitpoints = 100;
	data[static_cast<int>(TankType::kEnemyTank)].m_max_hitpoints = 200;
	data[static_cast<int>(TankType::kEnemyTank)].m_ammo = 10;
	data[static_cast<int>(TankType::kEnemyTank)].m_max_ammo = 20;
	data[static_cast<int>(TankType::kEnemyTank)].m_speed = 200.f;
	data[static_cast<int>(TankType::kEnemyTank)].m_fire_interval = sf::seconds(1);
	data[static_cast<int>(TankType::kEnemyTank)].m_texture = Textures::kPlayer2Tank;

	return data;
}

std::vector<ProjectileData> InitializeProjectileData()
{
	std::vector<ProjectileData> data(static_cast<int>(ProjectileType::kProjectileCount));

	data[static_cast<int>(ProjectileType::kPlayerBullet)].m_damage = 20;
	data[static_cast<int>(ProjectileType::kPlayerBullet)].m_speed = 400;
	data[static_cast<int>(ProjectileType::kPlayerBullet)].m_texture = Textures::kBullet;

	data[static_cast<int>(ProjectileType::kPlayerMissile)].m_damage = 40;
	data[static_cast<int>(ProjectileType::kPlayerMissile)].m_speed = 300.f;
	data[static_cast<int>(ProjectileType::kPlayerMissile)].m_texture = Textures::kMissile;

	return data;
}


std::vector<PickupData> InitializePickupData()
{
	std::vector<PickupData> data(static_cast<int>(PickupType::kPickupCount));

	data[static_cast<int>(PickupType::kHealthRefill)].m_texture = Textures::kHealthRefill;
	data[static_cast<int>(PickupType::kHealthRefill)].m_action = [](Tank& t) { t.Repair(50); };
	
	data[static_cast<int>(PickupType::kAmmoRefill)].m_texture = Textures::kAmmoRefill;
	data[static_cast<int>(PickupType::kAmmoRefill)].m_action = [](Tank& t) { t.ReplenishAmmo(); };

	data[static_cast<int>(PickupType::kExplosiveShots)].m_texture = Textures::kExplosiveShots;
	data[static_cast<int>(PickupType::kExplosiveShots)].m_action = [](Tank& t) { t.GetExplosiveShots(); };

	data[static_cast<int>(PickupType::kFireRate)].m_texture = Textures::kFireRate;
	data[static_cast<int>(PickupType::kFireRate)].m_action = [](Tank& t) { t.GetIncreasedFireRate(); };

	return data;
}

std::vector<ParticleData> InitializeParticleData()
{
	std::vector<ParticleData> data(static_cast<int>(ParticleType::kParticleCount));

	data[static_cast<int>(ParticleType::kPropellant)].m_color = sf::Color(255, 255, 50);
	data[static_cast<int>(ParticleType::kPropellant)].m_lifetime = sf::seconds(0.6f);

	data[static_cast<int>(ParticleType::kSmoke)].m_color = sf::Color(50, 50, 50);
	data[static_cast<int>(ParticleType::kSmoke)].m_lifetime = sf::seconds(4.f);

	return data;
}



