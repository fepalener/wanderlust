#ifndef _PlayerPlatformer_h_
#define _PlayerPlatformer_h_

#include "Command.h"

#include <SFML\Graphics.hpp>
#include <map>

class CommandQueue;

class PlayerPlatformer
{
public:
	enum Action
	{
		MoveLeft,
		MoveRight,
		MoveDown,
		Attack,
		Jump,
		ActionCount
	};

	enum MissionStatus
	{
		MissionRunning,
		MissionSuccess,
		MissionFailure
	};

public:
							PlayerPlatformer();
	void					handleEvent(const sf::Event& event, CommandQueue& commands);
	void					handleRealtimeInput(CommandQueue& commands);

	void					assignKey(Action action, sf::Keyboard::Key key);
	sf::Keyboard::Key		getAssignedKey(Action action) const;

	void 					setMissionStatus(MissionStatus status);
	MissionStatus 			getMissionStatus() const;

private:
	void					initializeActions();
	static bool				isRealtimeAction(Action action);

private:
	std::map<sf::Keyboard::Key, Action>		mKeyBinding;
	std::map<Action, Command>				mActionBinding;
	MissionStatus 							mCurrentMissionStatus;
};

#endif