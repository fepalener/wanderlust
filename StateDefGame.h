#ifndef _StateDefGame_h_
#define _StateDefGame_h_

#include "State.h"
#include "World.h"
#include "Player.h"

#include <SFML\Graphics.hpp>

class StateDefGame : public State
{
	public:
							StateDefGame(StateStack& stack, Context context);

		virtual void		draw();
		virtual bool		update(sf::Time dt);
		virtual bool		handleEvent(const sf::Event& event);


	private:
		World				mWorld;
		Player&				mPlayer;
};

#endif