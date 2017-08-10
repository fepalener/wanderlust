#ifndef _StateDefMenu_h_
#define _StateDefMenu_h_

#include "State.h"
#include "GuiContainer.h"
#include <SFML\Graphics.hpp>

class StateDefMenu : public State
{
	public:
								StateDefMenu(StateStack& stack, Context context);

		virtual void			draw();
		virtual bool			update(sf::Time dt);
		virtual bool			handleEvent(const sf::Event& event);

	private:
		sf::Sprite				mBackgroundSprite;
		GUI::GuiContainer		mGUIContainer;
};

#endif