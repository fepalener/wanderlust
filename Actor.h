#ifndef _Actor_h_
#define _Actor_h_

#include "CollisionStruct.h"
#include "Room.h"
#include "Vector2D.h"
#include <SFML\Graphics.hpp>

class Actor : public sf::Drawable
{
public:
	class Input 
	{
	public:
		typedef std::unique_ptr<Input> Ptr;

		/** Directional input: Set to true when the LEFT/RIGHT/DOWN buttons
		* are pressed. */
		bool left, right, down;
			
		/** This should be set to true in the frame when the JUMP button is
		* pressed, and set to false in the next frame. */
		bool jumpPress;
		bool jumpPressOld;

		/** This should be set to true as long as JUMP is kept pressed. */
		bool jump;
			
		bool attack;

		/**
		 * The constructor sets all key states to false.
		 */
		Input() : left(false), right(false), down(false), jumpPress(false), jumpPressOld(false), jump(false), attack(false) {};
		void reset() {
			jumpPressOld = jumpPress;
			left = right = down = jumpPress = jump = attack = false;
		}

		virtual ~Input() {}
	};

	float WIDTH;

	float HEIGHT;

	float WALK_SPEED;

	float CLIMB_26_SLOPE_SLOWDOWN;

	float CLIMB_45_SLOPE_SLOWDOWN;
		
	float WALK_ACCELERATION;
		
	float WALK_DECELERATION;
		
	float JUMP_STRENGTH;

	float WATER_JUMP_STRENGTH;

	float GRAVITY;
		
	float MAX_FALL_SPEED;

	float SUBMERSION_HEIGHT;

	float BUOYANCY;
		
	float MAX_FLOAT_SPEED;
		
	float MAX_SINK_SPEED;
		
	float SWIM_SPEED;

	float SWIM_ACCELERATION;
		
	float SWIM_DECELERATION;
		
	bool FLOAT_ON_WATER_SURFACE;

	/** This flag is set by the <tt>update()</tt> method, indicating which
     * way the character should be facing when its sprites are rendered. */
	bool isFacingLeft;

	/** These flags are set by the <tt>update()</tt> method, and indicate
	 * which segments are touching walls, floors and ceilings.
	 * 
	 * @see rightContact, leftContact, topContact, bottomContact */
	bool rightContact, leftContact, topContact, bottomContact;

public:
					Actor(Room* room);

	virtual void	update(sf::Time dt) = 0;


	sf::FloatRect&	getBounds();

private:
	virtual void	draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	Room*					mRoom;
	float					accDX, accDT;
	Vector2D				mPos, mVel;
	CollisionStruct::Box	mCollisionStruct;
	Input::Ptr				mInput;

private:
	inline float botRightX () { return mPos.x + WIDTH/2; }
	inline float botRightY () { return mPos.y; }
	inline float topRightX () { return mPos.x + WIDTH/2; }
	inline float topRightY () { return mPos.y - HEIGHT; }
	inline float topLeftX  () { return mPos.x - WIDTH/2; }
	inline float topLeftY  () { return mPos.y - HEIGHT; }
	inline float botLeftX  () { return mPos.x - WIDTH/2; }
	inline float botLeftY  () { return mPos.y; }
		
	inline bool isUnderwater () {
		return mRoom->waterLevel > 0 ? (mPos.y > mRoom->waterLevel + SUBMERSION_HEIGHT) : false;
	}

	inline bool isFloatingOnWaterSurface () {
		if ( !FLOAT_ON_WATER_SURFACE )
			return false;
		else
		if ( std::abs(mVel.y) > EPSILON )
			return false;
		else
		if ( std::abs(mPos.y - (mRoom->waterLevel + SUBMERSION_HEIGHT)) > EPSILON )
			return false;
		else
			return true;
	}

	inline bool rightCollision () {
		return mRoom->segmentCollision(
				topRightX(), topRightY(),
				botRightX(), botRightY(),
				false ); 
	}

	inline bool leftCollision () {
		return mRoom->segmentCollision(
				topLeftX(), topLeftY(),
				botLeftX(), botLeftY(),
				false );
	}

	inline bool topCollision () {
		return mRoom->segmentCollision(
				topLeftX(),  topLeftY(),
				topRightX(), topRightY(),
				false );
	}

	inline bool botCollision () {
		return mRoom->segmentCollision(
				botLeftX(),  botLeftY(),
				botRightX(), botRightY(),
				false );
	}

};

#endif