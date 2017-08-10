#ifndef _StateDefPause_h_
#define _StateDefPause_h_

#include "State.h"
#include "GuiContainer.h"

#include <SFML\Graphics.hpp>

class StateDefPause : public State
{
	public:
							StateDefPause(StateStack& stack, Context context);

		virtual void		draw();
		virtual bool		update(sf::Time dt);
		virtual bool		handleEvent(const sf::Event& event);


	private:
		sf::Sprite			mBackgroundSprite;
		sf::Text			mPausedText;
		GUI::GuiContainer 	mGUIContainer;
};

#endif