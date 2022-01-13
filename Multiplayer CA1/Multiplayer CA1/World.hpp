#pragma once
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "SceneNode.hpp"
#include "SpriteNode.hpp"
#include "Aircraft.hpp"
#include "Layers.hpp"
#include "AircraftType.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <array>
#include <SFML/Graphics/RenderTexture.hpp>

#include "BloomEffect.hpp"
#include "CommandQueue.hpp"
#include "Map.hpp"
#include "ShakeEffect.hpp"
#include "SoundPlayer.hpp"
#include "Tank.hpp"

//Foward
namespace sf
{
	class RenderWindow;
}


class World : private sf::NonCopyable
{
public:
	explicit World(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds);
	void Update(sf::Time dt);
	void Draw();
	CommandQueue& getCommandQueue();

	const Tank* const  GetPlayer1() const;
	const Tank* const GetPlayer2() const;

	bool IsGameOver() const;
	Category::Type GetWinner() const;
	bool AllowPlayerInput();

private:
	void LoadTextures();
	void BuildScene();
	void AdaptPlayerVelocity(Tank* player);

	sf::FloatRect GetViewBounds() const;
	sf::FloatRect GetBattlefieldBounds() const;
	void HandleCollisions();
	void UpdateSounds();

private:
	struct SpawnPoint
	{
		SpawnPoint(AircraftType type, float x, float y) : m_type(type), m_x(x), m_y(y)
		{
			
		}
		AircraftType m_type;
		float m_x;
		float m_y;
	};
	

private:
	sf::RenderTarget& m_target;
	sf::RenderTexture m_scene_texture;
	sf::View m_camera;
	TextureHolder m_textures;
	FontHolder& m_fonts;
	SceneNode m_scenegraph;
	std::array<SceneNode*, static_cast<int>(Layers::kLayerCount)> m_scene_layers;
	CommandQueue m_command_queue;

	SoundPlayer& m_sounds;
	Map* map;

	sf::FloatRect m_world_bounds;
	sf::Vector2f m_spawn_offset;
	sf::Vector2f m_world_center;
	float m_scrollspeed;
	Tank* m_player_1_tank;
	Tank* m_player_2_tank;
	bool m_game_over;
	Category::Type m_winner;

	sf::Time m_total_time;
	sf::Time m_shake_timer;
	sf::Time m_max_shake_timer;
	float m_max_shake_intensity;

	BloomEffect m_bloom_effect;
	ShakeEffect m_shake_effect;
};

