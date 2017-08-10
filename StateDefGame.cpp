#include "StateDefGame.h"

StateDefGame::StateDefGame(StateStack& stack, Context context)
: State(stack, context)
, mWorld(*context.window, *context.fonts)
, mPlayer(*context.player)
{
	mPlayer.setMissionStatus(Player::MissionRunning);
}

void StateDefGame::draw()
{
	mWorld.draw();
}

bool StateDefGame::update(sf::Time dt)
{
	mWorld.update(dt);

	if(!mWorld.hasAlivePlayer())
	{
		mPlayer.setMissionStatus(Player::MissionFailure);
		//requestStackPush(States::GameOver);
	}
	else if(mWorld.hasPlayerReachedEnd())
	{
		mPlayer.setMissionStatus(Player::MissionSuccess);
		//requestStackPush(States::GameOver);
	}

	CommandQueue& commands = mWorld.getCommandQueue();
	mPlayer.handleRealtimeInput(commands);

	return true;
}

bool StateDefGame::handleEvent(const sf::Event& event)
{
	// Game input handling
	CommandQueue& commands = mWorld.getCommandQueue();
	mPlayer.handleEvent(event, commands);

	// Escape pressed, trigger the pause screen
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		requestStackPush(States::Pause);

	return true;
}