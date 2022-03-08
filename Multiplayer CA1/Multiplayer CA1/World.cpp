//Alex Nogueira / D00242564 
#include "World.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>

#include "Pickup.hpp"
#include "Projectile.hpp"
#include "SoundNode.hpp"
#include "SpawnerManager.hpp"
#include "Tank.hpp"
#include "Tile.hpp"

World::World(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, bool networked)
	: m_target(output_target)
	  , m_camera(m_target.getDefaultView())
	  , m_textures()
	  , m_fonts(font)
	  , m_scenegraph()
	  , m_scene_layers()
	  , m_world_bounds(0.f, 0.f, 900, 600)
	  , m_world_center(m_world_bounds.width / 2.f, m_world_bounds.height / 2.f)
	  , m_spawn_offset(370, -175)
	  , m_scrollspeed(-50.f)
	  , m_player_1_tank(nullptr)
	  , m_game_over(false)
	  , m_winner(Category::kNone)
	  , m_sounds(sounds)
	  , m_max_shake_timer(sf::seconds(0.25))
	  , m_max_shake_intensity(10)
	  , m_networked_world(networked)
	  , m_network_node(nullptr)
	  , m_finish_sprite(nullptr)
{
	m_scene_texture.create(m_target.getSize().x, m_target.getSize().y);
	LoadTextures();
	BuildScene();
	m_camera.setCenter(450, 400);
}

void World::Update(sf::Time dt)
{
	for (Tank* tank : m_player_tanks)
	{
		tank->SetVelocity(0, 0);
	}

	//Forward commands to the scenegraph until the command queue is empty
	while (!m_command_queue.IsEmpty())
	{
		m_scenegraph.OnCommand(m_command_queue.Pop(), dt);
	}

	AdaptPlayerVelocity();

	HandleCollisions();

	m_scenegraph.RemoveWrecks();

	//Apply movement
	m_scenegraph.Update(dt, m_command_queue);

	UpdateSounds();

	if (m_shake_timer.asSeconds() > 0)
		m_shake_timer -= dt;

	m_total_time += dt;
}

Tank* World::GetTank(int identifier) const
{
	for (Tank* a : m_player_tanks)
	{
		if (a->GetIdentifier() == identifier)
		{
			return a;
		}
	}
	return nullptr;
}

bool World::PollGameAction(GameActions::Action& out)
{
	return m_network_node->PollGameAction(out);
}

void World::Draw()
{
	if (PostEffect::IsSupported())
	{
		m_scene_texture.clear();
		m_scene_texture.setView(m_camera);
		m_scene_texture.draw(m_scenegraph);
		if (m_shake_timer.asSeconds() > 0)
		{
			float intensity = m_max_shake_intensity * m_shake_timer.asSeconds() / m_max_shake_timer.asSeconds();
			m_shake_effect.Apply(m_scene_texture, m_target, m_total_time, intensity);
		}
		else
		{
			m_bloom_effect.Apply(m_scene_texture, m_target);
		}
	}
	else
	{
		m_target.setView(m_camera);
		m_target.draw(m_scenegraph);
	}
}

void World::LoadTextures()
{
	m_textures.Load(Textures::kPlayer1Tank, "Media/Textures/Tanx.png", sf::IntRect(1, 12, 10, 10));
	m_textures.Load(Textures::kPlayer2Tank, "Media/Textures/Tanx.png", sf::IntRect(1, 23, 10, 10));
	m_textures.Load(Textures::kBackground, "Media/Textures/Tanx.png", sf::IntRect(88, 44, 10, 10));

	m_textures.Load(Textures::kHealthRefill, "Media/Textures/HealthRefill.png");
	m_textures.Load(Textures::kAmmoRefill, "Media/Textures/FireSpread.png");
	m_textures.Load(Textures::kExplosiveShots, "Media/Textures/MissileRefill.png");
	m_textures.Load(Textures::kFireRate, "Media/Textures/FireRate.png");

	m_textures.Load(Textures::kBullet, "Media/Textures/Bullet.png");
	m_textures.Load(Textures::kMissile, "Media/Textures/Missile.png");

	m_textures.Load(Textures::kExplosion, "Media/Textures/Explosion.png");
}

