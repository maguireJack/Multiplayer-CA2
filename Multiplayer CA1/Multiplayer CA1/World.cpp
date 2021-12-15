#include "World.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>

#include "Pickup.hpp"
#include "Projectile.hpp"
#include "SpawnerManager.hpp"
#include "Tank.hpp"

World::World(sf::RenderWindow& window, FontHolder& font)
	: m_window(window)
	, m_camera(window.getDefaultView())
	, m_textures()
	, m_fonts(font)
	, m_scenegraph()
	, m_scene_layers()
	, m_world_bounds(0.f, 0.f, 900, 600)
	, m_world_center(m_world_bounds.width / 2.f, m_world_bounds.height / 2.f)
	, m_spawn_offset(370, -175)
	, m_scrollspeed(-50.f)
	, m_player_1_tank(nullptr)
	, m_player_2_tank(nullptr)
	, m_game_over(false)
	, m_winner(Category::kNone)
{
	LoadTextures();
	BuildScene();
	std::cout << m_camera.getSize().x << m_camera.getSize().y << std::endl;
	m_camera.setCenter(450,400);
}

void World::Update(sf::Time dt)
{
	m_player_1_tank->SetVelocity(0, 0);
	m_player_2_tank->SetVelocity(0, 0);

	//Forward commands to the scenegraph until the command queue is empty
	while(!m_command_queue.IsEmpty())
	{
		m_scenegraph.OnCommand(m_command_queue.Pop(), dt);
	}

	AdaptPlayerVelocity(m_player_1_tank);
	AdaptPlayerVelocity(m_player_2_tank);

	HandleCollisions();

	m_scenegraph.RemoveWrecks();

	//Apply movement
	m_scenegraph.Update(dt, m_command_queue);
}

void World::Draw()
{
	m_window.setView(m_camera);
	m_window.draw(m_scenegraph);
}

void World::LoadTextures()
{
	m_textures.Load(Textures::kPlayer1Tank, "Media/Textures/Tanx.png", sf::IntRect(1,12,10,10));
	m_textures.Load(Textures::kPlayer2Tank, "Media/Textures/Tanx.png", sf::IntRect(1,23,10,10));
	m_textures.Load(Textures::kBackground, "Media/Textures/Tanx.png", sf::IntRect(88, 44, 10, 10));

	m_textures.Load(Textures::kHealthRefill, "Media/Textures/HealthRefill.png");
	m_textures.Load(Textures::kAmmoRefill, "Media/Textures/FireRate.png");

	m_textures.Load(Textures::kBullet, "Media/Textures/Bullet.png");
	m_textures.Load(Textures::kMissile, "Media/Textures/Missile.png");
}

void World::BuildScene()
{
	//Initialize the different layers
	for (std::size_t i = 0; i < static_cast<int>(Layers::kLayerCount); ++i)
	{
		Category::Type category = (i == static_cast<int>(Layers::kBattlefield)) ? Category::Type::kScene : Category::Type::kNone;
		SceneNode::Ptr layer(new SceneNode(false, category));
		m_scene_layers[i] = layer.get();
		m_scenegraph.AttachChild(std::move(layer));
	}

	//Prepare the background
	sf::Texture& texture = m_textures.Get(Textures::kBackground);
	sf::IntRect textureRect(m_world_bounds);
	//Tile the texture to cover our world
	texture.setRepeated(true);

	//Add the background sprite to our scene
	std::unique_ptr<SpriteNode> background_sprite(new SpriteNode(texture, textureRect));
	background_sprite->setPosition(m_world_bounds.left, m_world_bounds.top);
	m_scene_layers[static_cast<int>(Layers::kBackground)]->AttachChild(std::move(background_sprite));

	//Load the map
	std::unique_ptr<Map> m(new Map("Media/Arena Data/map", "Media/Textures/Tanx.png", 16));
	m_scene_layers[static_cast<int>(Layers::kBattlefield)]->AttachChild(std::move(m));

	//Add player 1 tank
	std::unique_ptr<Tank> p1tank(new Tank(TankType::kPlayer1Tank, m_textures));
	m_player_1_tank = p1tank.get();
	m_player_1_tank->setPosition(m_world_center - m_spawn_offset);
	m_scene_layers[static_cast<int>(Layers::kBattlefield)]->AttachChild(std::move(p1tank));

	//Add plyaer 2 tank
	std::unique_ptr<Tank> p2tank(new Tank(TankType::kPlayer2Tank, m_textures));
	m_player_2_tank = p2tank.get();
	m_player_2_tank->setPosition(m_world_center + m_spawn_offset);
	m_scene_layers[static_cast<int>(Layers::kBattlefield)]->AttachChild(std::move(p2tank));

	std::unique_ptr<SpawnerManager> spawner_manager(new SpawnerManager(m_textures, sf::seconds(1), 0.1f));
	spawner_manager->setPosition(m_world_center);
	m_scene_layers[static_cast<int>(Layers::kBattlefield)]->AttachChild(std::move(spawner_manager));
}

