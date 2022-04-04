//Alex Nogueira / D00242564 
#include "SoundNode.hpp"

#include "SoundNode.hpp"
#include "SoundPlayer.hpp"


SoundNode::SoundNode(World* world, SoundPlayer& player)
	: SceneNode(world), m_sounds(player)
{
}

void SoundNode::PlaySound(SoundEffect sound)
{
	m_sounds.Play(sound);
}

void SoundNode::PlaySound(SoundEffect sound, sf::Vector2f position)
{
	m_sounds.Play(sound, position);
}

unsigned int SoundNode::GetCategory() const
{
	return Category::kSoundEffect;
}