//Alex Nogueira / D00242564
#pragma once
#include "Command.hpp"
#include <SFML/Window/Event.hpp>
#include <map>
#include <SFML/Network/TcpSocket.hpp>

#include "Application.hpp"
#include "CommandQueue.hpp"
#include "MissionStatus.hpp"
#include "PlayerAction.hpp"
#include "KeyBinding.hpp"

class Player
{
public:
	Player(sf::TcpSocket* socket, sf::Int32 identifier, const KeyBinding* binding);
	void HandleEvent(const sf::Event& event, CommandQueue& commands);
	void HandleRealtimeInput(CommandQueue& commands);
	void HandleRealtimeNetworkInput(CommandQueue& commands);
	void SetMissionStatus(MissionStatus status);
	MissionStatus GetMissionStatus() const;

	//React to events or realtime state changes recevied over the network
	void HandleNetworkEvent(PlayerAction action, CommandQueue& commands);
	void HandleNetworkRealtimeChange(PlayerAction action, bool action_enabled);

	void DisableAllRealtimeActions();
	bool IsLocal() const;

private:
	void InitialiseActions();
	static bool IsRealtimeAction(PlayerAction action);

public:
	Category::Type m_winner;

private:
	const KeyBinding* m_key_binding;
	std::map<PlayerAction, Command> m_action_binding;
	std::map<PlayerAction, bool> m_action_proxies;
	MissionStatus m_current_mission_status;
	int m_identifier;
	sf::TcpSocket* m_socket;
};

