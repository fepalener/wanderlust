#ifndef _PlanAction_h_
#define _PlanAction_h_

#include <SFML\Graphics.hpp>

class PlanAction
{
public:
	enum ACTION
	{
		NONE,
		WALK,
		JUMP,
	};

public:
	PlanAction(sf::Vector2f point, ACTION type, PlanAction* next = 0);

public:
	sf::Vector2f	mPoint;
	ACTION			mType;
	PlanAction*		mNext;
};

#endif