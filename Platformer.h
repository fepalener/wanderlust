#ifndef _Platformer_h_
#define _Platformer_h_

#include "Entity.h"
#include "Command.h"
#include "ResourceIdentifiers.h"
#include "ResourceManager.h"
#include "Vector2D.h"
#include "CollisionStruct.h"
#include "Room.h"
#include "Anim.h"
#include "AnimationManager.h"

#include <SFML\Graphics.hpp>
#include <map>

/**
	 *           ___
	 * topLeft  |   | topRight
	 *          |   |
	 *          |   |
	 *  botLeft |___| botRight
	 *            ^
	 *           pos
	 */

class Platformer : public Entity
{
public:

	class PlatformerInput 
	{
	public:
		typedef std::unique_ptr<PlatformerInput> Ptr;

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
		PlatformerInput() : left(false), right(false), down(false), jumpPress(false), jumpPressOld(false), jump(false), attack(false) {};
		void reset() {
			jumpPressOld = jumpPress;
			left = right = down = jumpPress = jump = attack = false;
		}

		virtual ~PlatformerInput() {}
	};

	/** The character's width, in pixels.
		* @see HEIGHT */
	float WIDTH;

	/** The character's height, in pixels.
		* @see WIDTH */
	float HEIGHT;

	/** The character's maximum walk speed, in pixels/second. 
		* (Specify a number greater than zero; the object will adjust the sign
		* as needed.) */
	float WALK_SPEED;

	/** This factor is used to slow down the character when he is walking
		* up a slope with 26-degree inclination. Specify a number in the range
		* [0, 1].
		* @code
		* CLIMB_26_SLOPE_SLOWDOWN = 1.0; // Walks at normal speed when climbing 26-degree slopes
		* @endcode
		* 
		* @see CLIMB_45_SLOPE_SLOWDOWN
		*/
	float CLIMB_26_SLOPE_SLOWDOWN;

	/** This factor is used to slow down the character when he is walking
		* up a slope with 45-degree inclination. Specify a number in the range
		* [0, 1].
		* @code
		* CLIMB_45_SLOPE_SLOWDOWN = 0.5; // Walks at half speed when climbing 45-degree slopes
		* @endcode
		* 
		* @see CLIMB_26_SLOPE_SLOWDOWN
		*/
	float CLIMB_45_SLOPE_SLOWDOWN;
		
	/** The acceleration applied to the X speed as the player keeps
		* pressing the walk keys, in pixels/second<sup>2</sup>. (Specify
		* numbers greater than zero; the object will adjust the sign as
		* needed.)
		*
		* @see WALK_DECELERATION */
	float WALK_ACCELERATION;
		
	/** The deceleration applied to the X speed as the player releases
		* walk keys, in pixels/second<sup>2</sup>. (Specify numbers greater
		* than zero; the object will adjust the sign as needed.)
		*
		* @see WALK_ACCELERATION */
	float WALK_DECELERATION;
		
	/** The Y speed applied when the player jumps on the ground, in
		* pixels/second. (Specify a number greater than zero; the object will
		* adjust the sign as needed.)
		* 
		* @see WATER_JUMP_STRENGTH */
	float JUMP_STRENGTH;

	/** The Y speed applied when the player jumps on the water's surface,
		* in pixels/second. (Specify a number greater than zero; the object
		* will adjust the sign as needed.)
		* 
		* @see JUMP_STRENGTH */
	float WATER_JUMP_STRENGTH;

	/** The acceleration applied to the Y speed when the player is falling,
		* in pixels/second<sup>2</sup>. */
	float GRAVITY;
		
	/** When falling, the Y speed will be clamped to this value. */
	float MAX_FALL_SPEED;

	/** This indicates how much of the character will be submerged when
		* floating on the water's surface.
		*
		* @code
		*      ____
		*     |    |
		* ~~~~~~~~~~~~~~~~ /
		*     |    |       |
		*     |    |       | Subm. height
		*     |    |       |
		*     |____|       /
		* 
		* @endcode
		*/
	float SUBMERSION_HEIGHT;

	/** "Reverse gravity" applied to the character when underwater, in
		* pixels/second<sup>2</sup>.
		* (Specify a number greater than zero; the object will adjust the
		* sign as needed.) */
	float BUOYANCY;
		
