#include "PlayerPlatformer.h"
#include "CommandQueue.h"
#include "Platformer.h"
#include "Foreach.h"

#include <map>
#include <string>
#include <algorithm>

using namespace std::placeholders;

PlayerPlatformer::PlayerPlatformer()
: mCurrentMissionStatus(MissionRunning)
{
	// Set initial key bindings
	mKeyBinding[sf::Keyboard::A]		= MoveLeft;
	mKeyBinding[sf::Keyboard::D]		= MoveRight;
	mKeyBinding[sf::Keyboard::S]		= MoveDown;
	mKeyBinding[sf::Keyboard::Z]		= Attack;
	mKeyBinding[sf::Keyboard::Space]	= Jump;
 
	// Set initial action bindings
	initializeActions();	

	// Assign all categories to player's aircraft
	FOREACH(auto& pair, mActionBinding)
		pair.second.category = CommandCategory::PlayerShip;
}

void PlayerPlatformer::handleEvent(const sf::Event& event, CommandQueue& commands)
{
	if (event.type == sf::Event::KeyPressed)
	{
		// Check if pressed key appears in key binding, trigger command if so
		auto found = mKeyBinding.find(event.key.code);
		if (found != mKeyBinding.end() && !isRealtimeAction(found->second))
		{
			commands.push(mActionBinding[found->second]);
		}
	}
}

void PlayerPlatformer::handleRealtimeInput(CommandQueue& commands)
{
	// Traverse all assigned keys and check if they are pressed
	FOREACH(auto pair, mKeyBinding)
	{
		// If key is pressed, lookup action and trigger corresponding command
		if (sf::Keyboard::isKeyPressed(pair.first) && isRealtimeAction(pair.second)){
			commands.push(mActionBinding[pair.second]);
		}
	}
}

void PlayerPlatformer::assignKey(Action action, sf::Keyboard::Key key)
{
	// Remove all keys that already map to action
	for (auto itr = mKeyBinding.begin(); itr != mKeyBinding.end(); )
	{
		if (itr->second == action)
			mKeyBinding.erase(itr++);
		else
			++itr;
	}

	// Insert new binding
	mKeyBinding[key] = action;
}

sf::Keyboard::Key PlayerPlatformer::getAssignedKey(Action action) const
{
	FOREACH(auto pair, mKeyBinding)
	{
		if (pair.second == action)
			return pair.first;
	}

	return sf::Keyboard::Unknown;
}

void PlayerPlatformer::setMissionStatus(MissionStatus status)
{
	mCurrentMissionStatus = status;
}

PlayerPlatformer::MissionStatus PlayerPlatformer::getMissionStatus() const
{
	return mCurrentMissionStatus;
}

void PlayerPlatformer::initializeActions()
{
	mActionBinding[MoveLeft].action      = derivedAction<Platformer>([] (Platformer& a, sf::Time) { a.getInput()->left = true; });
	mActionBinding[MoveRight].action     = derivedAction<Platformer>([] (Platformer& a, sf::Time) { a.getInput()->right = true;	});
	mActionBinding[MoveDown].action      = derivedAction<Platformer>([] (Platformer& a, sf::Time) { a.getInput()->down = true; });
	mActionBinding[Attack].action        = derivedAction<Platformer>([] (Platformer& a, sf::Time) { a.getInput()->attack = true; });
	mActionBinding[Jump].action		     = derivedAction<Platformer>([] (Platformer& a, sf::Time) {	if(!a.getInput()->jumpPressOld) {a.getInput()->jumpPress = true;} a.getInput()->jump = true; });
}

bool PlayerPlatformer::isRealtimeAction(Action action)
{
	switch (action)
	{
		case MoveLeft:
		case MoveRight:
		case MoveDown:
		case Attack:
		case Jump:
			return true;

		default:
			return false;
	}
}