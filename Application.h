#ifndef _Application_h_
#define _Application_h_

#include "ResourceManager.h"
#include "ResourceIdentifiers.h"

#include "Player.h"
#include "PlayerPlatformer.h"

#include "StateStack.h"

#include <SFML\Graphics.hpp>

class Application
{
public:
			Application();

	void	run();

private:
	void	processInput();
	void	update(sf::Time dt);
	void	render();

	void	updateStatistics(sf::Time dt);
	void	registerStates();

private:
	static const sf::Time	TimePerFrame;

	sf::RenderWindow		mWindow;
	TextureManager			mTextureManager;
	FontManager				mFontManager;

	Player					mPlayer;
	PlayerPlatformer		mPlayerPlatformer;

	StateStack				mStateStack;

	sf::Text				mStatisticsText;
	sf::Time				mStatisticsUpdateTime;
	std::size_t				mStatisticsNumFrames;
};

#endif