//Alex Nogueira / D00242564
#pragma once
//SceneNode category used to dispatch commands
namespace Category
{
	enum Type
	{
		kNone = 0,
		kScene = 1 << 0,
		kPlayer1Tank = 1 << 1,
		kPlayer2Tank = 1 << 2,
		kPlayerAircraft = 1 << 1,
		kAlliedAircraft = 1 << 2,
		kEnemyAircraft = 1 << 3,
		kPickup = 1 << 4,
		kPlayer1Projectile = 1 << 5,
		kPlayer2Projectile = 1 << 6,
		kAlliedProjectile = 1 << 5,
		kEnemyProjectile = 1 << 6,
		kFloorTile = 1 << 7,
		kWallTile = 1 << 8,
		kDestroyableTile = 1 << 9,
		kParticleSystem = 1 << 10,
		kSoundEffect = 1 << 11,

		kTile = kFloorTile | kWallTile | kDestroyableTile,
		kTank = kPlayer1Tank | kPlayer2Tank,
		kAircraft = kPlayerAircraft | kAlliedAircraft | kEnemyAircraft,
		kProjectile = kPlayer1Projectile | kPlayer2Projectile,
	};
}