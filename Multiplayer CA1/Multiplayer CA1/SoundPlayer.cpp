//Alex Nogueira / D00242564 
#include "SoundPlayer.hpp"
#include "SoundEffect.hpp"

#include <SFML/Audio/Listener.hpp>

#include <cmath>


namespace
{
	// Sound coordinate system, point of view of a player in front of the screen:
	// X = left; Y = up; Z = back (out of the screen)
	const float ListenerZ = 300.f;
	const float Attenuation = 8.f;
	const float MinDistance2D = 400.f;
	const float MinDistance3D = std::sqrt(MinDistance2D * MinDistance2D + ListenerZ * ListenerZ);
}

SoundPlayer::SoundPlayer()
{
	m_sound_buffers.Load(SoundEffect::kPlayer1Fire, "Media/Sounds/Gunfire.wav");
	m_sound_buffers.Load(SoundEffect::kPlayer2Fire, "Media/Sounds/Gunfire.wav");
	m_sound_buffers.Load(SoundEffect::kPlayer1FireMissile, "Media/Sounds/Gunfire5.wav");
	m_sound_buffers.Load(SoundEffect::kPlayer2FireMissile, "Media/Sounds/Gunfire5.wav");
	m_sound_buffers.Load(SoundEffect::kTankExplosion1, "Media/Sounds/TankExplosion.wav");
	m_sound_buffers.Load(SoundEffect::kTankExplosion2, "Media/Sounds/TankExplosion.wav");
	m_sound_buffers.Load(SoundEffect::kMissileExplosion1, "Media/Sounds/MissileExplosion.wav");
	m_sound_buffers.Load(SoundEffect::kMissileExplosion2, "Media/Sounds/MissileExplosion.wav");
	m_sound_buffers.Load(SoundEffect::kPlayer1Hit, "Media/Sounds/Hit.wav");
	m_sound_buffers.Load(SoundEffect::kPlayer2Hit, "Media/Sounds/Hit.wav");
	m_sound_buffers.Load(SoundEffect::kCollectPickup, "Media/Sounds/CollectPickup.wav");
	m_sound_buffers.Load(SoundEffect::kRepair, "Media/Sounds/Repair.wav");
	m_sound_buffers.Load(SoundEffect::kRestock, "Media/Sounds/Restock.wav");
	m_sound_buffers.Load(SoundEffect::kMissileUpgrade, "Media/Sounds/Missile.wav");
	m_sound_buffers.Load(SoundEffect::kFireRateUpgrade, "Media/Sounds/FireRate.wav");
	m_sound_buffers.Load(SoundEffect::kHealthFull, "Media/Sounds/HealthFull.wav");
	m_sound_buffers.Load(SoundEffect::kButton, "Media/Sounds/Button.wav");

	// Listener points towards the screen (default in SFML)
	sf::Listener::setDirection(0.f, 0.f, -1.f);
}

void SoundPlayer::Play(SoundEffect effect)
{
	Play(effect, GetListenerPosition());
}

void SoundPlayer::Play(SoundEffect effect, sf::Vector2f position)
{
	m_sounds.emplace_back(sf::Sound());
	sf::Sound& sound = m_sounds.back();

	sound.setBuffer(m_sound_buffers.Get(effect));
	sound.setPosition(position.x, -position.y, 0.f);
	sound.setAttenuation(Attenuation);
	sound.setMinDistance(MinDistance3D);

	sound.play();
}

void SoundPlayer::RemoveStoppedSounds()
{
	m_sounds.remove_if([](const sf::Sound& s)
	{
		return s.getStatus() == sf::Sound::Stopped;
	});
}

void SoundPlayer::SetListenerPosition(sf::Vector2f position)
{
	sf::Listener::setPosition(position.x, -position.y, ListenerZ);
}

sf::Vector2f SoundPlayer::GetListenerPosition() const
{
	sf::Vector3f position = sf::Listener::getPosition();
	return sf::Vector2f(position.x, -position.y);
}
