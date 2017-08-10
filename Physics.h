#ifndef _Physics_h_
#define _Physics_h_

#include <Box2D\Box2d.h>
#include <Box2D\Common\b2Math.h>

extern std::auto_ptr<b2World> pWorld;

namespace physics
{
	const float GRAVITY = 10.f;
	const float timeStep = 1.0f / 60.0f;
	const int velocityIterations = 6;
	const int positionIterations = 2;
}

#endif