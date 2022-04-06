#pragma once
#include "BoundLabel.hpp"
#include "Container.hpp"
#include "State.hpp"
#include "World.hpp"
#include "Player.hpp"
#include "GameServer.hpp"
#include "Image.hpp"
#include "NetworkProtocol.hpp"

class MultiplayerGameState : public State
{
public:
	MultiplayerGameState(StateStack& stack, Context context, bool is_host);
	virtual void Draw();
	virtual bool Update(sf::Time dt);
	virtual bool HandleEvent(const sf::Event& event);
	virtual void OnActivate();
	void OnDestroy();
	void DisableAllRealtimeActions();

private:
	void UpdateBroadcastMessage(sf::Time elapsed_time);
	void HandlePacket(sf::Int32 packet_type, sf::Packet& packet);
	void UpdateIcons();
	void BindGui(const FontHolder& fonts);
	void CreatePlayerLabels(const FontHolder& fonts, sf::Vector2f offset, int text_size, std::string prefix,
	                        std::function<std::function<std::string()>(const Tank* const t)> func_factory);
	void CreateLabel(const FontHolder& fonts, sf::Color text_color, sf::Vector2f position, int text_size,
	                 std::string prefix, std::function<std::string()> update_action);
	void UpdateLabels() const;

private:
	typedef std::unique_ptr<Player> PlayerPtr;

private:
	World m_world;
	sf::RenderWindow& m_window;
	TextureHolder& m_texture_holder;

	std::map<int, PlayerPtr> m_players;
	sf::Int32 m_local_player_identifier;
	sf::TcpSocket m_socket;
	bool m_connected;
	std::unique_ptr<GameServer> m_game_server;
	sf::Clock m_tick_clock;

	std::vector<std::string> m_broadcasts;
	sf::Text m_broadcast_text;
	sf::Time m_broadcast_elapsed_time;

	sf::Text m_failed_connection_text;
	sf::Clock m_failed_connection_clock;

	bool m_active_state;
	bool m_has_focus;
	bool m_host;
	bool m_game_started;
	sf::Time m_client_timeout;
	sf::Time m_time_since_last_packet;
	sf::Time m_lobby_timer;
	sf::Text m_lobby_text;

	//GUI
	Image::Ptr m_player_fire_rate_upgrade_image;
	Image::Ptr m_player_explosion_upgrade_image;
	std::vector<GUI::BoundLabel::Ptr> m_bound_labels;
	GUI::Container m_container;
	sf::FloatRect m_gui_area;
	sf::Vector2f m_gui_center;
};

