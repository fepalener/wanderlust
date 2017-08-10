#ifndef _StateDefTitle_h_
#define _StateDefTitle_h_

#include "State.h"

#include "SFML\Graphics.hpp"


class StateDefTitle : public State
{
	public:
							StateDefTitle(StateStack& stack, Context context);

		virtual void		draw();
		virtual bool		update(sf::Time dt);
		virtual bool		handleEvent(const sf::Event& event);


	private:
		sf::Sprite			mBackgroundSprite;
		sf::Text			mText;

		bool				mShowText;
		sf::Time			mTextEffectTime;
};


#endif