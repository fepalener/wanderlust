#include "Actor.h"

Actor::Actor(Room* room)
: mRoom(room)
, mPos(Vector2D(0,0))
, mVel(Vector2D(0,0))
, isFacingLeft(false)
, mInput(nullptr)
{
	rightContact = leftContact = topContact = bottomContact = false;
	accDX = 0;
	accDT = 0;

	// A few deault values:
	HEIGHT                   =  32;
	WIDTH                    =  24;
	
	WALK_SPEED               = 120.0f;
	
	CLIMB_26_SLOPE_SLOWDOWN  =   1.0f;
	CLIMB_45_SLOPE_SLOWDOWN  =   0.5f;
	
	WALK_ACCELERATION        = 900.0f;
	WALK_DECELERATION        = 900.0f;
	
	JUMP_STRENGTH            = 260.0f;
	WATER_JUMP_STRENGTH      = 220.0f;
	GRAVITY                  = 590.5f;
	MAX_FALL_SPEED           = 330.0f;
	
	SUBMERSION_HEIGHT        = 65.0f;
	BUOYANCY                 = 250.0f;
	MAX_FLOAT_SPEED          =  60.0f;
	MAX_SINK_SPEED           = 150.0f;
	SWIM_SPEED               =  90.0f;
	SWIM_ACCELERATION        = 225.0f;
	SWIM_DECELERATION        = 225.0f;
	
	FLOAT_ON_WATER_SURFACE   = true;

	// Now that all the parameters have been read... initialize the collision
	// structure:
	mCollisionStruct.setX( mPos.x - WIDTH/2 );
	mCollisionStruct.setY( mPos.y - HEIGHT );
	mCollisionStruct.setWidth( WIDTH );
	mCollisionStruct.setHeight( HEIGHT );
}

