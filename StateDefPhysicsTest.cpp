#include "StateDefPhysicsTest.h"

StateDefPhysicsTest::StateDefPhysicsTest(StateStack& stack, Context context)
: State(stack, context)
, mDebugDraw(*context.window)
, mB2World(b2Vec2(0.0f, physics::GRAVITY))
{
	mB2World.SetAllowSleeping(true);

	/* Initialize SFML Debug Draw */
	mB2World.SetDebugDraw(&mDebugDraw);

	/* Set initial flags for what to draw */
	mDebugDraw.SetFlags(b2Draw::e_shapeBit); //Only draw shapes

	/* Create the bounding box */
	b2BodyDef boundingBoxDef;
	boundingBoxDef.type = b2_staticBody;
	float xPos = (context.window->getSize().x / 2.f) / sfdd::SCALE;
	float yPos = 0.5f;
	boundingBoxDef.position.Set(xPos, yPos);

	b2Body* boundingBoxBody = mB2World.CreateBody(&boundingBoxDef);

	b2PolygonShape boxShape;
	boxShape.SetAsBox((context.window->getSize().x) / sfdd::SCALE, 0.5f, b2Vec2(0.f, 0.f), 0.f);
	boundingBoxBody->CreateFixture(&boxShape, 1.0); //Top

	yPos = (context.window->getSize().y) / sfdd::SCALE - 1.f;
	boxShape.SetAsBox((context.window->getSize().x) / sfdd::SCALE, 0.5f, b2Vec2(0.f, yPos), 0.f);
	boundingBoxBody->CreateFixture(&boxShape, 1.f); //Bottom

	xPos -= 0.5f;
	boxShape.SetAsBox(0.5f, (context.window->getSize().y) / sfdd::SCALE, b2Vec2(-xPos, 0.f), 0.f);
	boundingBoxBody->CreateFixture(&boxShape, 1.f);//Left

	boxShape.SetAsBox(0.5f, (context.window->getSize().y) / sfdd::SCALE, b2Vec2(xPos, 0.f), 0.f);
	boundingBoxBody->CreateFixture(&boxShape, 1.f);//Right

}

void StateDefPhysicsTest::draw()
{
	mB2World.DrawDebugData();
}

bool StateDefPhysicsTest::update(sf::Time dt)
{
	mB2World.Step(physics::timeStep,physics::velocityIterations,physics::positionIterations);

	return true;
}

bool StateDefPhysicsTest::handleEvent(const sf::Event& event)
{
	// Escape pressed, trigger the pause screen
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		requestStackPush(States::Pause);

	if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Num1)
	{
		createSquare(mB2World);
	}

	return true;
}

b2Body*	StateDefPhysicsTest::createSquare(b2World &world)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position = b2Vec2(100 / sfdd::SCALE, 100 / sfdd::SCALE);// sfVecToB2Vec(sf::Mouse::getPosition(*context.window));
	b2Body* body = world.CreateBody(&bodyDef);


	b2PolygonShape boxShape;
	boxShape.SetAsBox(0.5f, 0.5f, b2Vec2(0.f, 0.f), 0);
	body->CreateFixture(&boxShape, 1.f);
	
	return body;
}