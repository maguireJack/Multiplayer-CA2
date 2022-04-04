//Alex Nogueira / D00242564
#pragma once
#include "SceneNode.hpp"
#include "ResourceIdentifiers.hpp"


class SoundPlayer;

class SoundNode : public SceneNode
{
public:
	explicit SoundNode(World* world, SoundPlayer& player);
	void PlaySound(SoundEffect sound);
	void PlaySound(SoundEffect sound, sf::Vector2f position);

	virtual unsigned int GetCategory() const override;


private:
	SoundPlayer& m_sounds;
};

