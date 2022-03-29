#include "NetworkNode.hpp"

NetworkNode::NetworkNode()
: SceneNode()
, m_pending_actions()
{
}

void NetworkNode::NotifyGameAction(GameActions::Action action)
{
	m_pending_actions.push(action);
}

bool NetworkNode::PollGameAction(GameActions::Action& out)
{
	if (m_pending_actions.empty())
	{
		return false;
	}
	else
	{
		out = m_pending_actions.front();
		m_pending_actions.pop();
		return true;
	}
}

unsigned NetworkNode::GetCategory() const
{
	return Category::kNetwork;
}
