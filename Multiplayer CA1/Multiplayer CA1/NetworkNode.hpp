#pragma once
#include "SceneNode.hpp"
#include "NetworkProtocol.hpp"

#include <queue>

class NetworkNode : public SceneNode
{
public:
	NetworkNode(World* world);
	void NotifyGameAction(GameActions::Action action);
	bool PollGameAction(GameActions::Action& out);
	virtual unsigned int GetCategory() const override;

private:
	std::queue<GameActions::Action> m_pending_actions;
};

