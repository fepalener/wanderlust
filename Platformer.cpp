#include "Platformer.h"
#include "CommandQueue.h"
#include "Foreach.h"

Platformer::Platformer(const TextureManager& textures, const FontManager& fonts, Room* room)
: Entity(100)
, mSprite(textures.get(Textures::Player), sf::IntRect(0, 0, 48, 48))
, mRoom(room)
, pos(Vector2D(8,0))
, vel(Vector2D(0,0))
, isFacingLeft(false)
, mPlatformerInput(nullptr)
, mCurrentAnimation(ANIM::IDLE)
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
	platformerCollisionStruct.setX( pos.x - WIDTH/2 );
	platformerCollisionStruct.setY( pos.y - HEIGHT );
	platformerCollisionStruct.setWidth( WIDTH );
	platformerCollisionStruct.setHeight( HEIGHT );

	mPlatformerInput = PlatformerInput::Ptr(new PlatformerInput());

	//load animations
	std::unique_ptr<AnimationManager> animationManager = std::unique_ptr<AnimationManager>(new AnimationManager("../resources/character"));
	animationManager->load("character.anim");
	
	mAnim.insert(std::make_pair(ANIM::IDLE, animationManager->get("Idle")));
	mAnim.insert(std::make_pair(ANIM::RUN,  animationManager->get("Run")));
	mAnim.insert(std::make_pair(ANIM::JUMP, animationManager->get("Jump")));
	mAnim.insert(std::make_pair(ANIM::FALL, animationManager->get("Fall")));
}

unsigned int Platformer::getCategory() const
{
	return CommandCategory::PlayerShip;
}

Platformer::PlatformerInput* Platformer::getInput()
{
	return mPlatformerInput.get();
}

void Platformer::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	//target.draw(mSprite, states);

	target.draw(mAnim.find(mCurrentAnimation)->second, states);
}

void Platformer::updateAnimation(sf::Time dt)
{
	if(vel.x == 0.f) 
		mCurrentAnimation = ANIM::IDLE;
	
	if(vel.x > 0.f && !rightContact)
		mCurrentAnimation = ANIM::RUN;
	//else
	//	mCurrentAnimation = ANIM::IDLE;

	if(vel.x < 0.f && !leftContact)
		mCurrentAnimation = ANIM::RUN;
	//else
	//	mCurrentAnimation = ANIM::IDLE;

	if(!bottomContact || mPlatformerInput->jump)
		mCurrentAnimation = ANIM::FALL;

	mSprite.setPosition(std::ceilf(pos.x - (WIDTH / 2) - 12), std::ceilf(pos.y - HEIGHT - 16));
	mAnim.find(mCurrentAnimation)->second.setPosition(std::ceilf(pos.x), std::ceilf(pos.y - HEIGHT + 8));

	if(!isFacingLeft){
		mAnim.find(mCurrentAnimation)->second.setFlipH(false);
		mSprite.setTextureRect(sf::IntRect(48, 0, -48, 48));
	}
	else{
		mAnim.find(mCurrentAnimation)->second.setFlipH(true);
		mSprite.setTextureRect(sf::IntRect(0, 0, 48, 48));
	}

	mAnim.find(mCurrentAnimation)->second.update(dt);
}

