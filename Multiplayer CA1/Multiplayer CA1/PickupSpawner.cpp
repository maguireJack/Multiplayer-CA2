//Alex Nogueira / D00242564 
#include "PickupSpawner.hpp"
#include "NetworkNode.hpp"

PickupSpawner::PickupSpawner(World* world)
	: SceneNode(world), m_life(sf::seconds(10))
{
	is_static = true;
}

void PickupSpawner::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
	if (m_notify_pickup_spawn_on_next_update)
	{
		sf::Vector2f position = GetWorldPosition();
		PickupType type = m_pickup_type;

		Command command;
		command.category = Category::kNetwork;
		command.action = DerivedAction<NetworkNode>([position, type](NetworkNode& node, sf::Time)
		{
			node.NotifyGameAction(GameActions::Action(GameActions::PickupSpawn, position, type));
		});

		commands.Push(command);

		m_notify_pickup_spawn_on_next_update = false;
	}

	if (HasPickup())
	{
		m_current_life -= dt;
		if (m_current_life.asSeconds() <= 0.f)
		{
			static_cast<Pickup*>(GetChild(0))->Destroy();
		}
	}
}

void PickupSpawner::SpawnPickup(PickupType type, const TextureHolder& textures)
{
	m_current_life = m_life;
	auto p = std::make_unique<Pickup>(m_world, type, textures);
	AttachChild(std::move(p));
	m_pickup = p.get();
}

void PickupSpawner::SpawnPickupNetwork(PickupType type, const TextureHolder& textures)
{
	m_notify_pickup_spawn_on_next_update = true;
	m_pickup_type = type;

	m_current_life = m_life;
	auto p = std::make_unique<Pickup>(m_world, type, textures);
	AttachChild(std::move(p), false);
	m_pickup = p.get();
}

bool PickupSpawner::HasPickup() const
{
	return ChildCount() > 0;
}

Pickup* PickupSpawner::GetPickup()
{
	return m_pickup;
}
