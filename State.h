#ifndef _State_h_
#define _State_h_

#include "StateIdentifiers.h"
#include "ResourceIdentifiers.h"

#include <SFML\Graphics.hpp>

namespace sf
{
	class RenderWindow;
}

class StateStack;
class Player;
class PlayerPlatformer;

class State
{
public:
	typedef std::unique_ptr<State> Ptr;

	struct Context 
	{
							Context(sf::RenderWindow& window, TextureManager& textures, FontManager& fonts, Player& player, PlayerPlatformer& playerPlatformer);

		sf::RenderWindow*	window;
		TextureManager*		textures;
		FontManager*		fonts;
		Player*				player;
		PlayerPlatformer*	playerPlatformer;
	};

public:
	State(StateStack& stack, Context context);
	virtual ~State();

	virtual void draw() = 0;
	virtual bool update(sf::Time dt) = 0;
	virtual bool handleEvent(const sf::Event& event) = 0;
	
protected:
	void requestStackPush(States::ID stateID);
	void requestStackPop();
	void requestStateClear();

	Context getContext() const;
	
private:
	StateStack* mStack;
	Context		mContext;
};

#endif