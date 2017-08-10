#ifndef _Command_h_
#define _Command_h_

#include "CommandCategory.h"
#include "SFML\Graphics.hpp"
#include "SFML\System.hpp"

#include <functional>
#include <cassert>

class SceneNode;

struct Command
{
	typedef std::function<void(SceneNode&, sf::Time)> Action;

								Command();

	Action						action;
	unsigned int				category;
};

template <typename GameObject, typename Function>
Command::Action derivedAction(Function fn)
{
	return [=] (SceneNode& node, sf::Time dt)
	{
		// Check if cast is safe
		assert(dynamic_cast<GameObject*>(&node) != nullptr);

		// Downcast node and invoke function on it
		fn(static_cast<GameObject&>(node), dt);
	};
}

#endif