CommandQueue& World::getCommandQueue()
{
	return m_command_queue;
}

const Tank* const World::GetPlayer1() const
{
	return m_player_1_tank;
}

const Tank* const World::GetPlayer2() const
{
	return m_player_2_tank;
}

bool World::IsGameOver() const
{
	return m_game_over;
}

Category::Type World::GetWinner() const
{
	return m_winner;
}

void World::AdaptPlayerVelocity(Tank* player)
{
	sf::Vector2f velocity = player->GetVelocity();
	//if moving diagonally then reduce velocity
	if (velocity.x != 0.f && velocity.y != 0.f)
	{
		player->SetVelocity(velocity / std::sqrt(2.f));
	}
}

sf::FloatRect World::GetViewBounds() const
{
	return sf::FloatRect(m_camera.getCenter() - m_camera.getSize() / 2.f, m_camera.getSize());
}

sf::FloatRect World::GetBattlefieldBounds() const
{
	//Return camera bounds + a small area at the top where enemies spawn offscreen
	sf::FloatRect bounds = GetViewBounds();
	bounds.top -= 100.f;
	bounds.height += 100.f;

	return bounds;
}

bool MatchesCategories(SceneNode::Pair& colliders, Category::Type type1, Category::Type type2)
{
	unsigned int category1 = colliders.first->GetCategory();
	unsigned int category2 = colliders.second->GetCategory();

	if(type1 & category1 && type2 & category2)
	{
		return true;
	}
	else if(type1 & category2 && type2 & category1)
	{
		std::swap(colliders.first, colliders.second);
		return true;
	}
	else
	{
		return false;
	}
}

void World::HandleCollisions()
{
	std::set<SceneNode::Pair> collision_pairs;
	m_scenegraph.CheckSceneCollision(m_scenegraph, collision_pairs);
	for(SceneNode::Pair pair : collision_pairs)
	{
		if (MatchesCategories(pair, Category::Type::kPlayerAircraft, Category::Type::kPickup))
		{
			auto& player = static_cast<Tank&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);
			//Apply the pickup effect
			pickup.Apply(player);
			pickup.Destroy();
		}

		else if (MatchesCategories(pair, Category::Type::kPlayer1Tank, Category::Type::kPlayer2Projectile) || MatchesCategories(pair, Category::Type::kPlayer2Tank, Category::Type::kPlayer1Projectile))
		{
			auto& tank = static_cast<Tank&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);
			//Apply the projectile damage to the plane
			tank.Damage(projectile.GetDamage());
			projectile.Destroy();
			if (tank.IsDestroyed()) {
				m_game_over = true;
				m_winner = static_cast<Category::Type>(m_winner | (tank.GetCategory() == Category::Type::kPlayer1Tank ? Category::Type::kPlayer2Tank : Category::Type::kPlayer1Tank));
			}
		}

		else if (MatchesCategories(pair, Category::Type::kPlayer1Projectile, Category::Type::kTile) || MatchesCategories(pair, Category::Type::kPlayer2Projectile, Category::Type::kTile))
		{
			auto& projectile = static_cast<Projectile&>(*pair.first);
			projectile.Destroy();
		}

		else if (MatchesCategories(pair, Category::Type::kPlayer1Tank, Category::Type::kTile) || MatchesCategories(pair, Category::Type::kPlayer2Tank, Category::Type::kTile))
		{
			auto& tank = static_cast<Tank&>(*pair.first);
			tank.ResetToLastPos();
		}

	}
}
