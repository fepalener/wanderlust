#include "Player.h"
#include "CommandQueue.h"
#include "Character.h"
#include "Foreach.h"

#include <map>
#include <string>
#include <algorithm>

using namespace std::placeholders;


struct CharacterMover
{
	CharacterMover(float vx, float vy)
	: velocity(vx, vy)
	{
	}

	void operator() (Character& character, sf::Time) const
	{
		character.accelerate(velocity * character.getMaxSpeed());
	}

	sf::Vector2f velocity;
};

Player::Player()
: mCurrentMissionStatus(MissionRunning)
{
	// Set initial key bindings
	mKeyBinding[sf::Keyboard::A]		= MoveLeft;
	mKeyBinding[sf::Keyboard::D]		= MoveRight;
	mKeyBinding[sf::Keyboard::Space]	= Attack;
 
	// Set initial action bindings
	initializeActions();	

	// Assign all categories to player's aircraft
	FOREACH(auto& pair, mActionBinding)
		pair.second.category = CommandCategory::PlayerShip;
}

void Player::handleEvent(const sf::Event& event, CommandQueue& commands)
{
	if (event.type == sf::Event::KeyPressed)
	{
		// Check if pressed key appears in key binding, trigger command if so
		auto found = mKeyBinding.find(event.key.code);
		if (found != mKeyBinding.end() && !isRealtimeAction(found->second))
			commands.push(mActionBinding[found->second]);
	}
}

void Player::handleRealtimeInput(CommandQueue& commands)
{
	// Traverse all assigned keys and check if they are pressed
	FOREACH(auto pair, mKeyBinding)
	{
		// If key is pressed, lookup action and trigger corresponding command
		if (sf::Keyboard::isKeyPressed(pair.first) && isRealtimeAction(pair.second))
			commands.push(mActionBinding[pair.second]);
	}
}

void Player::assignKey(Action action, sf::Keyboard::Key key)
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

sf::Keyboard::Key Player::getAssignedKey(Action action) const
{
	FOREACH(auto pair, mKeyBinding)
	{
		if (pair.second == action)
			return pair.first;
	}

	return sf::Keyboard::Unknown;
}

void Player::setMissionStatus(MissionStatus status)
{
	mCurrentMissionStatus = status;
}

Player::MissionStatus Player::getMissionStatus() const
{
	return mCurrentMissionStatus;
}

void Player::initializeActions()
{
	mActionBinding[MoveLeft].action      = derivedAction<Character>(CharacterMover(-1,  0));
	mActionBinding[MoveRight].action     = derivedAction<Character>(CharacterMover(+1,  0));
	mActionBinding[Attack].action        = derivedAction<Character>([] (Character& a, sf::Time) { a.fire(); });
}

bool Player::isRealtimeAction(Action action)
{
	switch (action)
	{
		case MoveLeft:
		case MoveRight:
		case Attack:
			return true;

		default:
			return false;
	}
}