	/** When floating upwards the Y speed will be clamped to this value.
		* (Specify a number greater than zero; the object will adjust the
		* sign as needed.)
		* 
		* @see MAX_SINK_SPEED */
	float MAX_FLOAT_SPEED;
		
	/** When sinking downwards the Y speed will be clamped to this value.
		* (Specify a number greater than zero; the object will adjust the
		* sign as needed.)
		* 
		* @see MAX_FLOAT_SPEED */
	float MAX_SINK_SPEED;
		
	/** The character's maximum swimming X speed, in pixels/second.
		* (Specify a number greater than zero; the object will adjust the
		* sign as needed.)
		*/
	float SWIM_SPEED;

	/** The acceleration applied to the X speed as the player keeps
		* pressing the walk keys while underwater, in
		* pixels/second<sup>2</sup>. (Specify numbers greater than zero; the
		* object will adjust the signs as needed.)
		* 
		* @see SWIM_DECELERATION */
	float SWIM_ACCELERATION;
		
	/** The deceleration applied to the X speed as the player releases
		* walk keys while underwater, in pixels/second<sup>2</sup>. (Specify
		* numbers greater than zero; the object will adjust the signs as
		* needed.)
		* 
		* @see SWIM_ACCELERATION */
	float SWIM_DECELERATION;
		
	/** You may turn off the underwater mechanics by setting this flag to
		* false. The character will behave as though there were no water at
		* all. */
	bool FLOAT_ON_WATER_SURFACE;

	/**
	 * Returns a collision structure delimited by the platformerObj's
	 * position, width and height.
	 */
	CollisionStruct* getCollisionStruct();

	/** This flag is set by the <tt>update()</tt> method, indicating which
     * way the character should be facing when its sprites are rendered. */
	bool isFacingLeft;

	/** These flags are set by the <tt>update()</tt> method, and indicate
	 * which segments are touching walls, floors and ceilings.
	 * 
	 * @see rightContact, leftContact, topContact, bottomContact */
	bool rightContact, leftContact, topContact, bottomContact;

							Platformer(const TextureManager& textures, const FontManager& fonts, Room* room);
	virtual unsigned int	getCategory() const;
	PlatformerInput*		getInput();
	Vector2D				getPos() const {return pos;};

	/**
	 * Returns true if the platformer is submerged.
	 */
	inline bool isUnderwater () {
		return mRoom->waterLevel > 0 ? (pos.y > mRoom->waterLevel + SUBMERSION_HEIGHT) : false;
	}

	/**
	 * Returns true if the platformer is floating on the water's surface.
	 */
	inline bool isFloatingOnWaterSurface () {
		if ( !FLOAT_ON_WATER_SURFACE )
			return false;
		else
		if ( std::abs(vel.y) > EPSILON )
			return false;
		else
		if ( std::abs(pos.y - (mRoom->waterLevel + SUBMERSION_HEIGHT)) > EPSILON )
			return false;
		else
			return true;
	}

private:
	virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void 			updateCurrent(sf::Time dt, CommandQueue& commands);
	virtual void			updateAnimation(sf::Time dt);

private:
	enum ANIM
	{
		IDLE,
		JUMP,
		FALL,
		DAMAGE,
		DEATH,
		RUN
	};

	PlatformerInput::Ptr	mPlatformerInput;

	float					accDX, accDT;
	Vector2D				pos, vel;
	CollisionStruct::Box	platformerCollisionStruct;

	sf::Sprite				mSprite;
	Room*					mRoom;
	std::map<ANIM, Anim>	mAnim;

	ANIM					mCurrentAnimation;

	inline float botRightX () { return pos.x + WIDTH/2; }
	inline float botRightY () { return pos.y; }
	inline float topRightX () { return pos.x + WIDTH/2; }
	inline float topRightY () { return pos.y - HEIGHT; }
	inline float topLeftX  () { return pos.x - WIDTH/2; }
	inline float topLeftY  () { return pos.y - HEIGHT; }
	inline float botLeftX  () { return pos.x - WIDTH/2; }
	inline float botLeftY  () { return pos.y; }
		
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