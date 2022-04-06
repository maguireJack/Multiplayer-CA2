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

World::World(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, bool networked, bool is_host)
	: m_target(output_target)
	  , m_camera(m_target.getDefaultView())
	  , m_textures()
	  , m_fonts(font)
	  , m_scenegraph(this)
	  , m_scene_layers()
	  , m_world_bounds(0.f, 0.f, 2700, 1800)
	  , m_world_center(m_world_bounds.width / 2.f, m_world_bounds.height / 2.f)
	  , m_arena_bounds(80.f, 80.f, m_world_bounds.width - 160.f, m_world_bounds.height - 160.f)
	  , m_scrollspeed(-50.f)
	  , m_player_tank(nullptr)
	  , m_player_spawned(false)
	  , m_game_over(false)
	  , m_winner("")
	  , m_sounds(sounds)
	  , m_max_shake_timer(sf::seconds(0.25))
	  , m_max_shake_intensity(10)
	  , m_networked_world(networked)
	  , m_is_host(is_host)
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

	//Destroy all pickups that reached the end of their lifetime
	for (int i = 0; i < m_pickups.size(); i++)
	{
		m_pickup_lifetimes[i] -= dt.asSeconds();
		if (m_pickup_lifetimes[i] <= 0)
		{
			m_pickups[i]->Destroy();
			m_pickups.erase(m_pickups.begin() + i);
			m_pickup_lifetimes.erase(m_pickup_lifetimes.begin() + i);
			i--;
		}
	}

	//Forward commands to the scenegraph until the command queue is empty
	while (!m_command_queue.IsEmpty())
	{
		m_scenegraph.OnCommand(m_command_queue.Pop(), dt);
	}

	AdaptPlayerVelocity();
	AdaptPlayerPosition();

	if (m_player_spawned) m_camera.setCenter(GetPlayer()->getPosition());

	HandleCollisions();

	m_scenegraph.RemoveWrecks();

	//Apply movement
	m_scenegraph.Update(dt, m_command_queue);

	if (m_player_tank != nullptr && m_player_tank->GetHitPoints() > 0)
	{
		UpdateSounds();
	}

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

void World::RegisterCollidableSceneNode(SceneNode* node)
{
	m_non_static_coll.emplace_back(node);
	return;
	//If it's static we can add it to our collection of non moving stuff -> already sorted
	if (node->IsStatic())
	{
		float xMin = node->GetBoundingRect().left;
		int index = 0;
		for (auto staticNode : m_static_coll)
		{
			if (staticNode->GetBoundingRect().left > xMin) break;
			index++;
		}
		m_static_coll.insert(m_static_coll.begin() + index, node);
	}
		//If not, we add it to the dynamic objects
	else
	{
		m_non_static_coll.emplace_back(node);
	}
}

void World::UnregisterCollidableSceneNode(SceneNode* node)
{
	m_non_static_coll.erase(std::find(m_non_static_coll.begin(), m_non_static_coll.end(), node));
	return;
	if (node->IsStatic())
	{
		m_static_coll.erase(std::find(m_static_coll.begin(), m_static_coll.end(), node));
	}
	else
	{
		m_non_static_coll.erase(std::find(m_non_static_coll.begin(), m_non_static_coll.end(), node));
	}
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
	m_textures.Load(Textures::kPlayer2Tank, "Media/Textures/Tanx.png", sf::IntRect(1, 34, 10, 10));
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
		SceneNode::Ptr layer(new SceneNode(this, false, category));
		m_scene_layers[i] = layer.get();
		m_scenegraph.AttachChild(std::move(layer));
	}


	//Prepare the background
	sf::Texture& texture = m_textures.Get(Textures::kBackground);
	sf::IntRect textureRect(m_world_bounds);
	//Tile the texture to cover our world
	texture.setRepeated(true);

	//Add the background sprite to our scene
	std::unique_ptr<SpriteNode> background_sprite(new SpriteNode(this, texture, textureRect));
	background_sprite->setPosition(m_world_bounds.left, m_world_bounds.top);
	m_scene_layers[static_cast<int>(Layers::kBackground)]->AttachChild(std::move(background_sprite));

	//Load the map
	std::vector<sf::Vector2f> spawner_positions;
	std::unique_ptr<Map> m(new Map(this, "Media/Arena Data/map", "Media/Textures/Tanx.png", 16, m_tank_spawns,
	                               spawner_positions, m_world_center,
	                               45.f, false));
	m_scene_layers[static_cast<int>(Layers::kBattlefield)]->AttachChild(std::move(m));


	// Add sound effect node
	std::unique_ptr<SoundNode> soundNode(new SoundNode(this, m_sounds));
	m_scenegraph.AttachChild(std::move(soundNode));

	/*std::unique_ptr<SpawnerManager> spawner_manager(new SpawnerManager(m_textures, sf::seconds(1), 0.2f));
	spawner_manager->setPosition(m_world_center);
	m_scene_layers[static_cast<int>(Layers::kBattlefield)]->AttachChild(std::move(spawner_manager));*/

	if (m_networked_world)
	{
		std::unique_ptr<NetworkNode> network_node(new NetworkNode(this));
		m_network_node = network_node.get();
		m_scenegraph.AttachChild(std::move(network_node));

		if (m_is_host)
		{
			std::unique_ptr<SpawnerManager> spawner_manager(
				new SpawnerManager(this, m_textures, sf::seconds(1), spawner_positions, 0.5f, true));
			m_scene_layers[static_cast<int>(Layers::kBattlefield)]->AttachChild(std::move(spawner_manager));
			m_spawner_manager = spawner_manager.get();
		}
	}
	else
	{
		std::unique_ptr<SpawnerManager> spawner_manager(
			new SpawnerManager(this, m_textures, sf::seconds(1), spawner_positions, 0.5f));
		m_scene_layers[static_cast<int>(Layers::kBattlefield)]->AttachChild(std::move(spawner_manager));
	}
}