void Actor::update(sf::Time dt)
{
	// Store last frame's position.
	const Vector2D prevPos = mPos;

	// 
	// Horizontal controls:
	// 
	// Walk left?
	if ( mInput->left && !mInput->right ) {
		isFacingLeft = true;
		
		// Abovewater?
		if ( !isUnderwater() || !FLOAT_ON_WATER_SURFACE ) {
			// Make xSpeed tend to the walk speed:
			if ( mVel.x < -WALK_SPEED ) {
				mVel.x += WALK_DECELERATION * dt.asSeconds();
				
				if ( mVel.x > -WALK_SPEED )
					mVel.x = -WALK_SPEED;
			}
			else if ( mVel.x > -WALK_SPEED ) {
				mVel.x -= WALK_ACCELERATION * dt.asSeconds();
				
				if ( mVel.x < -WALK_SPEED )
					mVel.x = -WALK_SPEED;
			}
		}
		// Underwater?
		else {
			// Make xSpeed tend to the swim speed:
			if ( mVel.x < -SWIM_SPEED ) {
				mVel.x += SWIM_DECELERATION * dt.asSeconds();
				
				if ( mVel.x > -SWIM_SPEED )
					mVel.x = -SWIM_SPEED;
			}
			else if ( mVel.x > -SWIM_SPEED ) {
				mVel.x -= SWIM_ACCELERATION * dt.asSeconds();
				
				if ( mVel.x < -SWIM_SPEED )
					mVel.x = -SWIM_SPEED;
			}
		}
	}
	// Walk right?
	else if ( mInput->right && !mInput->left ) {
		isFacingLeft = false;
		
		// Abovewater?
		if ( !isUnderwater() || !FLOAT_ON_WATER_SURFACE ) {
			// Make xSpeed tend to the walk speed:
			if ( mVel.x > WALK_SPEED ) {
				mVel.x -= WALK_DECELERATION * dt.asSeconds();
				
				if ( mVel.x < WALK_SPEED )
					mVel.x = WALK_SPEED;
			}
			else if ( mVel.x < WALK_SPEED ) {
				mVel.x += WALK_ACCELERATION * dt.asSeconds();
				
				if ( mVel.x > WALK_SPEED )
					mVel.x = WALK_SPEED;
			}
		}
		// Underwater?
		else {
			// Make xSpeed tend to the swim speed:
			if ( mVel.x > SWIM_SPEED ) {
				mVel.x -= SWIM_DECELERATION * dt.asSeconds();
				
				if ( mVel.x < SWIM_SPEED )
					mVel.x = SWIM_SPEED;
			}
			else if ( mVel.x < SWIM_SPEED ) {
				mVel.x += SWIM_ACCELERATION * dt.asSeconds();
				
				if ( mVel.x > SWIM_SPEED )
					mVel.x = SWIM_SPEED;
			}
		}
	}
	// Neither left nor right.
	else {
		// Slow to a stop:
		// Abovewater?
		if ( !isUnderwater() || !FLOAT_ON_WATER_SURFACE ) {
			if ( mVel.x > 0 ) {
				mVel.x -= WALK_DECELERATION * dt.asSeconds();
				
				if ( mVel.x < 0 )
					mVel.x = 0;
			}
			else if ( mVel.x < 0 ) {
				mVel.x += WALK_DECELERATION * dt.asSeconds();
				
				if ( mVel.x > 0 )
					mVel.x = 0;
			}
		}
		// Underwater?
		else {
			if ( mVel.x > 0 ) {
				mVel.x -= SWIM_DECELERATION * dt.asSeconds();
				
				if ( mVel.x < 0 )
					mVel.x = 0;
			}
			else if ( mVel.x < 0 ) {
				mVel.x += SWIM_DECELERATION * dt.asSeconds();
				
				if ( mVel.x > 0 )
					mVel.x = 0;
			}
		}
	}

	// 
	// Horizontal movement:
	// 
	accDX += mVel.x * dt.asSeconds();
	float x1, y1, x2, y2;
	
	if ( accDX > 0 ) {
		// Look for collisions on the right side:
		x1 = topRightX();
		y1 = topRightY() + EPSILON;
		x2 = botRightX();
		y2 = botRightY() - EPSILON;
	}
	else {
		// Look for collisions on the left side:
		x1 = topLeftX();
		y1 = topLeftY() + EPSILON;
		x2 = botLeftX();
		y2 = botLeftY() - EPSILON;
	}
	
	float dx = (accDX < 0 ? -1.0f : 1.0f);
	while ( std::abs(accDX) > 1.0f ) {
		// Flat floor?
		if ( !mRoom->segmentCollision(
				x1 + dx, y1,
				x2 + dx, y2,
				false ) ) {
			// Just move it:
			mPos.x += dx;
			x1 += dx;
			x2 += dx;
			
			accDX -= dx;
		}
		// 26-degree slope?
		else if ( !mRoom->segmentCollision(
						x1 + dx, y1 - 0.5f,
						x2 + dx, y2 - 0.5f,
						false ) ) {
			// Move sideways and upwards, taking into account the climb factor:
			float scaledDX = dx * CLIMB_26_SLOPE_SLOWDOWN;
			mPos.x += scaledDX;
			x1 += scaledDX;
			x2 += scaledDX;
			
			float scaledDY = -CLIMB_26_SLOPE_SLOWDOWN/2;
			mPos.y += scaledDY;
			y1 += scaledDY;
			y2 += scaledDY;
			
			accDX -= dx;
		}
		// 45-degree slope?
		else if ( !mRoom->segmentCollision(
						x1 + dx, y1 - 1.0f,
						x2 + dx, y2 - 1.0f,
						false ) ) {
			// Move sideways and upwards, taking into account the climb factor:
			float scaledDX = dx * CLIMB_45_SLOPE_SLOWDOWN;
			mPos.x += scaledDX;
			x1 += scaledDX;
			x2 += scaledDX;
			
			float scaledDY = -CLIMB_45_SLOPE_SLOWDOWN;
			mPos.y += scaledDY;
			y1 += scaledDY;
			y2 += scaledDY;
			
			accDX -= dx;
		}
		// Wall.
		else {
			mVel.x = 0;
			accDX = 0;
			break;
		}
	}



	// 
	// Gravity/buoyancy:
	// 
	// Abovewater?
	if ( !isUnderwater() || !FLOAT_ON_WATER_SURFACE ) 
	{
		// Increase speed downwards if the character is not standing on the floor.
		if ( !bottomContact )
			mVel.y += GRAVITY * dt.asSeconds();
		
		// Keep speed within the allowed limit:
		if ( mVel.y > MAX_FALL_SPEED )
			mVel.y = MAX_FALL_SPEED;

		// 
		// Jumping:
		// 
		if ( mInput->jumpPress && bottomContact ) {
			// If the player presses DOWN + JUMP on a thin-floor, fall through.
			// Here, I'm going to use a trick to detect thin-floors: Since they
			// won't trigger segment collisions if I tell the Room not to detect
			// them, the collision will return false if the player is standing on a
			// thin-floor.
			if ( mInput->down &&
				!mRoom->segmentCollision(
					botLeftX(), botLeftY() + 1, botRightX(), botRightY() + 1, false ) ) {
				mPos.y += 2;
				mVel.y = 2;
			}
			else {
				// Jump!
				// Set the Y speed to the configured jump strength.
				// This will cause the character to start moving upwards.
				mVel.y = -JUMP_STRENGTH;
			}
		}
		
		// When the player stops pressing the jump button, stop the jump:
		if ( !mInput->jump && mVel.y < 0 )
			mVel.y = 0;
	}
	// Underwater?
	else {
		// Increase speed upwards if the character is not touching the ceiling.
		if ( !topContact )
			mVel.y -= BUOYANCY * dt.asSeconds();
		
		// Keep speed within the allowed limit:
		if ( mVel.y < -MAX_FLOAT_SPEED )
			mVel.y = -MAX_FLOAT_SPEED;
		
		if ( mVel.y > MAX_SINK_SPEED )
			mVel.y = MAX_SINK_SPEED;
		
		// Prevent the player from "bouncing" on the water's surface:
		// If nothing is done, the player will surpass the water's surface and
		// exit the water, then Gravity will kick in and make him fall in
		// again, and so on. This looks horrible; it's better to leave the
		// "floating" effect to the character's animation, not the game logic.
		// So, here's what I'm going to do: If the player is *about* to leave the
		// water (i.e., at the current speed and height, he'll be out of the
		// water by the next frame)...
		if ( mPos.y + mVel.y * dt.asSeconds() <= mRoom->waterLevel + SUBMERSION_HEIGHT ) {
			// Set the player's position on the water's surface, and set the
			// speed to zero so he'll stop bouncing.
			mPos.y = mRoom->waterLevel + SUBMERSION_HEIGHT + EPSILON;
			mVel.y = 0;
			
			// Now, that last change on the player's position might have
			// caused the bottom segment to collide against the floor...
			while ( botCollision() && !topCollision() ) {
				mPos.y -= EPSILON;
			}
			
			// 
			// Water-jumping:
			// 
			// So, the player is on the water's surface. Check for a water-jump
			// here.
			if ( mInput->jumpPress ) {
				// Jump!
				mVel.y = -WATER_JUMP_STRENGTH;
			}
			
		}
	}

	// 
	// Vertical movement:
	// 
	if ( mVel.y > 0 ) {
		// Fall, and check for a floor collision on the way.
		static Container<Vector2D> contacts;
		if ( mRoom->segmentCollision(
				botLeftX(), botLeftY(),
				botRightX(), botRightY(),
				0, mVel.y * dt.asSeconds(), 
				true,
				&contacts, NULL, NULL ) ) {
			// A collision was found!
			// Set the Y coordinate to that point, and reset the Y speed.
			mPos.y = contacts[0].y;
			mVel.y = 0;
		}
		else {
			// No collision was found. Move!
			mPos.y += mVel.y * dt.asSeconds();
		}
	}
	else if ( mVel.y < 0 ) {
		// Go up, and check for a ceiling collision on the way.
		static Container<Vector2D> contacts;
		if ( mRoom->segmentCollision(
				topLeftX(), topLeftY(),
				topRightX(), topRightY(),
				0, mVel.y * dt.asSeconds(),
				false,
				&contacts, NULL, NULL ) ) {
			// A collision was found!
			// Set the Y coordinate to that point, and reverse the Y speed.
			mPos.y = contacts[0].y + HEIGHT;
			mVel.y = -mVel.y / 3;
		}
		else {
			// No collision was found. Move!
			mPos.y += mVel.y * dt.asSeconds();
		}
	}

	// 
	// Stick to floor:
	// 
	// If the player was on the floor up until the last frame, and suddenly
	// found himself in the air...
	// Also, there will be no sticking to the floor when the player is
	// underwater, or he just jumped!
	if ( bottomContact && mVel.y >= 0 && (!isUnderwater() || !FLOAT_ON_WATER_SURFACE) ) {
		// Maybe he's walking down a slope.
		// Look for the floor downwards, at least as far as the X speed (thus
		// taking into account slopes of 45 degrees).
		float fDX = std::abs(mPos.x - prevPos.x);
		
		static Container<Vector2D> contacts;
		if ( mRoom->segmentCollision(
				botLeftX(),  botLeftY(),
				botRightX(), botRightY(),
				0, fDX + 1,
				true,
				&contacts, NULL, NULL ) ) {
			// Floor found! Move the player there:
			mPos.y = contacts[0].y;
		}
	}

	// 
	// Set contact status:
	// 
	topContact = mRoom->segmentCollision(
		topLeftX() , topLeftY()  - 1,
		topRightX(), topRightY() - 1,
		false );
	bottomContact = mRoom->segmentCollision(
		botLeftX() , botLeftY()  + 1,
		botRightX(), botRightY() + 1,
		false );
	leftContact = mRoom->segmentCollision(
		topLeftX()  - 1, topLeftY() + 1,
		botLeftX()  - 1, botLeftY() - 2,
		false );
	rightContact = mRoom->segmentCollision(
		topRightX() + 1, topRightY() + 1,
		botRightX() + 1, botRightY() - 2,
		false );
	
	// Still touching the floor?
	if ( bottomContact && mVel.y < 0 )
		mVel.y = 0;
	
	// Perform a "thin-floor test" now, using the bottom segment.
	// Also, allow the player to fall through the thin-floor if DOWN + JUMP was
	// pressed in this frame.
	if ( !mInput->jumpPress &&
	     !bottomContact &&
	     mVel.y >= 0 &&
		mRoom->segmentCollision(
			botLeftX() , botLeftY() - 2,
			botRightX(), botRightY() - 2,
			0, 4, true, NULL, NULL, NULL ) ) {
		bottomContact = true;
		mVel.y = 0;
	}

	// Update the Y speed to keep it consistent.
	// However, it should only be updated if it is positive (downwards) and the
	// player isn't underwater. This will ensure the player won't be "flying
	// off" slopes when he gets to their top, while still preventing the "snap"
	// that always takes place when the player walks off a ledge.
	if ( mPos.y > prevPos.y && (!isUnderwater() || !FLOAT_ON_WATER_SURFACE) ) {
		mVel.y = (mPos.y - prevPos.y) / dt.asSeconds();
	}
	
	// 
	// Collision structure:
	// 
	// Now that all the parameters have been calculated... update the collision
	// structure:
	mCollisionStruct.setX( mPos.x - WIDTH/2 );
	mCollisionStruct.setY( mPos.y - HEIGHT );
	mCollisionStruct.setWidth( WIDTH );
	mCollisionStruct.setHeight( HEIGHT );
}

void Actor::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
}

sf::FloatRect& Actor::getBounds()
{
	return sf::FloatRect(mPos.x, mPos.y,WIDTH, HEIGHT);
}
