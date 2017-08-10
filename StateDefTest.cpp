#include "StateDefTest.h"
#include "ResourceManager.h"
#include "GUI\GuiCtrlButton.h"


StateDefTest::StateDefTest(StateStack& stack, Context context)
: State(stack, context)
, mEnvironment(*context.window,*context.textures,*context.fonts)
, mGui()
, mPlayerPlatformer(*context.playerPlatformer)
{
	mPlayerPlatformer.setMissionStatus(PlayerPlatformer::MissionRunning);

	// GUI
	auto checkbox = std::make_shared<GUI::GuiCtrlButton>(sf::Vector2f(0,50), *context.textures);
	checkbox->setCallback([this](){
		printf("callback\n");
	});

	mGui.addCtrl(checkbox);

	std::unique_ptr<AnimationManager> animationManager = std::unique_ptr<AnimationManager>(new AnimationManager("../resources/character"));
	animationManager->load("character.anim");
	mAnim = animationManager->get("Run");
}

void StateDefTest::draw()
{
	sf::RenderWindow& window = *getContext().window;
	window.setView(window.getDefaultView());

	// DRAW GUI
	window.draw(mGui);

	//DRAW ENV
	mEnvironment.draw();

	//Draw Animation
	sf::RenderStates states;
	states.transform.translate(200,200);

	window.draw(mAnim, states);
}

bool StateDefTest::update(sf::Time dt)
{
	mGui.update(dt);

	mEnvironment.update(dt);

	CommandQueue& commands = mEnvironment.getCommandQueue();
	mPlayerPlatformer.handleRealtimeInput(commands);

	mAnim.update(dt);

	return true;
}

bool StateDefTest::handleEvent(const sf::Event& event)
{
	// Game input handling
	CommandQueue& commands = mEnvironment.getCommandQueue();
	mPlayerPlatformer.handleEvent(event, commands);

	// Escape pressed, trigger the pause screen
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		requestStackPush(States::Pause);

	mGui.handleEvent(event);

	return true;
}

