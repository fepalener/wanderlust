#ifndef _StateDefMenuSettings_h_
#define _StateDefMenuSettings_h_

#include "State.h"
#include "GuiContainer.h"
#include "GuiButton.h"
#include "GuiLabel.h"

#include <SFML/Graphics.hpp>

#include <map>
#include <array>

class StateDefMenuSettings : public State
{
	public:
								StateDefMenuSettings(StateStack& stack, Context context);

		virtual void			draw();
		virtual bool			update(sf::Time dt);
		virtual bool			handleEvent(const sf::Event& event);


	private:
		void					updateLabels();
		void					addButtonLabel(int action, float y, const std::string& text, Context context);


	private:
		sf::Sprite							mBackgroundSprite;
		GUI::GuiContainer					mGUIContainer;

		std::map<std::string, int>			mScreenResolutions;
		std::array<GUI::GuiButton::Ptr, 1>	mBindingButtons;
		std::array<GUI::GuiLabel::Ptr, 1> 	mBindingLabels;
};

#endif