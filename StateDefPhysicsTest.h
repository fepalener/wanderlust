#ifndef _StateDefPhysicsTest_h_
#define _StateDefPhysicsTest_h_

#include "State.h"
#include "Physics.h"
#include "PhysicsDebugDraw.h"

#include <SFML\Graphics.hpp>

class StateDefPhysicsTest : public State
{
	public:
							StateDefPhysicsTest(StateStack& stack, Context context);

		virtual void		draw();
		virtual bool		update(sf::Time dt);
		virtual bool		handleEvent(const sf::Event& event);

	private:
		b2Body*				createSquare(b2World &world);

	private:
		b2World				mB2World;
		PhysicsDebugDraw	mDebugDraw;
	
		//Converts SFML's vector to Box2D's vector and downscales it so it fits Box2D's MKS units
		template<typename T > 
		b2Vec2 sfVecToB2Vec(sf::Vector2<T> vector)
		{
			return b2Vec2(vector.x / sfdd::SCALE, vector.y / sfdd::SCALE);
		}
};

#endif