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

#include "CommandQueue.hpp"
#include "Map.hpp"
#include "Tank.hpp"

//Foward
namespace sf
{
	class RenderWindow;
}


class World : private sf::NonCopyable
{
public:
	explicit World(sf::RenderWindow& window, FontHolder& font);
	void Update(sf::Time dt);
	void Draw();
	CommandQueue& getCommandQueue();

	const Tank* const  GetPlayer1() const;
	const Tank* const GetPlayer2() const;

	bool IsGameOver() const;
	Category::Type GetWinner() const;

private:
	void LoadTextures();
	void BuildScene();
	void AdaptPlayerPosition(Tank* player);
	void AdaptPlayerVelocity(Tank* player);

	sf::FloatRect GetViewBounds() const;
	sf::FloatRect GetBattlefieldBounds() const;
	void GuideMissiles();
	void HandleCollisions();

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
	sf::RenderWindow& m_window;
	sf::View m_camera;
	TextureHolder m_textures;
	FontHolder& m_fonts;
	SceneNode m_scenegraph;
	//std::vector<SceneNode> m_collision_entities;
	std::array<SceneNode*, static_cast<int>(Layers::kLayerCount)> m_scene_layers;
	CommandQueue m_command_queue;

	Map* map;

	sf::FloatRect m_world_bounds;
	sf::Vector2f m_spawn_offset;
	sf::Vector2f m_world_center;
	float m_scrollspeed;
	Tank* m_player_1_tank;
	Tank* m_player_2_tank;
	std::vector<SpawnPoint> m_enemy_spawn_points;
	std::vector<Aircraft*>	m_active_enemies;
	bool m_game_over;
	Category::Type m_winner;
};

