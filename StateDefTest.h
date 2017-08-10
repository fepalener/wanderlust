#ifndef _StateDefTest_h_
#define _StateDefTest_h_

#include "State.h"
#include "Environment.h"
#include "PlayerPlatformer.h"
#include "AnimationManager.h"

#include "GUI\Gui.h"

#include <SFML\Graphics.hpp>

class StateDefTest : public State
{
	public:
							StateDefTest(StateStack& stack, Context context);
		virtual void		draw();
		virtual bool		update(sf::Time dt);
		virtual bool		handleEvent(const sf::Event& event);

	private:
		
		GUI::Gui			mGui;
		Environment			mEnvironment;
		PlayerPlatformer&	mPlayerPlatformer;
		Anim				mAnim;
};

#endif