void Platformer::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	// Store last frame's position.
	const Vector2D prevPos = pos;

	// 
	// Horizontal controls:
	// 
	// Walk left?
	if ( mPlatformerInput->left && !mPlatformerInput->right ) {
		isFacingLeft = true;
		
		// Abovewater?
		if ( !isUnderwater() || !FLOAT_ON_WATER_SURFACE ) {
			// Make xSpeed tend to the walk speed:
			if ( vel.x < -WALK_SPEED ) {
				vel.x += WALK_DECELERATION * dt.asSeconds();
				
				if ( vel.x > -WALK_SPEED )
					vel.x = -WALK_SPEED;
			}
			else if ( vel.x > -WALK_SPEED ) {
				vel.x -= WALK_ACCELERATION * dt.asSeconds();
				
				if ( vel.x < -WALK_SPEED )
					vel.x = -WALK_SPEED;
			}
		}
		// Underwater?
		else {
			// Make xSpeed tend to the swim speed:
			if ( vel.x < -SWIM_SPEED ) {
				vel.x += SWIM_DECELERATION * dt.asSeconds();
				
				if ( vel.x > -SWIM_SPEED )
					vel.x = -SWIM_SPEED;
			}
			else if ( vel.x > -SWIM_SPEED ) {
				vel.x -= SWIM_ACCELERATION * dt.asSeconds();
				
				if ( vel.x < -SWIM_SPEED )
					vel.x = -SWIM_SPEED;
			}
		}
	}
	// Walk right?
	else if ( mPlatformerInput->right && !mPlatformerInput->left ) {
		isFacingLeft = false;
		
		// Abovewater?
		if ( !isUnderwater() || !FLOAT_ON_WATER_SURFACE ) {
			// Make xSpeed tend to the walk speed:
			if ( vel.x > WALK_SPEED ) {
				vel.x -= WALK_DECELERATION * dt.asSeconds();
				
				if ( vel.x < WALK_SPEED )
					vel.x = WALK_SPEED;
			}
			else if ( vel.x < WALK_SPEED ) {
				vel.x += WALK_ACCELERATION * dt.asSeconds();
				
				if ( vel.x > WALK_SPEED )
					vel.x = WALK_SPEED;
			}
		}
		// Underwater?
		else {
			// Make xSpeed tend to the swim speed:
			if ( vel.x > SWIM_SPEED ) {
				vel.x -= SWIM_DECELERATION * dt.asSeconds();
				
				if ( vel.x < SWIM_SPEED )
					vel.x = SWIM_SPEED;
			}
			else if ( vel.x < SWIM_SPEED ) {
				vel.x += SWIM_ACCELERATION * dt.asSeconds();
				
				if ( vel.x > SWIM_SPEED )
					vel.x = SWIM_SPEED;
			}
		}
	}
	// Neither left nor right.
	else {
		// Slow to a stop:
		// Abovewater?
		if ( !isUnderwater() || !FLOAT_ON_WATER_SURFACE ) {
			if ( vel.x > 0 ) {
				vel.x -= WALK_DECELERATION * dt.asSeconds();
				
				if ( vel.x < 0 )
					vel.x = 0;
			}
			else if ( vel.x < 0 ) {
				vel.x += WALK_DECELERATION * dt.asSeconds();
				
				if ( vel.x > 0 )
					vel.x = 0;
			}
		}
		// Underwater?
		else {
			if ( vel.x > 0 ) {
				vel.x -= SWIM_DECELERATION * dt.asSeconds();
				
				if ( vel.x < 0 )
					vel.x = 0;
			}
			else if ( vel.x < 0 ) {
				vel.x += SWIM_DECELERATION * dt.asSeconds();
				
				if ( vel.x > 0 )
					vel.x = 0;
			}
		}
	}



	// 
	// Horizontal movement:
	// 
	accDX += vel.x * dt.asSeconds();
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
			pos.x += dx;
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
			pos.x += scaledDX;
			x1 += scaledDX;
			x2 += scaledDX;
			
			float scaledDY = -CLIMB_26_SLOPE_SLOWDOWN/2;
			pos.y += scaledDY;
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
			pos.x += scaledDX;
			x1 += scaledDX;
			x2 += scaledDX;
			
			float scaledDY = -CLIMB_45_SLOPE_SLOWDOWN;
			pos.y += scaledDY;
			y1 += scaledDY;
			y2 += scaledDY;
			
			accDX -= dx;
		}
		// Wall.
		else {
			vel.x = 0;
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
			vel.y += GRAVITY * dt.asSeconds();
		
		// Keep speed within the allowed limit:
		if ( vel.y > MAX_FALL_SPEED )
			vel.y = MAX_FALL_SPEED;

		// 
		// Jumping:
		// 
		if ( mPlatformerInput->jumpPress && bottomContact ) {
			// If the player presses DOWN + JUMP on a thin-floor, fall through.
			// Here, I'm going to use a trick to detect thin-floors: Since they
			// won't trigger segment collisions if I tell the Room not to detect
			// them, the collision will return false if the player is standing on a
			// thin-floor.
			if ( mPlatformerInput->down &&
				!mRoom->segmentCollision(
					botLeftX(), botLeftY() + 1, botRightX(), botRightY() + 1, false ) ) {
				pos.y += 2;
				vel.y = 2;
			}
			else {
				// Jump!
				// Set the Y speed to the configured jump strength.
				// This will cause the character to start moving upwards.
				vel.y = -JUMP_STRENGTH;
			}
		}
		
		// When the player stops pressing the jump button, stop the jump:
		if ( !mPlatformerInput->jump && vel.y < 0 )
			vel.y = 0;
	}
	// Underwater?
	else {
		// Increase speed upwards if the character is not touching the ceiling.
		if ( !topContact )
			vel.y -= BUOYANCY * dt.asSeconds();
		
		// Keep speed within the allowed limit:
		if ( vel.y < -MAX_FLOAT_SPEED )
			vel.y = -MAX_FLOAT_SPEED;
		
		if ( vel.y > MAX_SINK_SPEED )
			vel.y = MAX_SINK_SPEED;
		
		// Prevent the player from "bouncing" on the water's surface:
		// If nothing is done, the player will surpass the water's surface and
		// exit the water, then Gravity will kick in and make him fall in
		// again, and so on. This looks horrible; it's better to leave the
		// "floating" effect to the character's animation, not the game logic.
		// So, here's what I'm going to do: If the player is *about* to leave the
		// water (i.e., at the current speed and height, he'll be out of the
		// water by the next frame)...
		if ( pos.y + vel.y * dt.asSeconds() <= mRoom->waterLevel + SUBMERSION_HEIGHT ) {
			// Set the player's position on the water's surface, and set the
			// speed to zero so he'll stop bouncing.
			pos.y = mRoom->waterLevel + SUBMERSION_HEIGHT + EPSILON;
			vel.y = 0;
			
			// Now, that last change on the player's position might have
			// caused the bottom segment to collide against the floor...
			while ( botCollision() && !topCollision() ) {
				pos.y -= EPSILON;
			}
			
			// 
			// Water-jumping:
			// 
			// So, the player is on the water's surface. Check for a water-jump
			// here.
			if ( mPlatformerInput->jumpPress ) {
				// Jump!
				vel.y = -WATER_JUMP_STRENGTH;
			}
			
		}
	}

	// 
	// Vertical movement:
	// 
	if ( vel.y > 0 ) {
		// Fall, and check for a floor collision on the way.
		static Container<Vector2D> contacts;
		if ( mRoom->segmentCollision(
				botLeftX(), botLeftY(),
				botRightX(), botRightY(),
				0, vel.y * dt.asSeconds(), 
				true,
				&contacts, NULL, NULL ) ) {
			// A collision was found!
			// Set the Y coordinate to that point, and reset the Y speed.
			pos.y = contacts[0].y;
			vel.y = 0;
		}
		else {
			// No collision was found. Move!
			pos.y += vel.y * dt.asSeconds();
		}
	}
	else if ( vel.y < 0 ) {
		// Go up, and check for a ceiling collision on the way.
		static Container<Vector2D> contacts;
		if ( mRoom->segmentCollision(
				topLeftX(), topLeftY(),
				topRightX(), topRightY(),
				0, vel.y * dt.asSeconds(),
				false,
				&contacts, NULL, NULL ) ) {
			// A collision was found!
			// Set the Y coordinate to that point, and reverse the Y speed.
			pos.y = contacts[0].y + HEIGHT;
			vel.y = -vel.y / 3;
		}
		else {
			// No collision was found. Move!
			pos.y += vel.y * dt.asSeconds();
		}
	}

	// 
	// Stick to floor:
	// 
	// If the player was on the floor up until the last frame, and suddenly
	// found himself in the air...
	// Also, there will be no sticking to the floor when the player is
	// underwater, or he just jumped!
	if ( bottomContact && vel.y >= 0 && (!isUnderwater() || !FLOAT_ON_WATER_SURFACE) ) {
		// Maybe he's walking down a slope.
		// Look for the floor downwards, at least as far as the X speed (thus
		// taking into account slopes of 45 degrees).
		float fDX = std::abs(pos.x - prevPos.x);
		
		static Container<Vector2D> contacts;
		if ( mRoom->segmentCollision(
				botLeftX(),  botLeftY(),
				botRightX(), botRightY(),
				0, fDX + 1,
				true,
				&contacts, NULL, NULL ) ) {
			// Floor found! Move the player there:
			pos.y = contacts[0].y;
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
	if ( bottomContact && vel.y < 0 )
		vel.y = 0;
	
	// Perform a "thin-floor test" now, using the bottom segment.
	// Also, allow the player to fall through the thin-floor if DOWN + JUMP was
	// pressed in this frame.
	if ( !mPlatformerInput->jumpPress &&
	     !bottomContact &&
	     vel.y >= 0 &&
		mRoom->segmentCollision(
			botLeftX() , botLeftY() - 2,
			botRightX(), botRightY() - 2,
			0, 4, true, NULL, NULL, NULL ) ) {
		bottomContact = true;
		vel.y = 0;
	}

	// Update the Y speed to keep it consistent.
	// However, it should only be updated if it is positive (downwards) and the
	// player isn't underwater. This will ensure the player won't be "flying
	// off" slopes when he gets to their top, while still preventing the "snap"
	// that always takes place when the player walks off a ledge.
	if ( pos.y > prevPos.y && (!isUnderwater() || !FLOAT_ON_WATER_SURFACE) ) {
		vel.y = (pos.y - prevPos.y) / dt.asSeconds();
	}
	
	// 
	// Collision structure:
	// 
	// Now that all the parameters have been calculated... update the collision
	// structure:
	platformerCollisionStruct.setX( pos.x - WIDTH/2 );
	platformerCollisionStruct.setY( pos.y - HEIGHT );
	platformerCollisionStruct.setWidth( WIDTH );
	platformerCollisionStruct.setHeight( HEIGHT );

	updateAnimation(dt);

	mPlatformerInput->reset();

	Entity::updateCurrent(dt, commands);
}
