#include "StateDefMenu.h"

#include "GuiButton.h"
#include "Utility.h"
#include "ResourceManager.h"

StateDefMenu::StateDefMenu(StateStack& stack, Context context)
: State(stack, context)
, mGUIContainer()
{
	sf::Texture& texture = context.textures->get(Textures::TitleScreen);
	mBackgroundSprite.setTexture(texture);

	auto playButton = std::make_shared<GUI::GuiButton>(*context.fonts, *context.textures);
	playButton->setPosition(100, 250);
	playButton->setText("Play");
	playButton->setCallback([this] ()
	{
		requestStackPop();
		requestStackPush(States::Test);
	});

	auto settingsButton = std::make_shared<GUI::GuiButton>(*context.fonts, *context.textures);
	settingsButton->setPosition(100, 300);
	settingsButton->setText("Settings");
	settingsButton->setCallback([this] ()
	{
		requestStackPush(States::MenuSettings);
	});

	auto exitButton = std::make_shared<GUI::GuiButton>(*context.fonts, *context.textures);
	exitButton->setPosition(100, 350);
	exitButton->setText("Exit");
	exitButton->setCallback([this] ()
	{
		requestStackPop();
	});

	mGUIContainer.pack(playButton);
	mGUIContainer.pack(settingsButton);
	mGUIContainer.pack(exitButton);
}

void StateDefMenu::draw()
{
	sf::RenderWindow& window = *getContext().window;

	window.setView(window.getDefaultView());

	window.draw(mBackgroundSprite);
	window.draw(mGUIContainer);
}

bool StateDefMenu::update(sf::Time)
{
	return true;
}

bool StateDefMenu::handleEvent(const sf::Event& event)
{
	mGUIContainer.handleEvent(event);
	return false;
}