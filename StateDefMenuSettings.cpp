#include "StateDefMenuSettings.h"
#include "Utility.h"
#include "ResourceManager.h"

#include <SFML/Graphics.hpp>


StateDefMenuSettings::StateDefMenuSettings(StateStack& stack, Context context)
: State(stack, context)
, mGUIContainer()
{
	mBackgroundSprite.setTexture(context.textures->get(Textures::TitleScreen));
	
	// Build key binding buttons and labels
	addButtonLabel(0, 300.f, "Screen resolution", context);

	updateLabels();

	auto backButton = std::make_shared<GUI::GuiButton>(*context.fonts, *context.textures);
	backButton->setPosition(80.f, 420.f);
	backButton->setText("Back");
	backButton->setCallback(std::bind(&StateDefMenuSettings::requestStackPop, this));

	mGUIContainer.pack(backButton);
}

void StateDefMenuSettings::draw()
{
	sf::RenderWindow& window = *getContext().window;

	window.draw(mBackgroundSprite);
	window.draw(mGUIContainer);
}

bool StateDefMenuSettings::update(sf::Time)
{
	return true;
}

bool StateDefMenuSettings::handleEvent(const sf::Event& event)
{
	bool isKeyBinding = false;
	
	// Iterate through all key binding buttons to see if they are being pressed, waiting for the user to enter a key
	for (std::size_t action = 0; action < 1; ++action)
	{
		if (mBindingButtons[action]->isActive())
		{
			isKeyBinding = true;
			if (event.type == sf::Event::KeyReleased)
			{
				//getContext().player->assignKey(static_cast<Player::Action>(action), event.key.code);
				//mBindingButtons[action]->deactivate();
			}
			break;
		}
	}

	// If pressed button changed key bindings, update labels; otherwise consider other buttons in container
	if (isKeyBinding)
		updateLabels();
	else
		mGUIContainer.handleEvent(event);

	return false;
}

void StateDefMenuSettings::updateLabels()
{
	/*
	Player& player = *getContext().player;

	for (std::size_t i = 0; i < ScreenResolution::Count; ++i)
	{
		sf::Keyboard::Key key = player.getAssignedKey(static_cast<ScreenResolution>(i));
		mBindingLabels[i]->setText(toString(key));
	}
	*/
}

void StateDefMenuSettings::addButtonLabel(int action, float y, const std::string& text, Context context)
{
	mBindingButtons[action] = std::make_shared<GUI::GuiButton>(*context.fonts, *context.textures);
	mBindingButtons[action]->setPosition(80.f, y);
	mBindingButtons[action]->setText(text);
	mBindingButtons[action]->setToggle(true);

	mBindingLabels[action] = std::make_shared<GUI::GuiLabel>("", *context.fonts);
	mBindingLabels[action]->setPosition(300.f, y + 15.f);

	mGUIContainer.pack(mBindingButtons[action]);
	mGUIContainer.pack(mBindingLabels[action]);
}