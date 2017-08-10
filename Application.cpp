#include "Application.h"
#include "StateIdentifiers.h"
#include "State.h"
#include "Utility.h"

#include "StateDefTitle.h"
#include "StateDefGame.h"
#include "StateDefMenu.h"
#include "StateDefMenuSettings.h"
#include "StateDefPause.h"
#include "StateDefTest.h"
#include "StateDefPhysicsTest.h"

const sf::Time Application::TimePerFrame = sf::seconds(1.f/60.f);

Application::Application()
: mWindow(sf::VideoMode(800, 600), "Graphics", sf::Style::Close)// | sf::Style::Fullscreen)
, mTextureManager()
, mFontManager()
, mPlayer()
, mPlayerPlatformer()
, mStateStack(State::Context(mWindow, mTextureManager, mFontManager, mPlayer, mPlayerPlatformer))
, mStatisticsText()
, mStatisticsUpdateTime()
, mStatisticsNumFrames(0)
{
	mWindow.setKeyRepeatEnabled(false);
	//mWindow.setVerticalSyncEnabled(true);

	mFontManager.load(Fonts::Main, 	"../resources/Sansation.ttf");

	mTextureManager.load(Textures::TitleScreen,	"../resources/Textures/TitleScreen.png");
	mTextureManager.load(Textures::Buttons,		"../resources/Textures/Buttons.png");
	mTextureManager.load(Textures::Tileset,		"../resources/Textures/Tileset.png");
	mTextureManager.load(Textures::Gui,			"../resources/Textures/Gui.png");
	mTextureManager.load(Textures::Collision,	"../resources/Textures/Collision.png");
	mTextureManager.load(Textures::Water,		"../resources/Textures/Water.png");
	mTextureManager.load(Textures::Player,		"../resources/Textures/Player.png");
	
	//test
	mTextureManager.load(Textures::TestImage,	"../resources/Textures/image.png");
	mTextureManager.load(Textures::TestLight,	"../resources/Textures/light.png");

	mStatisticsText.setFont(mFontManager.get(Fonts::Main));
	mStatisticsText.setPosition(5.f, 5.f);
	mStatisticsText.setCharacterSize(10u);

	registerStates();
	mStateStack.pushState(States::Title);
}

void Application::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;

	while (mWindow.isOpen())
	{
		sf::Time dt = clock.restart();
		timeSinceLastUpdate += dt;
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;

			processInput();
			update(TimePerFrame);

			// Check inside this loop, because stack might be empty before update() call
			if (mStateStack.isEmpty())
				mWindow.close();
		}

		updateStatistics(dt);
		render();
	}
}

void Application::processInput()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		mStateStack.handleEvent(event);

		if (event.type == sf::Event::Closed)
			mWindow.close();
	}
}

void Application::update(sf::Time dt)
{
	mStateStack.update(dt);
}

void Application::render()
{
	mWindow.clear();

	mStateStack.draw();

	mWindow.setView(mWindow.getDefaultView());
	mWindow.draw(mStatisticsText);

	mWindow.display();
}

void Application::updateStatistics(sf::Time dt)
{
	mStatisticsUpdateTime += dt;
	mStatisticsNumFrames += 1;
	if (mStatisticsUpdateTime >= sf::seconds(1.0f))
	{
		mStatisticsText.setString("FPS: " + toString(mStatisticsNumFrames));

		mStatisticsUpdateTime -= sf::seconds(1.0f);
		mStatisticsNumFrames = 0;
	}
}

void Application::registerStates()
{
	mStateStack.registerState<StateDefTitle>(States::Title);
	mStateStack.registerState<StateDefGame>(States::Game);
	mStateStack.registerState<StateDefMenu>(States::Menu);
	mStateStack.registerState<StateDefMenuSettings>(States::MenuSettings);
	mStateStack.registerState<StateDefPause>(States::Pause);
	mStateStack.registerState<StateDefTest>(States::Test);
	mStateStack.registerState<StateDefPhysicsTest>(States::PhysicsTest);
}