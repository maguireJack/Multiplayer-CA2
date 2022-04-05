//Alex Nogueira / D00242564
#pragma once
#include "CommandQueue.hpp"
#include "SceneNode.hpp"
#include "SoundEffect.hpp"

class Entity : public SceneNode
{
public:
	Entity(World* world, int hitpoints, bool is_static = true);
	void SetVelocity(sf::Vector2f velocity);
	void SetVelocity(float vx, float vy);
	void Accelerate(sf::Vector2f velocity);
	void Accelerate(float vx, float vy);
	virtual void SetHitpoints(int points);
	sf::Vector2f GetVelocity() const;

	int GetHitPoints() const;
	void Repair(unsigned int points);
	virtual void Damage(unsigned int points);
	virtual void Destroy();
	virtual bool IsDestroyed() const override;

protected:
	virtual void UpdateCurrent(sf::Time dt, CommandQueue& commands);

private:
	sf::Vector2f m_velocity;
	int m_hitpoints;
};