void World::SetWorldHeight(float height)
{
	m_world_bounds.height = height;
}

CommandQueue& World::getCommandQueue()
{
	return m_command_queue;
}

const Tank* const World::GetPlayer() const
{
	return m_player_tank;
}

bool World::IsGameOver() const
{
	return m_game_over;
}

std::string World::GetWinner() const
{
	return m_winner;
}

bool World::AllowPlayerInput()
{
	return !m_game_over;
}

void World::CreatePickup(sf::Vector2f position, PickupType type)
{
	if (m_is_host) return;
	std::unique_ptr<Pickup> pickup(new Pickup(this, type, m_textures));
	pickup->setPosition(position);
	m_pickups.emplace_back(pickup.get());
	m_pickup_lifetimes.emplace_back(10.f);
	m_scene_layers[static_cast<int>(Layers::kBattlefield)]->AttachChild(std::move(pickup));
}

sf::Vector2f World::GetTankSpawn(int tank_identifier)
{
	return m_tank_spawns[tank_identifier];
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

void World::AdaptPlayerPosition()
{
	for (Tank* tank : m_player_tanks)
	{
		sf::Vector2f position = tank->getPosition();
		position.x = std::max(position.x, m_arena_bounds.left);
		position.x = std::min(position.x, m_arena_bounds.left + m_arena_bounds.width);
		position.y = std::max(position.y, m_arena_bounds.top);
		position.y = std::min(position.y, m_arena_bounds.top + m_arena_bounds.height);
		tank->setPosition(position);
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

Tank* World::AddTank(int identifier, TankType type, sf::Vector2f position)
{
	std::unique_ptr<Tank> player(new Tank(this, type, m_textures, m_fonts, &m_ghost_world));
	player->setPosition(position);
	player->SetSpawnPos(position);
	player->SetMapBounds(m_world_bounds);
	player->SetIdentifier(identifier);

	if (type == TankType::kLocalTank)
	{
		m_player_tank = player.get();
		m_player_spawned = true;
		/*std::cout << "Added NETWORK tank to game with Identifier : " << identifier << " at spawn pos : " << position.x
			<< " , " << position.y << std::endl;*/
	}
	else
	{
		/*std::cout << "Added LOCAL tank to game with Identifier : " << identifier << " at spawn pos : " << position.x <<
			" , " << position.y << std::endl;*/
	}

	if (m_ghost_world)
	{
		m_ghost_tanks.emplace_back(player.get());
		m_scene_layers[static_cast<int>(Layers::kGhostWorld)]->AttachChild(std::move(player));
		return m_ghost_tanks.back();
	}
	m_player_tanks.emplace_back(player.get());
	m_scene_layers[static_cast<int>(Layers::kBattlefield)]->AttachChild(std::move(player));
	return m_player_tanks.back();
}

void World::HandleCollisions()
{
	std::sort(m_non_static_coll.begin(), m_non_static_coll.end(), [](SceneNode* a, SceneNode* b)
	{
		return a->GetBoundingRect().left < b->GetBoundingRect().left;
	});

	std::set<SceneNode::Pair> collision_pairs;
	std::vector<SceneNode*> candidates;
	float xMin = 0;
	float xMax = 0;
	int size = m_non_static_coll.size();

	int i = 0, j = 0;
	bool iFinish = false, jFinish = false;
	int index = 0;
	for (auto candidate : m_non_static_coll)
	{
		if (candidate->IsStatic())
		{
			index++;
			continue;
		}

		sf::FloatRect candidateRect = candidate->GetBoundingRect();
		xMin = candidateRect.left;
		xMax = candidateRect.left + candidateRect.width;
		while (!(iFinish && jFinish))
		{
			if (!iFinish)
			{
				i++;
				if ((index - i) < 0) iFinish = true;
				else
				{
					SceneNode* node = m_non_static_coll[index - i];
					sf::FloatRect boundingRect = node->GetBoundingRect();
					if (boundingRect.left + boundingRect.width < xMin)
					{
						iFinish = true;
					}
					else
					{
						if (candidateRect.intersects(boundingRect))
						{
							collision_pairs.emplace(std::minmax(candidate, node));
						}
					}
				}
			}
			if (!jFinish)
			{
				j++;
				if ((index + j) >= size) jFinish = true;
				else {
					SceneNode* node = m_non_static_coll[index + j];
					sf::FloatRect boundingRect = node->GetBoundingRect();
					if (boundingRect.left > xMax)
					{
						jFinish = true;
					}
					else
					{
						if (candidateRect.intersects(boundingRect))
						{
							collision_pairs.emplace(std::minmax(candidate, node));
						}
					}
				}
			}
		}

		iFinish = false;
		jFinish = false;
		i = 0;
		j = 0;
		index++;
	}

	//m_scenegraph.CheckSceneCollision(m_scenegraph, collision_pairs);
	for (SceneNode::Pair pair : collision_pairs)
	{
		if (MatchesCategories(pair, Category::Type::kTank, Category::Type::kPickup))
		{
			auto& player = static_cast<Tank&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);
			//Apply the m_pickup effect
			pickup.Apply(player);
			//Search for the pickup in your list and destroy it, the timer doesn't need to tick anymore for that pickup
			for (int i = 0; i < m_pickups.size(); i++)
			{
				if (m_pickups[i] == pair.second)
				{
					m_pickups.erase(m_pickups.begin() + i);
					m_pickup_lifetimes.erase(m_pickup_lifetimes.begin() + i);
				}
			}
			pickup.Destroy();
		}

		else if (MatchesCategories(pair, Category::Type::kTank, Category::Type::kProjectile))
		{
			auto& tank = static_cast<Tank&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);
			if (!projectile.CanDamagePlayers())
				continue;

			bool tank_was_alive = !tank.IsGhost();

			//Apply the projectile damage to the tank
			tank.Damage(projectile.GetDamage());
			projectile.AppliedPlayerDamage();
			projectile.Destroy();

			//Check if self has been hit
			if (&tank == m_player_tank)
			{
				m_shake_timer = m_max_shake_timer;
				m_ghost_world |= tank.IsExploding();
			}

			if (tank.IsExploding() && tank_was_alive && m_winner == "")
			{
				tank.OnFinishExploding = [this]
				{
					//TODO something
				};
			}
		}

		else if (MatchesCategories(pair, Category::Type::kGhostTank, Category::Type::kGhostProjectile))
		{
			auto& tank = static_cast<Tank&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);
			if (!projectile.CanDamagePlayers())
				continue;

			//Apply the projectile damage to the tank
			tank.Damage(projectile.GetDamage());
			projectile.AppliedPlayerDamage();
			projectile.Destroy();

			//Check if self has been hit
			if (&tank == m_player_tank)
			{
				m_shake_timer = m_max_shake_timer;
			}
		}

		else if (MatchesCategories(pair, Category::Type::kDestroyableTile, Category::Type::kProjectile))
		{
			auto& tile = static_cast<Tile&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);
			tile.Damage(1);
			projectile.Destroy();
		}

		else if (MatchesCategories(pair, Category::Type::kProjectile, Category::Type::kTile))
		{
			auto& projectile = static_cast<Projectile&>(*pair.first);
			projectile.Destroy();
		}

		else if (MatchesCategories(pair, Category::Type::kLocalTank, Category::Type::kTile) || MatchesCategories(
			pair, Category::Type::kEnemyTank, Category::Type::kTile))
		{
			auto& tank = static_cast<Tank&>(*pair.first);
			tank.ResetToLastPos();
		}
	}
}

void World::UpdateSounds()
{
	// Set listener's position to player position
	m_sounds.SetListenerPosition(m_player_tank->GetWorldPosition());

	// Remove unused sounds
	m_sounds.RemoveStoppedSounds();
}
