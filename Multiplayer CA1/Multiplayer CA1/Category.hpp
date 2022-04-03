//Alex Nogueira / D00242564
#pragma once
//SceneNode category used to dispatch commands
namespace Category
{
	enum Type
	{
		kNone = 0,
		kScene = 1 << 0,
		kLocalTank = 1 << 1,
		kEnemyTank = 1 << 2,
		kPickup = 1 << 4,
		kPlayerProjectile = 1 << 5,
		kEnemyProjectile = 1 << 6,
		kFloorTile = 1 << 7,
		kWallTile = 1 << 8,
		kDestroyableTile = 1 << 9,
		kParticleSystem = 1 << 10,
		kSoundEffect = 1 << 11,
		kNetwork = 1 << 12,
		kLocalGhostTank = 1 << 13,
		kEnemyGhostTank = 1 << 14,
		kPlayerGhostProjectile = 1 << 15,
		kEnemyGhostProjectile = 1 << 16,

		kTile = kFloorTile | kWallTile | kDestroyableTile,
		kTank = kLocalTank | kEnemyTank,
		kGhostTank = kLocalGhostTank | kEnemyGhostTank,
		kAllTank = kTank | kGhostTank,
		kProjectile = kPlayerProjectile | kEnemyProjectile,
		kGhostProjectile = kPlayerGhostProjectile | kEnemyGhostProjectile,

		//Aircraft stuff
		kPlayerAircraft = 1 << 1,
		kAlliedAircraft = 1 << 2,
		kEnemyAircraft = 1 << 3,
	};
}