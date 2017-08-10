#ifndef _SceneNode_h_
#define _SceneNode_h_

#include "CommandCategory.h"

#include <SFML\Graphics.hpp>
#include <vector>
#include <set>
#include <memory>
#include <utility>

struct Command;
class CommandQueue;

class SceneNode : public sf::Transformable, public sf::Drawable, private sf::NonCopyable
{
	public:
		typedef std::unique_ptr<SceneNode> Ptr;
		typedef std::pair<SceneNode*, SceneNode*> Pair;


	public:
		explicit				SceneNode(CommandCategory::Type category = CommandCategory::None);

		void					attachChild(Ptr child);
		Ptr						detachChild(const SceneNode& node);
		
		void					update(sf::Time dt, CommandQueue& commands);

		sf::Vector2f			getWorldPosition() const;
		sf::Transform			getWorldTransform() const;

		void					onCommand(const Command& command, sf::Time dt);
		virtual unsigned int	getCategory() const;

		void					checkSceneCollision(SceneNode& sceneGraph, std::set<Pair>& collisionPairs);
		void					checkNodeCollision(SceneNode& node, std::set<Pair>& collisionPairs);
		void					removeWrecks();
		virtual sf::FloatRect	getBoundingRect() const;
		virtual bool			isMarkedForRemoval() const;
		virtual bool			isDestroyed() const;


	private:
		virtual void			updateCurrent(sf::Time dt, CommandQueue& commands);
		void					updateChildren(sf::Time dt, CommandQueue& commands);

		virtual void			draw(sf::RenderTarget& target, sf::RenderStates states) const;
		virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
		void					drawChildren(sf::RenderTarget& target, sf::RenderStates states) const;
		void					drawBoundingRect(sf::RenderTarget& target, sf::RenderStates states) const;


	private:
		std::vector<Ptr>		mChildren;
		SceneNode*				mParent;
		CommandCategory::Type	mDefaultCategory;
};

bool	collision(const SceneNode& lhs, const SceneNode& rhs);
float	distance(const SceneNode& lhs, const SceneNode& rhs);


#endif