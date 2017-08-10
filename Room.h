#ifndef _Room_h_
#define _Room_h_


#include "RoomBlock.h"
#include "RoomBGLayer.h"

#include "Container.h"

#include "Matrix.h"
#include "Vector2D.h"
#include "Geom.h"

/**
* Represents the game's static environment -- obstacles and backgrounds.
* 
* Room sizes are measured in "screens" (one screen corresponds to the
* game window's dimensions).
* 
* @see Environment::loadRoom()
*/
class Room
{
public:
	typedef std::unique_ptr<Room> Ptr;

	/** This room's obstacle block size, in pixels. */
	const int blockSize;
		
	/** This room's size, in "blocks." */
	const int roomWidth, roomHeight;
		
	/** The room's background layers; the frontmost layer is stored at
	* index 0. */
	Container<RoomBGLayer *> bgLayers;
		
	/** The room's water level, measured in pixels from the room's top. A
	* water level greater than the room's pixel height indicates no water
	* at all; a negative water level indicates the entire room is filled
	* with water. */
	int waterLevel;
		
	/** The default layer where objects are inserted upon creation. Index 0
	* indicates they will be inserted in front of BG layer 0, and so on. */
	const int defaultObjLayer;
		
	/** The room's obstacle matrix. Each cell represents a "block."
	*/
	Matrix<RoomBlock::Type> obstacleLayer;
		
		
	/**
	* @return This room's width, in pixels.
	*/
	inline int getPixelWidth () {
		return obstacleLayer.columns * blockSize;
	}

	/**
	* @return This room's height, in pixels.
	*/
	inline int getPixelHeight () {
		return obstacleLayer.rows * blockSize;
	}
		
	/**
	* @return The width of one screen in this room, in pixels.
	*/
	inline int getScreenPixelWidth () {
		return roomWidth * blockSize;
	}
		
	/**
	* @return The height of one screen in this room, in pixels.
	*/
	inline int getScreenPixelHeight () {
		return roomHeight * blockSize;
	}
		
		
	/**
	* Tests the collision of a single point against the obstacle layer.
	* 
	* @return true if the specified point is colliding against the
	*         obstacle layer.
	*         The collision of a single point against "thin floors" is
	*         undefined, and therefore is not detected by this method.
	*/
	bool pointCollision ( float x, float y );
		
	/**
		* Tests the collision of a <em>moving</em> point against the obstacle
		* layer. Thus, (<tt>x1</tt>, <tt>y1</tt>) is the point's previous
		* position, and (<tt>x2</tt>, <tt>y2</tt>) is the point's current
		* position.
		* 
		* If a collision is detected between point1 and point2, its parameters
		* will be stored in the pointers <tt>contact</tt> (representing where
		* the point touched the obstacle layer) and <tt>normal</tt> (a unit
		* vector pointing away from the collision surface).
		* 
		* If <tt>touchThinFloor</tt> is false, thin floors will be ignored.
		* 
		* @return true if a collision occurred between point1 and point2.
		*/
	bool pointCollision ( float x1, float y1, float x2, float y2, bool touchThinFloor,
		                    Vector2D * contact, Vector2D * normal );
		
		
	/**
		* Tests the collision of a static segment against the obstacle layer.
		* 
		* The <tt>touchThinFloor</tt> parameter indicates whether "thin-floor"
		* blocks should be tested.
		* 
		* @return true If the segment is colliding is touching the obstacle
		*         layer.
		*/
	bool segmentCollision ( float x1, float y1, float x2, float y2, bool touchThinFloor );
		
	/**
		* Tests the collision of a moving segment against the obstacle layer.
		* The segment is specified by the pair of points ((x1, y1), (x2, y2)),
		* and is considered to have moved in the direction of the vector
		* defined by (dx, dy).
		* 
		* If <tt>touchThinFloor</tt> is false, any collisions against "thin
		* floors" will be ignored.
		*
		* Should a collision be detected in the movement, several parameters
		* are "returned" in the pointers:
		* - The contact points, which represent the points where the segment
		*   touched the obstacle layer.
		* - The corresponding normals, which represent the direction facing
		*   away from the planes of contact.
		* - The valid displacement, which indicates how much the segment can
		*   move in the specified direction before it collides.
		* 
		* Any of those may be <tt>NULL</tt>, indicating there is no need to
		* store it. (All of them will be calculated, however, as this is
		* needed for the method's execution.)
		*
		* The <tt>Container</tt>s will be emptied by invoking their
		* <tt>removeAll()</tt> method before any parameters are inserted.
		* 
		* @return true if a collision was detected.
		*/
	bool segmentCollision ( float x1, float y1, float x2, float y2, float dx, float dy, bool touchThinFloor,
		                    Container<Vector2D> * contacts, Container<Vector2D> * normals, Vector2D * validDisplacement );
		
	// Constructor: Creates an empty room.
	// Water level is placed right below the room's lower border; object layer
	// position is set to 0.
	//
	Room ( int blockSize, int roomWidth, int roomHeight, int waterLevel, int defaultObjLayer );
		
	// Destructor: Deletes all BGLayers in this room.
	virtual ~Room ();
		
};

#endif