void World::BuildScene()
{
	//Initialize the different layers
	for (std::size_t i = 0; i < static_cast<int>(Layers::kLayerCount); ++i)
	{
		Category::Type category = (i == static_cast<int>(Layers::kBattlefield))
			                          ? Category::Type::kScene
			                          : Category::Type::kNone;
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

	// Add sound effect node
	std::unique_ptr<SoundNode> soundNode(new SoundNode(m_sounds));
	m_scenegraph.AttachChild(std::move(soundNode));

	/*std::unique_ptr<SpawnerManager> spawner_manager(new SpawnerManager(m_textures, sf::seconds(1), 0.2f));
	spawner_manager->setPosition(m_world_center);
	m_scene_layers[static_cast<int>(Layers::kBattlefield)]->AttachChild(std::move(spawner_manager));*/

	if (m_networked_world)
	{
		std::unique_ptr<NetworkNode> network_node(new NetworkNode());
		m_network_node = network_node.get();
		m_scenegraph.AttachChild(std::move(network_node));
	}
}

void World::SetWorldHeight(float height)
{
	m_world_bounds.height = height;
}

void World::SetCurrentBattleFieldPosition(float lineY)
{
	m_spawn_offset.y = m_world_bounds.height;
}

CommandQueue& World::getCommandQueue()
{
	return m_command_queue;
}

const Tank* const World::GetPlayer1() const
{
	return m_player_1_tank;
}

bool World::IsGameOver() const
{
	return m_game_over;
}

Category::Type World::GetWinner() const
{
	return m_winner;
}

bool World::AllowPlayerInput()
{
	return !m_game_over;
}

void World::CreatePickup(sf::Vector2f position, PickupType type)
{
	std::unique_ptr<Pickup> pickup(new Pickup(type, m_textures));
	pickup->setPosition(position);
	pickup->SetVelocity(0.f, 1.f);
	m_scene_layers[static_cast<int>(Layers::kBattlefield)]->AttachChild(std::move(pickup));
}

void World::AdaptPlayerVelocity()
{
	for (Tank* tank : m_player_tanks)
	{
		sf::Vector2f velocity = tank->GetVelocity();
		//if moving diagonally then reduce velocity
		if (velocity.x != 0.f && velocity.y != 0.f)
		{
			tank->SetVelocity(velocity / std::sqrt(2.f));
		}
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

	if (type1 & category1 && type2 & category2)
	{
		return true;
	}
	else if (type1 & category2 && type2 & category1)
	{
		std::swap(colliders.first, colliders.second);
		return true;
	}
	else
	{
		return false;
	}
}

void World::RemoveTank(int identifier)
{
	Tank* tank = GetTank(identifier);
	if (tank)
	{
		tank->Destroy();
		m_player_tanks.erase(std::find(m_player_tanks.begin(), m_player_tanks.end(), tank));
	}
}

Tank* World::AddTank(int identifier)
{
	std::unique_ptr<Tank> player(new Tank(TankType::kPlayer1Tank, m_textures));
	player->setPosition(sf::Vector2f(150, 150));
	player->SetIdentifier(identifier);

	m_player_tanks.emplace_back(player.get());
	m_scene_layers[static_cast<int>(Layers::kBattlefield)]->AttachChild(std::move(player));
	return m_player_tanks.back();
}


void World::HandleCollisions()
{
	std::set<SceneNode::Pair> collision_pairs;
	m_scenegraph.CheckSceneCollision(m_scenegraph, collision_pairs);
	for (SceneNode::Pair pair : collision_pairs)
	{
		if (MatchesCategories(pair, Category::Type::kTank, Category::Type::kPickup))
		{
			auto& player = static_cast<Tank&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);
			//Apply the pickup effect
			pickup.Apply(player);
			pickup.Destroy();
		}

		else if (MatchesCategories(pair, Category::Type::kPlayer1Tank, Category::Type::kPlayer2Projectile) ||
			MatchesCategories(pair, Category::Type::kPlayer2Tank, Category::Type::kPlayer1Projectile))
		{
			auto& tank = static_cast<Tank&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);
			if (!projectile.CanDamagePlayers())
				continue;

			//Apply the projectile damage to the tank
			tank.Damage(projectile.GetDamage());
			projectile.AppliedPlayerDamage();
			projectile.Destroy();
			m_shake_timer = m_max_shake_timer;
			if (tank.IsExploding() && m_winner != Category::kPlayer1Tank && m_winner != Category::kPlayer2Tank)
			{
				tank.OnFinishExploding = [this] { m_game_over = true; };
				m_winner = static_cast<Category::Type>(m_winner | (tank.GetCategory() == Category::Type::kPlayer1Tank
					                                                   ? Category::Type::kPlayer2Tank
					                                                   : Category::Type::kPlayer1Tank));
			}
		}

		else if (MatchesCategories(pair, Category::Type::kDestroyableTile, Category::Type::kProjectile))
		{
			auto& tile = static_cast<Tile&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);
			tile.Damage(1);
			projectile.Destroy();
		}

		else if (MatchesCategories(pair, Category::Type::kPlayer1Projectile, Category::Type::kTile) ||
			MatchesCategories(pair, Category::Type::kPlayer2Projectile, Category::Type::kTile))
		{
			auto& projectile = static_cast<Projectile&>(*pair.first);
			projectile.Destroy();
		}

		else if (MatchesCategories(pair, Category::Type::kPlayer1Tank, Category::Type::kTile) || MatchesCategories(
			pair, Category::Type::kPlayer2Tank, Category::Type::kTile))
		{
			auto& tank = static_cast<Tank&>(*pair.first);
			tank.ResetToLastPos();
		}
	}
}

//TODO probably remove
void World::SetWorldScrollCompensation(float compensation)
{
	m_scrollspeed_compensation = compensation;
}

void World::UpdateSounds()
{
	// Set listener's position to player position
	m_sounds.SetListenerPosition(m_player_1_tank->GetWorldPosition());

	// Remove unused sounds
	m_sounds.RemoveStoppedSounds();
}
