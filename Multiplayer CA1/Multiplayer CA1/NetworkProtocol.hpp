#pragma once
#include <SFML/System/Vector2.hpp>

#include "PickupType.hpp"

const unsigned short SERVER_PORT = 50000;

namespace Server
{
	//These are packets that come from the Server
	enum class PacketType
	{
		BroadcastMessage,
		InitialState,
		PlayerEvent,
		PlayerRealtimeChange,
		PlayerConnect,
		PlayerDisconnect,
		SpawnPickup,
		SpawnSelf,
		UpdateClientState,
		GameOver
	};
}

namespace Client
{
	//Messages sent from the Client
	enum class PacketType
	{
		PlayerEvent,
		PlayerRealtimeChange,
		PositionUpdate,
		GameEvent,
		Quit
	};
}

namespace GameActions
{
	enum Type
	{
		EnemyExplode,
		PickupSpawn
	};

	struct Action
	{
		Action()
		{
			
		}

		Action(Type type, sf::Vector2f position, PickupType pickupType) :type(type), position(position), pickup_type(pickupType)
		{

		}

		Type type;
		sf::Vector2f position;
		PickupType pickup_type;
	};
}

