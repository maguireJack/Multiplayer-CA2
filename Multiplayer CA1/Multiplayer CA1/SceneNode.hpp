//Alex Nogueira / D00242564
#pragma once
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>

#include <vector>
#include <memory>
#include <set>

#include "Command.hpp"
#include "CommandQueue.hpp"

class World;

class SceneNode : public sf::Transformable, public sf::Drawable, private sf::NonCopyable
{
public:
	typedef  std::unique_ptr<SceneNode> Ptr;
	typedef std::pair<SceneNode*, SceneNode*> Pair;

public:
	explicit SceneNode(World* world, bool collidable = false, Category::Type category = Category::kNone);
	virtual ~SceneNode();
	void AttachChild(Ptr child);
	Ptr DetachChild(const SceneNode& node);

	void Update(sf::Time dt, CommandQueue& commands);

	sf::Vector2f GetWorldPosition() const;
	sf::Transform GetWorldTransform() const;

	void OnCommand(const Command& command, sf::Time dt);
	virtual unsigned int GetCategory() const;
	virtual sf::FloatRect GetBoundingRect() const;

	void CheckSceneCollision(SceneNode& scene_graph, std::set<Pair>& collision_pairs);
	void RemoveWrecks();

	bool IsStatic();

protected:
	void ClearChildren();
	SceneNode* GetChild(int index);
	int ChildCount() const;

private:
	virtual void UpdateCurrent(sf::Time dt, CommandQueue& commands);
	void UpdateChildren(sf::Time dt, CommandQueue& commands);

	//Note draw is from sf::Drawable hence the name, lower case d
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void DrawCurrent(sf::RenderTarget&, sf::RenderStates states) const;
	void DrawChildren(sf::RenderTarget& target, sf::RenderStates states) const;

	void DrawBoundingRect(sf::RenderTarget& target, sf::RenderStates states, sf::FloatRect& bounding_rect) const;

	virtual bool IsDestroyed() const;
	bool IsMarkedForRemoval() const;
	
	void CheckNodeCollision(SceneNode& node, std::set<Pair>& collisionPairs);

protected:
	bool is_collidable;
	bool is_static;
	Category::Type m_category;
	World* m_world;
private:
	std::vector<Ptr> m_children;
	SceneNode* m_parent;
};
float distance(const SceneNode& lhs, const SceneNode& rhs);
