#include "Room.h"

#include <cmath>

// 
// Block coordinates:
// 
#define BLOCK_N_X  (blockSize/2)
#define BLOCK_N_Y  0

#define BLOCK_NE_X (blockSize)
#define BLOCK_NE_Y 0

#define BLOCK_E_X  (blockSize)
#define BLOCK_E_Y  (blockSize/2)

#define BLOCK_SE_X (blockSize)
#define BLOCK_SE_Y (blockSize)

#define BLOCK_S_X  (blockSize/2)
#define BLOCK_S_Y  (blockSize)

#define BLOCK_SW_X 0
#define BLOCK_SW_Y (blockSize)

#define BLOCK_W_X  0
#define BLOCK_W_Y  (blockSize/2)

#define BLOCK_NW_X 0
#define BLOCK_NW_Y 0

#define BLOCK_CENTER_X (blockSize/2)
#define BLOCK_CENTER_Y (blockSize/2)


///////////////////////////////////////////////////////////////////////////////
// 
// Auxiliary functions:
// 
///////////////////////////////////////////////////////////////////////////////

// Given a vector (x, y) will return its angle in degrees.
//                                                 
//         90                            
//         |                             
//         |                             
// 180 ----+---- 0
//         |                             
//         |                             
//        270                            
//                                                  
static inline float degreeAngle ( float x, float y ) {
	if ( abs(x) < EPSILON && abs(y) < EPSILON )
		return 0;
	
	else if ( abs(x) < EPSILON ) {
		if ( y > 0 )
			return 270;
		else
			return 90;
	}
	
	else if ( abs(y) < EPSILON ) {
		if ( x < 0 )
			return 180;
		else
			return 0;
	}
	
	else {
		// Normalize the vector:
		float norm = sqrtf( x * x + y * y );
		x /= norm;
		
		// Dot product against the X vector (1, 0):
		// float dot = x * 1 + y * 0;
		// This is the cosine of the angle.
		
		float radians = acosf( x );
		float degrees = ((180.0f / 3.1415926535898f) * radians);
		
		// Vector in 3rd or 4th quadrant?
		if ( y > 0 )
			degrees = 360 - degrees;
		
		while ( degrees > 360 )
			degrees -= 360;
		
		while ( degrees < 0 )
			degrees += 360;
		
		return degrees;
	}	
} // End of function: degreeAngle




// Compares numbers with an error margin:
inline static bool marginCmp ( float val1, float val2, float error ) {
	return ( abs(val1 - val2) <= abs(error) );
}




///////////////////////////////////////////////////////////////////////////////
// 
// Public methods:
// 
///////////////////////////////////////////////////////////////////////////////

Room::Room ( int _blockSize, int _roomWidth, int _roomHeight, int _waterLevel, int _defaultObjLayer )
	// Initialize members:
	: blockSize(_blockSize),
	  roomWidth(_roomWidth),
	  roomHeight(_roomHeight),
	  obstacleLayer(_roomHeight, _roomWidth),
	  waterLevel( _waterLevel ),
	  defaultObjLayer( _defaultObjLayer )
{
	// Set the water level to huge numbers.
	if ( waterLevel < 0 ) {
		waterLevel = -10000;
	}
	else if ( waterLevel > getPixelHeight() ) {
		waterLevel = getPixelHeight() + 10000;
	}
}




Room::~Room () {
	// Delete all BGLayers:
	
	while ( bgLayers.getCount() > 0 ) {
		delete bgLayers[0];
		bgLayers.remove( 0 );
	}
}





bool Room::pointCollision ( float x, float y ) {
	// Find the obstacle block at (x, y):
	int i = (int) (y / blockSize);
	int j = (int) (x / blockSize);
	
	// Outside of matrix?
	if ( i < 0 || i >= obstacleLayer.rows ||
	     j < 0 || j >= obstacleLayer.columns ) {
		// Not colliding!
		return false;
	}
	else {
		// Transform the (x, y) coordinates so they're relative to the block's
		// top-left corner:
		x -= j * blockSize;
		y -= i * blockSize;
		
		// Get the obstacle block, and test it:
		switch ( obstacleLayer.cell(i, j) ) {
			case RoomBlock::BLK_SE_BLOCK:
				return (x > BLOCK_CENTER_X && y > BLOCK_CENTER_Y);
				
			case RoomBlock::BLK_W_E_BOTTOM:
				return (y > BLOCK_CENTER_Y);
				
			case RoomBlock::BLK_SW_BLOCK:
				return (x < BLOCK_CENTER_X && y > BLOCK_CENTER_Y);
				
			case RoomBlock::BLK_NW_MISSING:
				return !(x <= BLOCK_CENTER_X && y <= BLOCK_CENTER_Y);
				
			case RoomBlock::BLK_NE_MISSING:
				return !(x >= BLOCK_CENTER_X && y <= BLOCK_CENTER_Y);
				
			case RoomBlock::BLK_SW_NE_BOTTOM:
				return (y > (-x + blockSize));
				
			case RoomBlock::BLK_NW_SE_BOTTOM:
				return (y > x);
				
			case RoomBlock::BLK_SW_E_BOTTOM:
				return (y > (-x/2 + blockSize));
				
			case RoomBlock::BLK_W_NE_BOTTOM:
				return (y > (-x/2 + (float) blockSize/2));
				
			case RoomBlock::BLK_NW_E_BOTTOM:
				return (y > (x/2));
				
			case RoomBlock::BLK_W_SE_BOTTOM:
				return (y > (x/2 + (float) blockSize/2));
				
			case RoomBlock::BLK_N_S_RIGHT:
				return (x > BLOCK_CENTER_X);
				
			case RoomBlock::BLK_FULL:
				return true;
				
			case RoomBlock::BLK_N_S_LEFT:
				return (x < BLOCK_CENTER_X);
				
			case RoomBlock::BLK_SW_MISSING:
				return !(x <= BLOCK_CENTER_X && y >= BLOCK_CENTER_Y);
				
			case RoomBlock::BLK_SE_MISSING:
				return !(x >= BLOCK_CENTER_X && y >= BLOCK_CENTER_Y);
				
			case RoomBlock::BLK_NW_SE_TOP:
				return (y < x);
				
			case RoomBlock::BLK_SW_NE_TOP:
				return (y < -x + blockSize);
				
			case RoomBlock::BLK_NW_E_TOP:
				return (y < x/2);
				
			case RoomBlock::BLK_W_SE_TOP:
				return (y < (x/2 + (float) blockSize/2));
				
			case RoomBlock::BLK_SW_E_TOP:
				return (y < (-x/2 + blockSize));
				
			case RoomBlock::BLK_W_NE_TOP:
				return (y < (-x/2 + (float) blockSize/2));
				
			case RoomBlock::BLK_NE_BLOCK:
				return (x > BLOCK_CENTER_X && y < BLOCK_CENTER_Y);
				
			case RoomBlock::BLK_W_E_TOP:
				return (y < BLOCK_CENTER_Y);
				
			case RoomBlock::BLK_NW_BLOCK:
				return (x < BLOCK_CENTER_X && y < BLOCK_CENTER_Y);
				
			case RoomBlock::BLK_S_E_BOTTOM:
				return (y > -x + 3.0f * blockSize / 2.0f);
				
			case RoomBlock::BLK_W_N_BOTTOM:
				return (y > -x + (float) blockSize / 2);
				
			case RoomBlock::BLK_N_E_BOTTOM:
				return (y > x - (float) blockSize / 2);
				
			case RoomBlock::BLK_W_S_BOTTOM:
				return (y > x + (float) blockSize / 2);
				
			case RoomBlock::BLK_N_E_TOP:
				return (y < x - (float) blockSize / 2);
				
			case RoomBlock::BLK_W_S_TOP:
				return (y < x + (float) blockSize / 2);
				
			case RoomBlock::BLK_S_E_TOP:
				return (y < -x + 3.0f * blockSize / 2.0f);
				
			case RoomBlock::BLK_W_N_TOP:
				return (y < -x + (float) blockSize / 2.0f);
				
			case RoomBlock::BLK_THINFLOOR_HI:
				return false;
				
			case RoomBlock::BLK_THINFLOOR_MID:
				return false;
				
			case RoomBlock::BLK_THINFLOOR_LO:
				return false;
				
			default:
				return false;
		}
	}
} // End of method: Room::pointCollision (static point)






bool Room::pointCollision ( float x1, float y1, float x2, float y2, bool touchThinFloor,
                            Vector2D * contact, Vector2D * normal ) {

	// Maybe, point1 is already colliding...
	if ( pointCollision( x1, y1 ) ) {
		// Set it as the contact point:
		if ( contact ) {
			contact->x = x1;
			contact->y = y1;
		}
		
		// For the normal, use the point2 -> point1 vector:
		if ( normal ) {
			normal->x = x1 - x2;
			normal->y = y1 - y2;
			normal->normalize();
		}
		
		return true;
	}
	
	// If the point hasn't moved, there was no collision:
	if ( x1 == x2 && y1 == y2 )
		return false;
	
	// The movement's angle relative to the X axis, in degrees:
	float angle = degreeAngle( x2 - x1, y2 - y1 );
	
	bool leftToRight = (x1 < x2);
	bool rightToLeft = (x1 > x2);
	bool topToBottom = (y1 < y2);
	bool bottomToTop = (y1 > y2);
	
	// Point2's block: The loop will end when we reach it.
	int iFinal = (int) floorf( y2 / blockSize );
	int jFinal = (int) floorf( x2 / blockSize );
	
	// Start out at point1's block:
	int i = (int) floorf( y1 / blockSize );
	int j = (int) floorf( x1 / blockSize );
	
	while ( true ) {
		// The current block's top-left coordinates:
		float blkX = (float) j * blockSize;
		float blkY = (float) i * blockSize;
		
		// Index inside of matrix?
		if ( i >= 0 && i < obstacleLayer.rows &&
		     j >= 0 && j < obstacleLayer.columns ) {
			// Extend a line from point1 to point2, and test for intersection
			// against the obstacle layer:
			switch ( obstacleLayer.cell(i, j) ) {
				case RoomBlock::BLK_SE_BLOCK:
					// Top segment:
					if ( (angle > 180 && angle < 360) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
					                   blkX + BLOCK_E_X,      blkY + BLOCK_E_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					// Left segment:
					else 
					if ( ((angle >= 0 && angle < 90) || (angle > 270 && angle < 360)) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
					                   blkX + BLOCK_S_X,      blkY + BLOCK_S_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 0;
						}
						
						return true;
					}
					// Bottom segment:
					else 
					if ( (angle > 0 && angle < 180) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_S_X,  blkY + BLOCK_S_Y,
					                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = 1;
						}
						
						return true;
					}
					// Right segment:
					else 
					if ( (angle > 90 && angle < 270) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
					                   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
				case RoomBlock::BLK_W_E_BOTTOM:
					// Top segment:
					if ( (angle > 180 && angle < 360) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_W_X, blkY + BLOCK_W_Y,
					                   blkX + BLOCK_E_X, blkY + BLOCK_E_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					// Left segment:
					else 
					if ( ((angle >= 0 && angle < 90) || (angle > 270 && angle < 360)) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
					                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 0;
						}
						
						return true;
					}
					// Bottom segment:
					else 
					if ( (angle > 0 && angle < 180) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
					                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = 1;
						}
						
						return true;
					}
					// Right segment:
					else 
					if ( (angle > 90 && angle < 270) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
					                   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
				case RoomBlock::BLK_SW_BLOCK:
					// Top segment:
					if ( (angle > 180 && angle < 360) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_W_X,      blkY + BLOCK_W_Y,
					                   blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					// Left segment:
					else 
					if ( ((angle >= 0 && angle < 90) || (angle > 270 && angle < 360)) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
					                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 0;
						}
						
						return true;
					}
					// Bottom segment:
					else 
					if ( (angle > 0 && angle < 180) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
					                   blkX + BLOCK_S_X,  blkY + BLOCK_S_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = 1;
						}
						
						return true;
					}
					// Right segment:
					else 
					if ( (angle > 90 && angle < 270) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_S_X,      blkY + BLOCK_S_Y,
					                   blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
				case RoomBlock::BLK_NW_MISSING: {
					// This is a lot more complicated, since it's a non-convex
					// polygon.
					
					// Store all the intersections and their normals in a list:
					static Container<Vector2D> contacts;
					static Container<Vector2D> normals;
					
					contacts.removeAll();
					normals.removeAll();
					
					static Vector2D tmpContact;
					
					// Top segments:
					if ( angle > 180 && angle < 360 ) {
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_N_X,  blkY + BLOCK_N_Y,
						                   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(0, -1) );
						}
						
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_W_X,      blkY + BLOCK_W_Y,
						                   blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(0, -1) );
						}
					}
					
					// Left segments:
					if ( (angle >= 0 && angle < 90) || (angle > 270 && angle < 360) ) {
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
						                   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(-1, 0) );
						}
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_CENTER_X,  blkY + BLOCK_CENTER_Y,
						                   blkX + BLOCK_N_X,       blkY + BLOCK_N_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(-1, 0) );
						}
					}
					// Bottom segment:
					if ( angle > 0 && angle < 180 ) {
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
						                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(0, 1) );
						}
					}
					// Right segment:
					if ( angle > 90 && angle < 270 ) {
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
						                   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(1, 0) );
						}
					}
					
					
					// Have we found *any* contacts?
					if ( contacts.getCount() > 0 ) {
						
						// Find the closest contact point to point1:
						Vector2D p1 = Vector2D( x1, y1 );
						
						int   index = 0;
						float dist  = (contacts[0] - p1).length();
						
						int pInx;
						for ( pInx = 1; pInx < contacts.getCount(); pInx++ ) {
							float newDist = (contacts[pInx] - p1).length();
							
							if ( newDist < dist ) {
								index = pInx;
								dist  = newDist;
							}
						}
						
						// Return the parameters:
						if ( contact )
							*contact = contacts[index];
						if ( normal )
							*normal = normals[index];
						
						return true;
					}
					// No collision:
					else
						break;
				}
				
					
				case RoomBlock::BLK_NE_MISSING: {
					// This is a lot more complicated, since it's a non-convex
					// polygon.
					
					// Store all the intersections and their normals in a list:
					static Container<Vector2D> contacts;
					static Container<Vector2D> normals;
					
					contacts.removeAll();
					normals.removeAll();
					
					static Vector2D tmpContact;
					
					// Top segments:
					if ( angle > 180 && angle < 360 ) {
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						                   blkX + BLOCK_N_X,  blkY + BLOCK_N_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(0, -1) );
						}
						
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
						                   blkX + BLOCK_E_X,      blkY + BLOCK_E_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(0, -1) );
						}
					}
					
					// Left segment:
					if ( (angle >= 0 && angle < 90) || (angle > 270 && angle < 360) ) {
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(-1, 0) );
						}
					}
					// Bottom segment:
					if ( angle > 0 && angle < 180 ) {
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
						                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(0, 1) );
						}
					}
					// Right segments:
					if ( angle > 90 && angle < 270 ) {
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_N_X,      blkY + BLOCK_N_Y,
						                   blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(1, 0) );
						}
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
						                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(1, 0) );
						}
					}
					
					
					// Have we found *any* contacts?
					if ( contacts.getCount() > 0 ) {
						
						// Find the closest contact point to point1:
						Vector2D p1 = Vector2D( x1, y1 );
						
						int   index = 0;
						float dist  = (contacts[0] - p1).length();
						
						for ( int pInx = 1; pInx < contacts.getCount(); pInx++ ) {
							float newDist = (contacts[pInx] - p1).length();
							
							if ( newDist < dist ) {
								index = pInx;
								dist  = newDist;
							}
						}
						
						// Return the parameters:
						if ( contact )
							*contact = contacts[index];
						if ( normal )
							*normal = normals[index];
						
						return true;
					}
					// No collision:
					else
						break;
				}
					

				case RoomBlock::BLK_SW_NE_BOTTOM:
					// 45 top-left oblique segment:
					if ( ((angle >= 0 && angle < 45) || (angle > 225 && angle < 360)) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
					                   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = -1;
							normal->normalize();
						}
						
						return true;
					}
					// Bottom segment:
					else 
					if ( (angle > 0 && angle < 180) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
					                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = 1;
						}
						
						return true;
					}
					// Right segment:
					else 
					if ( (angle > 90 && angle < 270) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
					                   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_NW_SE_BOTTOM:
					// 45 top-right oblique segment:
					if ( (angle > 135 && angle < 315) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
					                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = -1;
							normal->normalize();
						}
						
						return true;
					}
					// Bottom segment:
					else 
					if ( (angle > 0 && angle < 180) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
					                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = 1;
						}
						
						return true;
					}
					// Left segment:
					else 
					if ( ((angle >= 0 && angle < 90) || (angle > 270 && angle < 360)) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
					                   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
				
				
				case RoomBlock::BLK_SW_E_BOTTOM:
					// 26 top-left oblique segment:
					if ( ((angle >= 0 && angle < 26) || (angle > 207 && angle < 360)) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
					                   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = -2;
							normal->normalize();
						}
						
						return true;
					}
					// Bottom segment:
					else 
					if ( (angle > 0 && angle < 180) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
					                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = 1;
						}
						
						return true;
					}
					// Right segment:
					else 
					if ( (angle > 90 && angle < 270) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
					                   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_W_NE_BOTTOM:
					// 26 top-left oblique segment:
					if ( ((angle >= 0 && angle < 26) || (angle > 207 && angle < 360)) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
					                   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = -2;
							normal->normalize();
						}
						
						return true;
					}
					// Left segment:
					else 
					if ( ((angle >= 0 && angle < 90) || (angle > 270 && angle < 360)) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
					                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 0;
						}
						
						return true;
					}
					// Bottom segment:
					else 
					if ( (angle > 0 && angle < 180) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
					                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = 1;
						}
						
						return true;
					}
					// Right segment:
					else 
					if ( (angle > 90 && angle < 270) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
					                   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_NW_E_BOTTOM:
					// 26 top-right oblique segment:
					if ( (angle > 153 && angle < 334) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
					                   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = -2;
							normal->normalize();
						}
						
						return true;
					}
					// Left segment:
					else 
					if ( ((angle >= 0 && angle < 90) || (angle > 270 && angle < 360)) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
					                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 0;
						}
						
						return true;
					}
					// Bottom segment:
					else 
					if ( (angle > 0 && angle < 180) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
					                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = 1;
						}
						
						return true;
					}
					// Right segment:
					else 
					if ( (angle > 90 && angle < 270) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
					                   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_W_SE_BOTTOM:
					// 26 top-right oblique segment:
					if ( (angle > 153 && angle < 334) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
					                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = -2;
							normal->normalize();
						}
						
						return true;
					}
					// Left segment:
					else 
					if ( ((angle >= 0 && angle < 90) || (angle > 270 && angle < 360)) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
					                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 0;
						}
						
						return true;
					}
					// Bottom segment:
					else 
					if ( (angle > 0 && angle < 180) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
					                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = 1;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_N_S_RIGHT:
					// Top segment:
					if ( (angle > 180 && angle < 360) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_N_X,  blkY + BLOCK_N_Y,
					                   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					// Left segment:
					else 
					if ( ((angle >= 0 && angle < 90) || (angle > 270 && angle < 360)) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_N_X, blkY + BLOCK_N_Y,
					                   blkX + BLOCK_S_X, blkY + BLOCK_S_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 0;
						}
						
						return true;
					}
					// Bottom segment:
					else 
					if ( (angle > 0 && angle < 180) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_S_X,  blkY + BLOCK_S_Y,
					                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = 1;
						}
						
						return true;
					}
					// Right segment:
					else 
					if ( (angle > 90 && angle < 270) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
					                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_FULL:
					// Top segment:
					if ( (angle > 180 && angle < 360) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
					                   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					// Left segment:
					else 
					if ( ((angle >= 0 && angle < 90) || (angle > 270 && angle < 360)) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
					                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 0;
						}
						
						return true;
					}
					// Bottom segment:
					else 
					if ( (angle > 0 && angle < 180) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
					                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = 1;
						}
						
						return true;
					}
					// Right segment:
					else 
					if ( (angle > 90 && angle < 270) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
					                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_N_S_LEFT:
					// Top segment:
					if ( (angle > 180 && angle < 360) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
					                   blkX + BLOCK_N_X,  blkY + BLOCK_N_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					// Left segment:
					else 
					if ( ((angle >= 0 && angle < 90) || (angle > 270 && angle < 360)) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
					                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 0;
						}
						
						return true;
					}
					// Bottom segment:
					else 
					if ( (angle > 0 && angle < 180) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
					                   blkX + BLOCK_S_X,  blkY + BLOCK_S_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = 1;
						}
						
						return true;
					}
					// Right segment:
					else 
					if ( (angle > 90 && angle < 270) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_N_X, blkY + BLOCK_N_Y,
					                   blkX + BLOCK_S_X, blkY + BLOCK_S_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_SW_MISSING: {
					// This is a lot more complicated, since it's a non-convex
					// polygon.
					
					// Store all the intersections and their normals in a list:
					static Container<Vector2D> contacts;
					static Container<Vector2D> normals;
					
					contacts.removeAll();
					normals.removeAll();
					
					static Vector2D tmpContact;
					
					// Top segment:
					if ( angle > 180 && angle < 360 ) {
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						                   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(0, -1) );
						}
					}
					
					// Left segments:
					if ( (angle >= 0 && angle < 90) || (angle > 270 && angle < 360) ) {
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						                   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(-1, 0) );
						}
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
						                   blkX + BLOCK_S_X,      blkY + BLOCK_S_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(-1, 0) );
						}
					}
					// Bottom segments:
					if ( angle > 0 && angle < 180 ) {
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_W_X,      blkY + BLOCK_W_Y,
						                   blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(0, 1) );
						}
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_S_X,  blkY + BLOCK_S_Y,
						                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(0, 1) );
						}
					}
					// Right segment:
					if ( angle > 90 && angle < 270 ) {
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
						                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(1, 0) );
						}
					}
					
					
					// Have we found *any* contacts?
					if ( contacts.getCount() > 0 ) {
						
						// Find the closest contact point to point1:
						Vector2D p1 = Vector2D( x1, y1 );
						
						int   index = 0;
						float dist  = (contacts[0] - p1).length();
						
						for ( int pInx = 1; pInx < contacts.getCount(); pInx++ ) {
							float newDist = (contacts[pInx] - p1).length();
							
							if ( newDist < dist ) {
								index = pInx;
								dist  = newDist;
							}
						}
						
						// Return the parameters:
						if ( contact )
							*contact = contacts[index];
						if ( normal )
							*normal = normals[index];
						
						return true;
					}
					// No collision:
					else
						break;
				}
					
					
				case RoomBlock::BLK_SE_MISSING: {
					// This is a lot more complicated, since it's a non-convex
					// polygon.
					
					// Store all the intersections and their normals in a list:
					static Container<Vector2D> contacts;
					static Container<Vector2D> normals;
					
					contacts.removeAll();
					normals.removeAll();
					
					static Vector2D tmpContact;
					
					// Top segment:
					if ( angle > 180 && angle < 360 ) {
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						                   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(0, -1) );
						}
					}
					
					// Left segment:
					if ( (angle >= 0 && angle < 90) || (angle > 270 && angle < 360) ) {
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(-1, 0) );
						}
					}
					// Bottom segments:
					if ( angle > 0 && angle < 180 ) {
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
						                   blkX + BLOCK_S_X,  blkY + BLOCK_S_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(0, 1) );
						}
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
						                   blkX + BLOCK_E_X,      blkY + BLOCK_E_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(0, 1) );
						}
					}
					// Right segments:
					if ( angle > 90 && angle < 270 ) {
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
						                   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(1, 0) );
						}
						if ( Geom::segmentIntersection( x1, y1,
						                   x2, y2,
						                   blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
						                   blkX + BLOCK_S_X,      blkY + BLOCK_S_Y,
						                   &tmpContact ) ) {
							contacts.add( tmpContact );
							normals.add( Vector2D(1, 0) );
						}
					}
					
					
					// Have we found *any* contacts?
					if ( contacts.getCount() > 0 ) {
						
						// Find the closest contact point to point1:
						Vector2D p1 = Vector2D( x1, y1 );
						
						int   index = 0;
						float dist  = (contacts[0] - p1).length();
						
						for ( int pInx = 1; pInx < contacts.getCount(); pInx++ ) {
							float newDist = (contacts[pInx] - p1).length();
							
							if ( newDist < dist ) {
								index = pInx;
								dist  = newDist;
							}
						}
						
						// Return the parameters:
						if ( contact )
							*contact = contacts[index];
						if ( normal )
							*normal = normals[index];
						
						return true;
					}
					// No collision:
					else
						break;
				}
					
					
				case RoomBlock::BLK_NW_SE_TOP:
					// 45 bott-left oblique segment:
					if ( ((angle >= 0 && angle < 135) || (angle > 315 && angle < 360)) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						               blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 1;
							normal->normalize();
						}
						
						return true;
					}
					// Top segment:
					else 
					if ( (angle > 180 && angle < 360) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						               blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					// Right segment:
					else 
					if ( (angle > 90 && angle < 270) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
						               blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_SW_NE_TOP:
					// 45 bott-right oblique segment:
					if ( (angle > 45 && angle < 225) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
						               blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 1;
							normal->normalize();
						}
						
						return true;
					}
					// Top segment:
					else 
					if ( (angle > 180 && angle < 360) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						               blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					// Left segment:
					else 
					if ( ((angle >= 0 && angle < 90) || (angle > 270 && angle < 360)) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
						               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_NW_E_TOP:
					// 26 bott-left oblique segment:
					if ( ((angle >= 0 && angle < 153) || (angle > 334 && angle < 360)) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						               blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 2;
							normal->normalize();
						}
						
						return true;
					}
					// Top segment:
					else 
					if ( (angle > 180 && angle < 360) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						               blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					// Right segment:
					else 
					if ( (angle > 90 && angle < 270) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
						               blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_W_SE_TOP:
					// 26 bott-left oblique segment:
					if ( ((angle >= 0 && angle < 153) || (angle > 334 && angle < 360)) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
						               blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 2;
							normal->normalize();
						}
						
						return true;
					}
					// Top segment:
					else 
					if ( (angle > 180 && angle < 360) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						               blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					// Left segment:
					else 
					if ( ((angle >= 0 && angle < 90) || (angle > 270 && angle < 360)) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						               blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 0;
						}
						
						return true;
					}
					// Right segment:
					else 
					if ( (angle > 90 && angle < 270) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
						               blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_SW_E_TOP:
					// 26 bott-right oblique segment:
					if ( (angle > 26 && angle < 207) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
						               blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 2;
							normal->normalize();
						}
						
						return true;
					}
					// Top segment:
					else 
					if ( (angle > 180 && angle < 360) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						               blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					// Left segment:
					else 
					if ( ((angle >= 0 && angle < 90) || (angle > 270 && angle < 360)) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						               blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 0;
						}
						
						return true;
					}
					// Right segment:
					else 
					if ( (angle > 90 && angle < 270) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
						               blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_W_NE_TOP:
					// 26 bott-right oblique segment:
					if ( (angle > 26 && angle < 207) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
						               blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 2;
							normal->normalize();
						}
						
						return true;
					}
					// Top segment:
					else 
					if ( (angle > 180 && angle < 360) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						               blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					// Left segment:
					else 
					if ( ((angle >= 0 && angle < 90) || (angle > 270 && angle < 360)) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						               blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_NE_BLOCK:
					// Top segment:
					if ( (angle > 180 && angle < 360) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_N_X,  blkY + BLOCK_N_Y,
						               blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					// Left segment:
					else 
					if ( ((angle >= 0 && angle < 90) || (angle > 270 && angle < 360)) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_N_X,      blkY + BLOCK_N_Y,
						               blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 0;
						}
						
						return true;
					}
					// Bottom segment:
					else 
					if ( (angle > 0 && angle < 180) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
						               blkX + BLOCK_E_X,      blkY + BLOCK_E_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = 1;
						}
						
						return true;
					}
					// Right segment:
					else 
					if ( (angle > 90 && angle < 270) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
						               blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_W_E_TOP:
					// Top segment:
					if ( (angle > 180 && angle < 360) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						               blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					// Left segment:
					else 
					if ( ((angle >= 0 && angle < 90) || (angle > 270 && angle < 360)) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						               blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 0;
						}
						
						return true;
					}
					// Bottom segment:
					else 
					if ( (angle > 0 && angle < 180) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_W_X, blkY + BLOCK_W_Y,
						               blkX + BLOCK_E_X, blkY + BLOCK_E_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = 1;
						}
						
						return true;
					}
					// Right segment:
					else 
					if ( (angle > 90 && angle < 270) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
						               blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_NW_BLOCK:
					// Top segment:
					if ( (angle > 180 && angle < 360) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						               blkX + BLOCK_N_X,  blkY + BLOCK_N_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					// Left segment:
					else 
					if ( ((angle >= 0 && angle < 90) || (angle > 270 && angle < 360)) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						               blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 0;
						}
						
						return true;
					}
					// Bottom segment:
					else 
					if ( (angle > 0 && angle < 180) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_W_X,      blkY + BLOCK_W_Y,
						               blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = 1;
						}
						
						return true;
					}
					// Right segment:
					else 
					if ( (angle > 90 && angle < 270) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_N_X,      blkY + BLOCK_N_Y,
						               blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_S_E_BOTTOM:
					// 45 top-left oblique segment:
					if ( ((angle >= 0 && angle < 45) || (angle > 225 && angle < 360)) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_S_X, blkY + BLOCK_S_Y,
						               blkX + BLOCK_E_X, blkY + BLOCK_E_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = -1;
							normal->normalize();
						}
						
						return true;
					}
					// Bottom segment:
					else 
					if ( (angle > 0 && angle < 180) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_S_X,  blkY + BLOCK_S_Y,
						               blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = 1;
						}
						
						return true;
					}
					// Right segment:
					else 
					if ( (angle > 90 && angle < 270) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
						               blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_W_N_BOTTOM:
					// 45 top-left oblique segment:
					if ( ((angle >= 0 && angle < 45) || (angle > 225 && angle < 360)) && 
						 Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_W_X, blkY + BLOCK_W_Y,
						               blkX + BLOCK_N_X, blkY + BLOCK_N_Y,
						               contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = -1;
							normal->normalize();
						}
						
						return true;
					}
					// Top segment:
					if ( (angle > 180 && angle < 360) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_N_X,  blkY + BLOCK_N_Y,
					                   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					// Left segment:
					else 
					if ( ((angle >= 0 && angle < 90) || (angle > 270 && angle < 360)) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
					                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 0;
						}
						
						return true;
					}
					// Bottom segment:
					else 
					if ( (angle > 0 && angle < 180) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
					                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = 1;
						}
						
						return true;
					}
					// Right segment:
					else 
					if ( (angle > 90 && angle < 270) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
					                   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_N_E_BOTTOM:
					// 45 top-right oblique segment:
					if ( (angle > 135 && angle < 315) && 
					     Geom::segmentIntersection( x1, y1,
					                   x2, y2,
					                   blkX + BLOCK_N_X, blkY + BLOCK_N_Y,
					                   blkX + BLOCK_E_X, blkY + BLOCK_E_Y,
					                   contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = -1;
							normal->normalize();
						}
						
						return true;
					}
					// Top segment:
					if ( (angle > 180 && angle < 360) && 
						 Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_N_X,  blkY + BLOCK_N_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					// Left segment:
					else 
					if ( ((angle >= 0 && angle < 90) || (angle > 270 && angle < 360)) && 
						 Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 0;
						}
						
						return true;
					}
					// Bottom segment:
					else 
					if ( (angle > 0 && angle < 180) && 
						 Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = 1;
						}
						
						return true;
					}
					// Right segment:
					else 
					if ( (angle > 90 && angle < 270) && 
						 Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_W_S_BOTTOM:
					// 45 top-right oblique segment:
					if ( (angle > 135 && angle < 315) && 
						 Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X, blkY + BLOCK_W_Y,
									   blkX + BLOCK_S_X, blkY + BLOCK_S_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = -1;
							normal->normalize();
						}
						
						return true;
					}
					// Left segment:
					else 
					if ( ((angle >= 0 && angle < 90) || (angle > 270 && angle < 360)) && 
						 Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 0;
						}
						
						return true;
					}
					// Bottom segment:
					else 
					if ( (angle > 0 && angle < 180) && 
						 Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_S_X,  blkY + BLOCK_S_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = 1;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_N_E_TOP:
					// 45 bott-left oblique segment:
					if ( ((angle >= 0 && angle < 135) || (angle > 315 && angle < 360)) && 
						 Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_N_X, blkY + BLOCK_N_Y,
									   blkX + BLOCK_E_X, blkY + BLOCK_E_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 1;
							normal->normalize();
						}
						
						return true;
					}
					// Top segment:
					else 
					if ( (angle > 180 && angle < 360) && 
						 Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_N_X,  blkY + BLOCK_N_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					// Right segment:
					else 
					if ( (angle > 90 && angle < 270) && 
						 Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_W_S_TOP:
					// 45 bott-left oblique segment:
					if ( ((angle >= 0 && angle < 135) || (angle > 315 && angle < 360)) && 
						 Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X, blkY + BLOCK_W_Y,
									   blkX + BLOCK_S_X, blkY + BLOCK_S_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 1;
							normal->normalize();
						}
						
						return true;
					}
					// Top segment:
					else
					if ( (angle > 180 && angle < 360) && 
						 Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					// Left segment:
					else 
					if ( ((angle >= 0 && angle < 90) || (angle > 270 && angle < 360)) && 
						 Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 0;
						}
						
						return true;
					}
					// Bottom segment:
					else 
					if ( (angle > 0 && angle < 180) && 
						 Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_S_X,  blkY + BLOCK_S_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = 1;
						}
						
						return true;
					}
					// Right segment:
					else 
					if ( (angle > 90 && angle < 270) && 
						 Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_S_E_TOP:
					// 45 bott-right oblique segment:
					if ( (angle > 45 && angle < 225) && 
						 Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_S_X, blkY + BLOCK_S_Y,
									   blkX + BLOCK_E_X, blkY + BLOCK_E_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 1;
							normal->normalize();
						}
						
						return true;
					}
					// Top segment:
					else
					if ( (angle > 180 && angle < 360) && 
						 Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					// Left segment:
					else 
					if ( ((angle >= 0 && angle < 90) || (angle > 270 && angle < 360)) && 
						 Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 0;
						}
						
						return true;
					}
					// Bottom segment:
					else 
					if ( (angle > 0 && angle < 180) && 
						 Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_S_X,  blkY + BLOCK_S_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = 1;
						}
						
						return true;
					}
					// Right segment:
					else 
					if ( (angle > 90 && angle < 270) && 
						 Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_W_N_TOP:
					// 45 bott-right oblique segment:
					if ( (angle > 45 && angle < 225) && 
						 Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X, blkY + BLOCK_W_Y,
									   blkX + BLOCK_N_X, blkY + BLOCK_N_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = 1;
							normal->y = 1;
							normal->normalize();
						}
						
						return true;
					}
					// Top segment:
					else
					if ( (angle > 180 && angle < 360) && 
						 Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_N_X,  blkY + BLOCK_N_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					// Left segment:
					else 
					if ( ((angle >= 0 && angle < 90) || (angle > 270 && angle < 360)) && 
						 Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = -1;
							normal->y = 0;
						}
						
						return true;
					}
					// No collision:
					break;
				
					
				case RoomBlock::BLK_THINFLOOR_HI:
					// Crossed from top to bottom?
					if ( touchThinFloor &&
					     (angle > 180 && angle < 360) &&
					     Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					break;
					
				case RoomBlock::BLK_THINFLOOR_MID:
					// Crossed from top to bottom?
					if ( touchThinFloor &&
					     (angle > 180 && angle < 360) &&
					     Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X, blkY + BLOCK_W_Y,
									   blkX + BLOCK_E_X, blkY + BLOCK_E_Y,
									   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					break;
				
				case RoomBlock::BLK_THINFLOOR_LO:
					// Crossed from top to bottom?
					if ( touchThinFloor &&
					     (angle > 180 && angle < 360) &&
					     Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y - 1,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y - 1,
									   contact ) ) {
						if ( normal ) {
							normal->x = 0;
							normal->y = -1;
						}
						
						return true;
					}
					break;
				
			}
		}
		
		// If we've reached point2's block and still no collisions... give up.
		if ( i == iFinal && j == jFinal )
			return false;
		
		// Go to next block!
		
		bool toLeft = false, toRight = false, toTop = false, toBott = false;
		
		// Left edge:
		if ( rightToLeft &&
		     Geom::segmentIntersection( x1, y1,
			               x2, y2,
			               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
			               blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
			               NULL ) ) {
			j--;
			toLeft = true;
		}
		
		// Right edge:
		if ( leftToRight &&
		     Geom::segmentIntersection( x1, y1,
			               x2, y2,
			               blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
			               blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
			               NULL ) ) {
			j++;
			toRight = true;
		}
		
		// Top edge:
		if ( bottomToTop &&
		     Geom::segmentIntersection( x1, y1,
			               x2, y2,
			               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
			               blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
			               NULL ) ) {
			i--;
			toTop = true;
		}
		
		// Bottom edge:
		if ( topToBottom &&
		     Geom::segmentIntersection( x1, y1,
			               x2, y2,
			               blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
			               blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
			               NULL ) ) {
			i++;
			toBott = true;
		}
		
//		// This is tricky: If *two* of the above conditions were checked, it
//		// means the point is crossing one of the block's edges. This may cause
//		// the collision detection to lose awareness of its surrounding blocks,
//		// since we'll be jumping to a cell that's not immediately adjacent to
//		// the current block.
//		// 
//		// So, here's what I'm going to do: Split the point in two, offsetting
//		// them by a half-pixel to ensure they'll fall on different borders, and
//		// then combine their results.
//		if ( (toLeft && toTop) || (toTop && toRight) || (toRight && toBott) || (toBott && toLeft) ) {
//			Vector2D offset;
//			
//			if ( toLeft && toTop ) {
//				offset.x = 1; offset.y = -1;
//			}
//			else if ( toTop && toRight ) {
//				offset.x = 1; offset.y = 1;
//			}
//			else if ( toRight && toBott ) {
//				offset.x = -1; offset.y = 1;
//			}
//			else {
//				offset.x = -1; offset.y = -1;
//			}
//			
//			Vector2D contact1, normal1;
//			bool p1 = pointCollision( x1 + offset.y, y1 + offset.y, x2 + offset.x, y2 + offset.y, &contact1, &normal1 );
//			
//			Vector2D contact2, normal2;
//			bool p2 = pointCollision( x1 - offset.y, y1 - offset.y, x2 - offset.x, y2 - offset.y, &contact2, &normal2 );
//			
//			// Both collided?
//			if ( p1 && p2 ) {
//				// Set the contact as the average contact of both contacts, and
//				// the normal as the sum of both normals.
//				if ( contact ) {
//					*contact = contact1 + contact2;
//					*contact /= 2;
//				}
//				if ( normal ) {
//					*normal = normal1 + normal2;
//					normal->normalize();
//				}
//				
//				return true;
//			}
//			// Just p1 collided?
//			else if ( p1 ) {
//				if ( contact )
//					*contact = contact1;
//				if ( normal )
//					*normal = normal1;
//				
//				return true;
//			}
//			// Just p2 collided?
//			else if ( p2 ) {
//				if ( contact )
//					*contact = contact2;
//				if ( normal )
//					*normal = normal2;
//				
//				return true;
//			}
//			// Neither collided?
//			else
//				return false;
//			
//		}
		
		
		// No intersection against any of the borders?
		if ( !toLeft && !toRight && !toTop && !toBott )
			// We've technically reached the final block, even though the
			// indices (i, j) are different from (iFinal, jFinal).
			return false;
	}
	
} // End of method: Room::pointCollision (moving point)






bool Room::segmentCollision ( float x1, float y1, float x2, float y2, bool falling ) {

	// Extremities colliding?
	if ( pointCollision( x1, y1 ) || pointCollision( x2, y2 ) )
		return true;
	
	bool leftToRight = (x1 < x2);
	bool rightToLeft = (x1 > x2);
	bool topToBottom = (y1 < y2);
	bool bottomToTop = (y1 > y2);
	
	
	// Point2's block: The loop will end when we reach it.
	int iFinal = (int) floorf( y2 / blockSize );
	int jFinal = (int) floorf( x2 / blockSize );
	
	// Start out at point1's block:
	int i = (int) floorf( y1 / blockSize );
	int j = (int) floorf( x1 / blockSize );
	
	while ( true ) {
		// The current block's top-left coordinates:
		float blkX = (float) j * blockSize;
		float blkY = (float) i * blockSize;
		
		
		// Index inside of matrix?
		if ( i >= 0 && i < obstacleLayer.rows &&
		     j >= 0 && j < obstacleLayer.columns ) {
			
			// Extend a line from point1 to point2, and test for intersection
			// against the obstacle layer:
			switch ( obstacleLayer.cell(i, j) ) {
				
				case RoomBlock::BLK_SE_BLOCK:
					// Top segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
									   blkX + BLOCK_E_X,      blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
									   blkX + BLOCK_S_X,      blkY + BLOCK_S_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_S_X,  blkY + BLOCK_S_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_W_E_BOTTOM:
					// Top segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X, blkY + BLOCK_W_Y,
									   blkX + BLOCK_E_X, blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_SW_BLOCK:
					// Top segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X,      blkY + BLOCK_W_Y,
									   blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_S_X,  blkY + BLOCK_S_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_S_X,      blkY + BLOCK_S_Y,
									   blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_NW_MISSING:
					// Top segments:
					if ( Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_N_X,  blkY + BLOCK_N_Y,
						               blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					if ( Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_W_X,      blkY + BLOCK_W_Y,
						               blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
									   NULL ) ) {
						return true;
					}
					// Left segments:
					if ( Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
						               blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
									   NULL ) ) {
						return true;
					}
					if ( Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_CENTER_X,  blkY + BLOCK_CENTER_Y,
						               blkX + BLOCK_N_X,       blkY + BLOCK_N_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segment:
					if ( Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
						               blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_NE_MISSING:
					// Top segments:
					if ( Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						               blkX + BLOCK_N_X,  blkY + BLOCK_N_Y,
									   NULL ) ) {
						return true;
					}
					if ( Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_E_X,      blkY + BLOCK_E_Y,
						               blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					if ( Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						               blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segment:
					if ( Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
						               blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// Right segments:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_N_X,      blkY + BLOCK_N_Y,
									   blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
									   NULL ) ) {
						return true;
					}
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_SW_NE_BOTTOM:
					// 45 top-left oblique segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_NW_SE_BOTTOM:
					// 45 top-right oblique segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
				
				
				case RoomBlock::BLK_SW_E_BOTTOM:
					// 26 top-left oblique segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_W_NE_BOTTOM:
					// 26 top-left oblique segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_NW_E_BOTTOM:
					// 26 top-right oblique segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_W_SE_BOTTOM:
					// 26 top-right oblique segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_N_S_RIGHT:
					// Top segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_N_X,  blkY + BLOCK_N_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_N_X, blkY + BLOCK_N_Y,
									   blkX + BLOCK_S_X, blkY + BLOCK_S_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_S_X,  blkY + BLOCK_S_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_FULL:
					// Top segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_N_S_LEFT:
					// Top segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_N_X,  blkY + BLOCK_N_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_S_X,  blkY + BLOCK_S_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_N_X, blkY + BLOCK_N_Y,
									   blkX + BLOCK_S_X, blkY + BLOCK_S_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_SW_MISSING:
					// Top segment:
					if ( Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						               blkX + BLOCK_N_X,  blkY + BLOCK_N_Y,
									   NULL ) ) {
						return true;
					}
					// Left segments:
					if ( Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						               blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
									   NULL ) ) {
						return true;
					}
					if ( Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
						               blkX + BLOCK_S_X,      blkY + BLOCK_S_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segments:
					if ( Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_W_X,      blkY + BLOCK_W_Y,
						               blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
									   NULL ) ) {
						return true;
					}
					if ( Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_S_X,  blkY + BLOCK_S_Y,
						               blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_SE_MISSING:
					// Top segment:
					if ( Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						               blkX + BLOCK_N_X,  blkY + BLOCK_N_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					if ( Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
						               blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segments:
					if ( Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
									   blkX + BLOCK_E_X,      blkY + BLOCK_E_Y,
						               NULL ) ) {
						return true;
					}
					if ( Geom::segmentIntersection( x1, y1,
						               x2, y2,
						               blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_S_X,  blkY + BLOCK_S_Y,
						               NULL ) ) {
						return true;
					}
					// Right segments:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
									   blkX + BLOCK_S_X,      blkY + BLOCK_S_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_NW_SE_TOP:
					// 45 bott-left oblique segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// Top segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_SW_NE_TOP:
					// 45 bott-right oblique segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					// Top segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_NW_E_TOP:
					// 26 bott-left oblique segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					// Top segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_W_SE_TOP:
					// 26 bott-left oblique segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// Top segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_SW_E_TOP:
					// 26 bott-right oblique segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					// Top segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_W_NE_TOP:
					// 26 bott-right oblique segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					// Top segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_NE_BLOCK:
					// Top segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_N_X,  blkY + BLOCK_N_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_N_X,      blkY + BLOCK_N_Y,
									   blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
									   blkX + BLOCK_E_X,      blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_W_E_TOP:
					// Top segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X, blkY + BLOCK_W_Y,
									   blkX + BLOCK_E_X, blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_NW_BLOCK:
					// Top segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_N_X,  blkY + BLOCK_N_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X,      blkY + BLOCK_W_Y,
									   blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_N_X,      blkY + BLOCK_N_Y,
									   blkX + BLOCK_CENTER_X, blkY + BLOCK_CENTER_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_S_E_BOTTOM:
					// 45 top-left oblique segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_S_X, blkY + BLOCK_S_Y,
									   blkX + BLOCK_E_X, blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_S_X,  blkY + BLOCK_S_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_W_N_BOTTOM:
					// 45 top-left oblique segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X, blkY + BLOCK_W_Y,
									   blkX + BLOCK_N_X, blkY + BLOCK_N_Y,
									   NULL ) ) {
						return true;
					}
					// Top segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_N_X,  blkY + BLOCK_N_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_N_E_BOTTOM:
					// 45 top-right oblique segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_N_X, blkY + BLOCK_N_Y,
									   blkX + BLOCK_E_X, blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					// Top segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_N_X,  blkY + BLOCK_N_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_W_S_BOTTOM:
					// 45 top-right oblique segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X, blkY + BLOCK_W_Y,
									   blkX + BLOCK_S_X, blkY + BLOCK_S_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_S_X,  blkY + BLOCK_S_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_N_E_TOP:
					// 45 bott-left oblique segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_N_X, blkY + BLOCK_N_Y,
									   blkX + BLOCK_E_X, blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					// Top segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_N_X,  blkY + BLOCK_N_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_W_S_TOP:
					// 45 bott-left oblique segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X, blkY + BLOCK_W_Y,
									   blkX + BLOCK_S_X, blkY + BLOCK_S_Y,
									   NULL ) ) {
						return true;
					}
					// Top segment:
					else
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_S_X,  blkY + BLOCK_S_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_S_E_TOP:
					// 45 bott-right oblique segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_S_X, blkY + BLOCK_S_Y,
									   blkX + BLOCK_E_X, blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					// Top segment:
					else
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   NULL ) ) {
						return true;
					}
					// Bottom segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
									   blkX + BLOCK_S_X,  blkY + BLOCK_S_Y,
									   NULL ) ) {
						return true;
					}
					// Right segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   blkX + BLOCK_E_X,  blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_W_N_TOP:
					// 45 bott-right oblique segment:
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X, blkY + BLOCK_W_Y,
									   blkX + BLOCK_N_X, blkY + BLOCK_N_Y,
									   NULL ) ) {
						return true;
					}
					// Top segment:
					else
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_N_X,  blkY + BLOCK_N_Y,
									   NULL ) ) {
						return true;
					}
					// Left segment:
					else 
					if ( Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_W_X,  blkY + BLOCK_W_Y,
									   NULL ) ) {
						return true;
					}
					// No collision:
					break;
					
					
				case RoomBlock::BLK_THINFLOOR_HI:
					// Crossed from top to bottom?
					if ( falling &&
					     Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
									   blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
									   NULL ) ) {
						return true;
					}
					break;
					
				case RoomBlock::BLK_THINFLOOR_MID:
					// Crossed from top to bottom?
					if ( falling &&
					     Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_W_X, blkY + BLOCK_W_Y,
									   blkX + BLOCK_E_X, blkY + BLOCK_E_Y,
									   NULL ) ) {
						return true;
					}
					break;
				
				case RoomBlock::BLK_THINFLOOR_LO:
					// Crossed from top to bottom?
					if ( falling &&
					     Geom::segmentIntersection( x1, y1,
									   x2, y2,
									   blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y - 1,
									   blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y - 1,
									   NULL ) ) {
						return true;
					}
					break;
				
			}
		}
		
		// If we've reached point2's block and still no collisions... give up.
		if ( i == iFinal && j == jFinal )
			return false;
		
		// Go to next block!
		
		bool changed = false;
		
		// Left edge:
		if ( rightToLeft &&
		     Geom::segmentIntersection( x1, y1,
			               x2, y2,
			               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
			               blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
			               NULL ) ) {
			j--;
			changed = true;
		}
		
		// Right edge:
		if ( leftToRight &&
		     Geom::segmentIntersection( x1, y1,
			               x2, y2,
			               blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
			               blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
			               NULL ) ) {
			j++;
			changed = true;
		}
		
		// Top edge:
		if ( bottomToTop &&
		     Geom::segmentIntersection( x1, y1,
			               x2, y2,
			               blkX + BLOCK_NW_X, blkY + BLOCK_NW_Y,
			               blkX + BLOCK_NE_X, blkY + BLOCK_NE_Y,
			               NULL ) ) {
			i--;
			changed = true;
		}
		
		// Bottom edge:
		if ( topToBottom &&
		     Geom::segmentIntersection( x1, y1,
			               x2, y2,
			               blkX + BLOCK_SW_X, blkY + BLOCK_SW_Y,
			               blkX + BLOCK_SE_X, blkY + BLOCK_SE_Y,
			               NULL ) ) {
			i++;
			changed = true;
		}
		
		// No intersection against any of the borders?
		if ( !changed )
			// We've technically reached the final block, even though the
			// indices (i, j) are different from (iFinal, jFinal).
			return false;
	}
	
} // End of method: Room::segmentCollision (static segment)	







bool Room::segmentCollision ( float x1, float y1, float x2, float y2, float dx, float dy, bool touchThinFloor,
                              Container<Vector2D> * contacts, Container<Vector2D> * normals, Vector2D * validDisplacement ) {
	
	// Pathological case: The segment is colliding right off the start:
	if ( segmentCollision( x1, y1, x2, y2, (touchThinFloor && dy >= 0) ) ) {
		// For the contact point, use the segment's center:
		if ( contacts ) {
			contacts->removeAll();
			contacts->add( Vector2D( (x1 + x2)/2, (y1 + y2)/2) );
		}
		
		// For the normal, use a vector pointing in the displacement's opposite
		// direction:
		if ( normals ) {
			normals->removeAll();
			Vector2D n = Vector2D( -dx, -dy);
			n.normalize();
			normals->add( n );
		}
		
		// No valid displacement!
		if ( validDisplacement ) {
			validDisplacement->x = validDisplacement->y = 0;
		}
		
		return true;
	}
	
	// Pathological case: Segment with length zero.
	if ( sqrtf( (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) ) < EPSILON ) {
		// It's a point!
		Vector2D tmpContact, tmpNormal;
		
		if ( pointCollision( x1, y1, x1 + dx, y1 + dy, touchThinFloor, &tmpContact, &tmpNormal ) ) {
			if ( contacts ) {
				contacts->removeAll();
				contacts->add( tmpContact );
			}

			if ( normals ) {
				normals->removeAll();
				normals->add( tmpNormal );
			}
			
			if ( validDisplacement ) {
				validDisplacement->x = tmpContact.x - x1;
				validDisplacement->y = tmpContact.y - y1;
			}
			
			return true;
		}
		else
			return false;
	}
	
	
	// These containers will store the candidates for collision.
	static Container<Vector2D> contactCandidates;
	static Container<Vector2D> normalCandidates;

	contactCandidates.removeAll();
	normalCandidates.removeAll();
	
	// If the segment hasn't moved, there was no collision:
	if ( dx == 0 && dy == 0 )
		return false;
	
	// The segment's angle relative to the X-axis (as if it were a moving point
	// from (x1,y1) -> (x2,y2)):
	float segmAngle = degreeAngle( x2 - x1, y2 - y1 );
	
	
	// Here's the plan:
	// The obvious choices are the points where the segment's extremities touch
	// the obstacle layer, so these will be included in the list of candidates
	// right away.
	// 
	// There is also a possibility that the segment will touch a "corner" in
	// the obstacle layer; thus, I will test *all* the corners in the segment's
	// path to determine which ones are candidate for collision.
	Vector2D tmpContact, tmpNormal;
	
	// "Obvious" candidates:
	// Extremity 1:
	if ( pointCollision( x1, y1, x1 + dx, y1 + dy, touchThinFloor, &tmpContact, &tmpNormal ) ) {
		contactCandidates.add( tmpContact );
		normalCandidates.add( tmpNormal );
	}
	
	// Extremity 2:
	if ( pointCollision( x2, y2, x2 + dx, y2 + dy, touchThinFloor, &tmpContact, &tmpNormal ) ) {
		contactCandidates.add( tmpContact );
		normalCandidates.add( tmpNormal );
	}
	
	
	// Now, find all "corners."
	// A polygon representing the segment's path:
	static Container<Vector2D> pathPolygon;
	while ( pathPolygon.getCount() < 4 )
		pathPolygon.add( Vector2D() );
	
	pathPolygon[0].x = x1;      pathPolygon[0].y = y1;
	pathPolygon[1].x = x2;      pathPolygon[1].y = y2;
	pathPolygon[2].x = x2 + dx; pathPolygon[2].y = y2 + dy;
	pathPolygon[3].x = x1 + dx; pathPolygon[3].y = y1 + dy;
	
	// Examine a matrix that covers the segment's path:
	int startI = (int) (std::min(std::min(y1, y2), std::min(y1 + dy, y2 + dy))) / blockSize;
	int startJ = (int) (std::min(std::min(x1, x2), std::min(x1 + dx, x2 + dx))) / blockSize;
	int finalI = (int) (std::max(std::max(y1, y2), std::max(y1 + dy, y2 + dy))) / blockSize;
	int finalJ = (int) (std::max(std::max(x1, x2), std::max(x1 + dx, x2 + dx))) / blockSize;
	
	if ( startI <  0                     ) startI = 0;
	if ( startI >= obstacleLayer.rows    ) startI = obstacleLayer.rows - 1;
	if ( finalI <  0                     ) finalI = 0;
	if ( finalI >= obstacleLayer.rows    ) finalI = obstacleLayer.rows - 1;
	if ( startJ <  0                     ) startJ = 0;
	if ( startJ >= obstacleLayer.columns ) startJ = obstacleLayer.columns - 1;
	if ( finalJ <  0                     ) finalJ = 0;
	if ( finalJ >= obstacleLayer.columns ) finalJ = obstacleLayer.columns - 1;
	
#define ANG_ERR 1.0f
	
	for ( int i = startI; i <= finalI; i++ ) {
		for ( int j = startJ; j <= finalJ; j++ ) {
			// The current block's top-left coordinates:
			float blkX = (float) (j * blockSize);
			float blkY = (float) (i * blockSize);
			
			// Add all "corners" to the list of contact candidates:
			switch ( obstacleLayer.cell(i, j) ) {
				
				case RoomBlock::BLK_SE_BLOCK:
					// Top-left:
					tmpContact.x = blkX + BLOCK_CENTER_X;
					tmpContact.y = blkY + BLOCK_CENTER_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90.0f, ANG_ERR) ||
						     marginCmp(segmAngle, 270.0f, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0.0f, ANG_ERR) ||
						          marginCmp(segmAngle, 180.0f, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_E_X;
					tmpContact.y = blkY + BLOCK_E_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90.0f, ANG_ERR) ||
						     marginCmp(segmAngle, 270.0f, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_SE_X;
					tmpContact.y = blkY + BLOCK_SE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90.0f, ANG_ERR) ||
						     marginCmp(segmAngle, 270.0f, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0.0f, ANG_ERR) ||
						          marginCmp(segmAngle, 180.0f, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_S_X;
					tmpContact.y = blkY + BLOCK_S_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90.0f, ANG_ERR) ||
						     marginCmp(segmAngle, 270.0f, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0.0f, ANG_ERR) ||
						          marginCmp(segmAngle, 180.0f, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
					}
					break;
					
					
				case RoomBlock::BLK_W_E_BOTTOM:
					// Top-left:
					tmpContact.x = blkX + BLOCK_W_X;
					tmpContact.y = blkY + BLOCK_W_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_E_X;
					tmpContact.y = blkY + BLOCK_E_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_SE_X;
					tmpContact.y = blkY + BLOCK_SE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_SW_X;
					tmpContact.y = blkY + BLOCK_SW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
					}
					break;
					
					
				case RoomBlock::BLK_SW_BLOCK:
					// Top-left:
					tmpContact.x = blkX + BLOCK_W_X;
					tmpContact.y = blkY + BLOCK_W_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_CENTER_X;
					tmpContact.y = blkY + BLOCK_CENTER_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_S_X;
					tmpContact.y = blkY + BLOCK_S_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_SW_X;
					tmpContact.y = blkY + BLOCK_SW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
					}
					break;
					
					
				case RoomBlock::BLK_NW_MISSING:
					// Top-left1:
					tmpContact.x = blkX + BLOCK_N_X;
					tmpContact.y = blkY + BLOCK_N_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
					}
					// Top-left2:
					tmpContact.x = blkX + BLOCK_W_X;
					tmpContact.y = blkY + BLOCK_W_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_NE_X;
					tmpContact.y = blkY + BLOCK_NE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_SE_X;
					tmpContact.y = blkY + BLOCK_SE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_SW_X;
					tmpContact.y = blkY + BLOCK_SW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
					}
					break;
					
					
				case RoomBlock::BLK_NE_MISSING:
					// Top-left:
					tmpContact.x = blkX + BLOCK_NW_X;
					tmpContact.y = blkY + BLOCK_NW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
					}
					// Top-right1:
					tmpContact.x = blkX + BLOCK_N_X;
					tmpContact.y = blkY + BLOCK_N_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
					}
					// Top-right2:
					tmpContact.x = blkX + BLOCK_E_X;
					tmpContact.y = blkY + BLOCK_E_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_SE_X;
					tmpContact.y = blkY + BLOCK_SE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_SW_X;
					tmpContact.y = blkY + BLOCK_SW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
					}
					break;
					
					
				case RoomBlock::BLK_SW_NE_BOTTOM:
					// Top-right:
					tmpContact.x = blkX + BLOCK_NE_X;
					tmpContact.y = blkY + BLOCK_NE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// 45 degree segment?
						else if ( marginCmp(segmAngle,  45, ANG_ERR) ||
						          marginCmp(segmAngle, 225, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
						else
							normalCandidates.add( Vector2D( 0.382683f, -0.92388f ) );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_SE_X;
					tmpContact.y = blkY + BLOCK_SE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_SW_X;
					tmpContact.y = blkY + BLOCK_SW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						// 45 degree segment?
						else if ( marginCmp(segmAngle,  45, ANG_ERR) ||
						          marginCmp(segmAngle, 225, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
						else
							normalCandidates.add( Vector2D( -0.92388f, 0.382683f ) );
					}
					break;
					
					
				case RoomBlock::BLK_NW_SE_BOTTOM:
					// Top-left:
					tmpContact.x = blkX + BLOCK_NW_X;
					tmpContact.y = blkY + BLOCK_NW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// 45 degree segment?
						else if ( marginCmp(segmAngle, 135, ANG_ERR) ||
						          marginCmp(segmAngle, 315, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
						else
							normalCandidates.add( Vector2D( -0.382683f, -0.92388f ) );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_SW_X;
					tmpContact.y = blkY + BLOCK_SW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_SE_X;
					tmpContact.y = blkY + BLOCK_SE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						     marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						// 45 degree segment?
						else if ( marginCmp(segmAngle, 135, ANG_ERR) ||
						          marginCmp(segmAngle, 315, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
						else
							normalCandidates.add( Vector2D( 0.92388f, 0.382683f ) );
					}
					break;
					
					
				case RoomBlock::BLK_SW_E_BOTTOM:
					// Top-right:
					tmpContact.x = blkX + BLOCK_E_X;
					tmpContact.y = blkY + BLOCK_E_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// 26 degree segment?
						else if ( marginCmp(segmAngle,  26, ANG_ERR) ||
						          marginCmp(segmAngle, 206, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, -2 ).unit() );
						else
							normalCandidates.add( Vector2D( 0.525731f, -0.850651f ) );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_SE_X;
					tmpContact.y = blkY + BLOCK_SE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_SW_X;
					tmpContact.y = blkY + BLOCK_SW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						// 26 degree segment?
						else if ( marginCmp(segmAngle,  26, ANG_ERR) ||
						          marginCmp(segmAngle, 206, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, -2 ).unit() );
						else
							normalCandidates.add( Vector2D( -0.973249f, 0.229753f ) );
					}
					break;
					
					
				case RoomBlock::BLK_W_NE_BOTTOM:
					// Top-right:
					tmpContact.x = blkX + BLOCK_NE_X;
					tmpContact.y = blkY + BLOCK_NE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// 26 degree segment?
						else if ( marginCmp(segmAngle,  26, ANG_ERR) ||
						          marginCmp(segmAngle, 206, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, -2 ).unit() );
						else
							normalCandidates.add( Vector2D( 0.525731f, -0.850651f ) );
					}
					// Top-left:
					tmpContact.x = blkX + BLOCK_W_X;
					tmpContact.y = blkY + BLOCK_W_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// 26 degree segment?
						else if ( marginCmp(segmAngle,  26, ANG_ERR) ||
						          marginCmp(segmAngle, 206, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, -2 ).unit() );
						else
							normalCandidates.add( Vector2D( -0.850651f, -0.525731f ) );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_SE_X;
					tmpContact.y = blkY + BLOCK_SE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_SW_X;
					tmpContact.y = blkY + BLOCK_SW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
					}
					break;
					
					
				case RoomBlock::BLK_NW_E_BOTTOM:
					// Top-left:
					tmpContact.x = blkX + BLOCK_NW_X;
					tmpContact.y = blkY + BLOCK_NW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// 26 degree segment?
						else if ( marginCmp(segmAngle, 154, ANG_ERR) ||
						          marginCmp(segmAngle, 334, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, -2 ).unit() );
						else
							normalCandidates.add( Vector2D( -0.525731f, -0.850651f ) );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_E_X;
					tmpContact.y = blkY + BLOCK_E_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						// 26 degree segment?
						else if ( marginCmp(segmAngle, 154, ANG_ERR) ||
						          marginCmp(segmAngle, 334, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, -2 ).unit() );
						else
							normalCandidates.add( Vector2D( 0.850651f, -0.525731f ) );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_SE_X;
					tmpContact.y = blkY + BLOCK_SE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_SW_X;
					tmpContact.y = blkY + BLOCK_SW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
					}
					break;
					
					
				case RoomBlock::BLK_W_SE_BOTTOM:
					// Top-left:
					tmpContact.x = blkX + BLOCK_W_X;
					tmpContact.y = blkY + BLOCK_W_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// 26 degree segment?
						else if ( marginCmp(segmAngle, 154, ANG_ERR) ||
						          marginCmp(segmAngle, 334, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, -2 ).unit() );
						else
							normalCandidates.add( Vector2D( -0.525731f, -0.850651f ) );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_SE_X;
					tmpContact.y = blkY + BLOCK_SE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						// 26 degree segment?
						else if ( marginCmp(segmAngle, 154, ANG_ERR) ||
						          marginCmp(segmAngle, 334, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, -2 ).unit() );
						else
							normalCandidates.add( Vector2D( 0.973249f, 0.229753f ) );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_SW_X;
					tmpContact.y = blkY + BLOCK_SW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
					}
					break;
					
					
				case RoomBlock::BLK_N_S_RIGHT:
					// Top-left:
					tmpContact.x = blkX + BLOCK_N_X;
					tmpContact.y = blkY + BLOCK_N_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_NE_X;
					tmpContact.y = blkY + BLOCK_NE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_SE_X;
					tmpContact.y = blkY + BLOCK_SE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_S_X;
					tmpContact.y = blkY + BLOCK_S_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
					}
					break;
					
					
				case RoomBlock::BLK_FULL:
					// Top-left:
					tmpContact.x = blkX + BLOCK_NW_X;
					tmpContact.y = blkY + BLOCK_NW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_NE_X;
					tmpContact.y = blkY + BLOCK_NE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_SE_X;
					tmpContact.y = blkY + BLOCK_SE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_SW_X;
					tmpContact.y = blkY + BLOCK_SW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
					}
					break;
					
					
				case RoomBlock::BLK_N_S_LEFT:
					// Top-left:
					tmpContact.x = blkX + BLOCK_NW_X;
					tmpContact.y = blkY + BLOCK_NW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_N_X;
					tmpContact.y = blkY + BLOCK_N_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_S_X;
					tmpContact.y = blkY + BLOCK_S_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_SW_X;
					tmpContact.y = blkY + BLOCK_SW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
					}
					break;
					
					
				case RoomBlock::BLK_SW_MISSING:
					// Top-left:
					tmpContact.x = blkX + BLOCK_NW_X;
					tmpContact.y = blkY + BLOCK_NW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_NE_X;
					tmpContact.y = blkY + BLOCK_NE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_SE_X;
					tmpContact.y = blkY + BLOCK_SE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
					}
					// Bottom-left1:
					tmpContact.x = blkX + BLOCK_W_X;
					tmpContact.y = blkY + BLOCK_W_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
					}
					// Bottom-left2:
					tmpContact.x = blkX + BLOCK_S_X;
					tmpContact.y = blkY + BLOCK_S_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
					}
					break;
					
					
				case RoomBlock::BLK_SE_MISSING:
					// Top-left:
					tmpContact.x = blkX + BLOCK_NW_X;
					tmpContact.y = blkY + BLOCK_NW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_NE_X;
					tmpContact.y = blkY + BLOCK_NE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
					}
					// Bottom-right1:
					tmpContact.x = blkX + BLOCK_S_X;
					tmpContact.y = blkY + BLOCK_S_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
					}
					// Bottom-right2:
					tmpContact.x = blkX + BLOCK_E_X;
					tmpContact.y = blkY + BLOCK_E_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_SW_X;
					tmpContact.y = blkY + BLOCK_SW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
					}
					break;
					
					
				case RoomBlock::BLK_NW_SE_TOP:
					// Top-left:
					tmpContact.x = blkX + BLOCK_NW_X;
					tmpContact.y = blkY + BLOCK_NW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// 45 degree segment?
						else if ( marginCmp(segmAngle, 135, ANG_ERR) ||
						          marginCmp(segmAngle, 315, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
						else
							normalCandidates.add( Vector2D( -0.92388f, -0.382683f ) );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_NE_X;
					tmpContact.y = blkY + BLOCK_NE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_SE_X;
					tmpContact.y = blkY + BLOCK_SE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						// 45 degree segment?
						else if ( marginCmp(segmAngle, 135, ANG_ERR) ||
						          marginCmp(segmAngle, 315, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
						else
							normalCandidates.add( Vector2D( 0.382683f, 0.92388f ) );
					}
					break;
					
					
				case RoomBlock::BLK_SW_NE_TOP:
					// Top-left:
					tmpContact.x = blkX + BLOCK_NW_X;
					tmpContact.y = blkY + BLOCK_NW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Horizontal segment?
						if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						     marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// Vertical segment?
						else if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						          marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						else
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_NE_X;
					tmpContact.y = blkY + BLOCK_NE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// 45 degree segment?
						else if ( marginCmp(segmAngle,  45, ANG_ERR) ||
						          marginCmp(segmAngle, 225, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
						else
							normalCandidates.add( Vector2D( 0.92388f, -0.382683f ) );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_SW_X;
					tmpContact.y = blkY + BLOCK_SW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						// 45 degree segment?
						else if ( marginCmp(segmAngle,  45, ANG_ERR) ||
						          marginCmp(segmAngle, 225, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
						else
							normalCandidates.add( Vector2D( -0.382683f, 0.92388f ) );
					}
					break;
					
					
				case RoomBlock::BLK_NW_E_TOP:
					// Top-left:
					tmpContact.x = blkX + BLOCK_NW_X;
					tmpContact.y = blkY + BLOCK_NW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// 26 degree segment?
						else if ( marginCmp(segmAngle, 154, ANG_ERR) ||
						          marginCmp(segmAngle, 334, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 2 ).unit() );
						else
							normalCandidates.add( Vector2D( -0.973249f, -0.229753f ) );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_NE_X;
					tmpContact.y = blkY + BLOCK_NE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_E_X;
					tmpContact.y = blkY + BLOCK_E_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						// 26 degree segment?
						else if ( marginCmp(segmAngle, 154, ANG_ERR) ||
						          marginCmp(segmAngle, 334, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 2 ).unit() );
						else
							normalCandidates.add( Vector2D( 0.525731f, 0.850651f ) );
					}
					break;
					
					
				case RoomBlock::BLK_W_SE_TOP:
					// Top-left:
					tmpContact.x = blkX + BLOCK_NW_X;
					tmpContact.y = blkY + BLOCK_NW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Horizontal segment?
						if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						     marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// Vertical segment?
						else if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						          marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						else
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_W_X;
					tmpContact.y = blkY + BLOCK_W_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						// 26 degree segment?
						else if ( marginCmp(segmAngle, 154, ANG_ERR) ||
						          marginCmp(segmAngle, 334, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 2 ).unit() );
						else
							normalCandidates.add( Vector2D( -0.850651f, 0.525731f ) );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_NE_X;
					tmpContact.y = blkY + BLOCK_NE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_SE_X;
					tmpContact.y = blkY + BLOCK_SE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						// 26 degree segment?
						else if ( marginCmp(segmAngle, 154, ANG_ERR) ||
						          marginCmp(segmAngle, 334, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 2 ).unit() );
						else
							normalCandidates.add( Vector2D( 0.525731f, 0.850651f ) );
					}
					break;
					
					
				case RoomBlock::BLK_SW_E_TOP:
					// Top-left:
					tmpContact.x = blkX + BLOCK_NW_X;
					tmpContact.y = blkY + BLOCK_NW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Horizontal segment?
						if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						     marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// Vertical segment?
						else if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						          marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						else
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_NE_X;
					tmpContact.y = blkY + BLOCK_NE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_E_X;
					tmpContact.y = blkY + BLOCK_E_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						// 26 degree segment?
						else if ( marginCmp(segmAngle,  26, ANG_ERR) ||
						          marginCmp(segmAngle, 206, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 2 ).unit() );
						else
							normalCandidates.add( Vector2D( 0.850651f, 0.525731f ) );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_SW_X;
					tmpContact.y = blkY + BLOCK_SW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						// 26 degree segment?
						else if ( marginCmp(segmAngle,  26, ANG_ERR) ||
						          marginCmp(segmAngle, 206, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 2 ).unit() );
						else
							normalCandidates.add( Vector2D( -0.525731f, 0.850651f ) );
					}
					break;
					
					
				case RoomBlock::BLK_W_NE_TOP:
					// Top-left:
					tmpContact.x = blkX + BLOCK_NW_X;
					tmpContact.y = blkY + BLOCK_NW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Horizontal segment?
						if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						     marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// Vertical segment?
						else if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						          marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						else
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_NE_X;
					tmpContact.y = blkY + BLOCK_NE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// 26 degree segment?
						else if ( marginCmp(segmAngle,  26, ANG_ERR) ||
						          marginCmp(segmAngle, 206, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 2 ).unit() );
						else
							normalCandidates.add( Vector2D( 0.973249f, -0.229753f ) );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_W_X;
					tmpContact.y = blkY + BLOCK_W_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						// 26 degree segment?
						else if ( marginCmp(segmAngle,  26, ANG_ERR) ||
						          marginCmp(segmAngle, 206, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 2 ).unit() );
						else
							normalCandidates.add( Vector2D( -0.525731f, 0.850651f ) );
					}
					break;
					
					
				case RoomBlock::BLK_NE_BLOCK:
					// Top-left:
					tmpContact.x = blkX + BLOCK_N_X;
					tmpContact.y = blkY + BLOCK_N_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_NE_X;
					tmpContact.y = blkY + BLOCK_NE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_E_X;
					tmpContact.y = blkY + BLOCK_E_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_CENTER_X;
					tmpContact.y = blkY + BLOCK_CENTER_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
					}
					break;
					
					
				case RoomBlock::BLK_W_E_TOP:
					// Top-left:
					tmpContact.x = blkX + BLOCK_NW_X;
					tmpContact.y = blkY + BLOCK_NW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_NE_X;
					tmpContact.y = blkY + BLOCK_NE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_E_X;
					tmpContact.y = blkY + BLOCK_E_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_W_X;
					tmpContact.y = blkY + BLOCK_W_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
					}
					break;
					
					
				case RoomBlock::BLK_NW_BLOCK:
					// Top-left:
					tmpContact.x = blkX + BLOCK_NW_X;
					tmpContact.y = blkY + BLOCK_NW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_N_X;
					tmpContact.y = blkY + BLOCK_N_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_CENTER_X;
					tmpContact.y = blkY + BLOCK_CENTER_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_W_X;
					tmpContact.y = blkY + BLOCK_W_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
					}
					break;
					
					
				case RoomBlock::BLK_S_E_BOTTOM:
					// Top-right:
					tmpContact.x = blkX + BLOCK_E_X;
					tmpContact.y = blkY + BLOCK_E_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// 45 degree segment?
						else if ( marginCmp(segmAngle,  45, ANG_ERR) ||
						          marginCmp(segmAngle, 225, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
						else
							normalCandidates.add( Vector2D( 0.382683f, -0.92388f ) );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_SE_X;
					tmpContact.y = blkY + BLOCK_SE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_S_X;
					tmpContact.y = blkY + BLOCK_S_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						// 45 degree segment?
						else if ( marginCmp(segmAngle,  45, ANG_ERR) ||
						          marginCmp(segmAngle, 225, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
						else
							normalCandidates.add( Vector2D( -0.92388f, 0.382683f ) );
					}
					break;
					
					
				case RoomBlock::BLK_W_N_BOTTOM:
					// Top-left1:
					tmpContact.x = blkX + BLOCK_N_X;
					tmpContact.y = blkY + BLOCK_N_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// 45 degree segment?
						else if ( marginCmp(segmAngle,  45, ANG_ERR) ||
						          marginCmp(segmAngle, 225, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
						else
							normalCandidates.add( Vector2D( -0.382683f, -0.92388f ) );
					}
					// Top-left2:
					tmpContact.x = blkX + BLOCK_W_X;
					tmpContact.y = blkY + BLOCK_W_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// 45 degree segment?
						else if ( marginCmp(segmAngle,  45, ANG_ERR) ||
						          marginCmp(segmAngle, 225, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
						else
							normalCandidates.add( Vector2D( -0.92388f, -0.382683f ) );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_NE_X;
					tmpContact.y = blkY + BLOCK_NE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_SE_X;
					tmpContact.y = blkY + BLOCK_SE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_SW_X;
					tmpContact.y = blkY + BLOCK_SW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
					}
					break;
					
					
				case RoomBlock::BLK_N_E_BOTTOM:
					// Top-left:
					tmpContact.x = blkX + BLOCK_NW_X;
					tmpContact.y = blkY + BLOCK_NW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
					}
					// Top-right1:
					tmpContact.x = blkX + BLOCK_N_X;
					tmpContact.y = blkY + BLOCK_N_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// 45 degree segment?
						else if ( marginCmp(segmAngle, 135, ANG_ERR) ||
						          marginCmp(segmAngle, 315, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
						else
							normalCandidates.add( Vector2D( 0.382683f, -0.92388f ) );
					}
					// Top-right2:
					tmpContact.x = blkX + BLOCK_E_X;
					tmpContact.y = blkY + BLOCK_E_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// 45 degree segment?
						else if ( marginCmp(segmAngle, 135, ANG_ERR) ||
						          marginCmp(segmAngle, 315, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
						else
							normalCandidates.add( Vector2D( 0.92388f, -0.382683f ) );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_SE_X;
					tmpContact.y = blkY + BLOCK_SE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_SW_X;
					tmpContact.y = blkY + BLOCK_SW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
					}
					break;
					
					
				case RoomBlock::BLK_W_S_BOTTOM:
					// Top-left:
					tmpContact.x = blkX + BLOCK_W_X;
					tmpContact.y = blkY + BLOCK_W_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// 45 degree segment?
						// 45 degree segment?
						else if ( marginCmp(segmAngle, 135, ANG_ERR) ||
						          marginCmp(segmAngle, 315, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
						else
							normalCandidates.add( Vector2D( -0.382683f, -0.92388f ) );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_SW_X;
					tmpContact.y = blkY + BLOCK_SW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_S_X;
					tmpContact.y = blkY + BLOCK_S_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						     marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						// 45 degree segment?
						else if ( marginCmp(segmAngle, 135, ANG_ERR) ||
						          marginCmp(segmAngle, 315, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
						else
							normalCandidates.add( Vector2D( 0.92388f, 0.382683f ) );
					}
					break;
					
					
				case RoomBlock::BLK_N_E_TOP:
					// Top-left:
					tmpContact.x = blkX + BLOCK_N_X;
					tmpContact.y = blkY + BLOCK_N_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// 45 degree segment?
						else if ( marginCmp(segmAngle, 135, ANG_ERR) ||
						          marginCmp(segmAngle, 315, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
						else
							normalCandidates.add( Vector2D( -0.92388f, -0.382683f ) );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_NE_X;
					tmpContact.y = blkY + BLOCK_NE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_E_X;
					tmpContact.y = blkY + BLOCK_E_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						// 45 degree segment?
						else if ( marginCmp(segmAngle, 135, ANG_ERR) ||
						          marginCmp(segmAngle, 315, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
						else
							normalCandidates.add( Vector2D( 0.382683f, 0.92388f ) );
					}
					break;
					
					
				case RoomBlock::BLK_W_S_TOP:
					// Top-left:
					tmpContact.x = blkX + BLOCK_NW_X;
					tmpContact.y = blkY + BLOCK_NW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_NE_X;
					tmpContact.y = blkY + BLOCK_NE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
					}
					// Bottom-right:
					tmpContact.x = blkX + BLOCK_SE_X;
					tmpContact.y = blkY + BLOCK_SE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
					}
					// Bottom-left1:
					tmpContact.x = blkX + BLOCK_W_X;
					tmpContact.y = blkY + BLOCK_W_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						// 45 degree segment?
						else if ( marginCmp(segmAngle, 135, ANG_ERR) ||
						          marginCmp(segmAngle, 315, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
						else
							normalCandidates.add( Vector2D( -0.92388f, 0.382683f ) );
					}
					// Bottom-left2:
					tmpContact.x = blkX + BLOCK_S_X;
					tmpContact.y = blkY + BLOCK_S_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						// 45 degree segment?
						else if ( marginCmp(segmAngle, 135, ANG_ERR) ||
						          marginCmp(segmAngle, 315, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
						else
							normalCandidates.add( Vector2D( -0.382683f, 0.92388f ) );
					}
					break;
					
					
				case RoomBlock::BLK_S_E_TOP:
					// Top-left:
					tmpContact.x = blkX + BLOCK_NW_X;
					tmpContact.y = blkY + BLOCK_NW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_NE_X;
					tmpContact.y = blkY + BLOCK_NE_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						else
							normalCandidates.add( Vector2D( 1, -1 ).unit() );
					}
					// Bottom-right1:
					tmpContact.x = blkX + BLOCK_S_X;
					tmpContact.y = blkY + BLOCK_S_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						// 45 degree segment?
						else if ( marginCmp(segmAngle,  45, ANG_ERR) ||
						          marginCmp(segmAngle, 225, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
						else
							normalCandidates.add( Vector2D( 0.92388f, 0.382683f ) );
					}
					// Bottom-right2:
					tmpContact.x = blkX + BLOCK_E_X;
					tmpContact.y = blkY + BLOCK_E_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						// 45 degree segment?
						else if ( marginCmp(segmAngle,  45, ANG_ERR) ||
						          marginCmp(segmAngle, 225, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
						else
							normalCandidates.add( Vector2D( 0.92388f, 0.382683f ) );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_SW_X;
					tmpContact.y = blkY + BLOCK_SW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						else
							normalCandidates.add( Vector2D( -1, 1 ).unit() );
					}
					break;
					
					
				case RoomBlock::BLK_W_N_TOP:
					// Top-left:
					tmpContact.x = blkX + BLOCK_NW_X;
					tmpContact.y = blkY + BLOCK_NW_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Horizontal segment?
						if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						     marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// Vertical segment?
						else if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						          marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						else
							normalCandidates.add( Vector2D( -1, -1 ).unit() );
					}
					// Top-right:
					tmpContact.x = blkX + BLOCK_N_X;
					tmpContact.y = blkY + BLOCK_N_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, -1 ) );
						// 45 degree segment?
						else if ( marginCmp(segmAngle,  45, ANG_ERR) ||
						          marginCmp(segmAngle, 225, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
						else
							normalCandidates.add( Vector2D( 0.92388f, -0.382683f ) );
					}
					// Bottom-left:
					tmpContact.x = blkX + BLOCK_W_X;
					tmpContact.y = blkY + BLOCK_W_Y;
					if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
						contactCandidates.add( tmpContact );
						
						// Vertical segment?
						if ( marginCmp(segmAngle,  90, ANG_ERR) ||
						     marginCmp(segmAngle, 270, ANG_ERR) )
							normalCandidates.add( Vector2D( -1, 0 ) );
						// Horizontal segment?
						else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
						          marginCmp(segmAngle, 180, ANG_ERR) )
							normalCandidates.add( Vector2D( 0, 1 ) );
						// 45 degree segment?
						else if ( marginCmp(segmAngle,  45, ANG_ERR) ||
						          marginCmp(segmAngle, 225, ANG_ERR) )
							normalCandidates.add( Vector2D( 1, 1 ).unit() );
						else
							normalCandidates.add( Vector2D( -0.382683f, 0.92388f ) );
					}
					break;
					
					
				case RoomBlock::BLK_THINFLOOR_HI:
					// Only test if the segment's moving from top to bottom:
					if ( touchThinFloor && dy >= 0 ) {
						// Top-left:
						tmpContact.x = blkX + BLOCK_NW_X;
						tmpContact.y = blkY + BLOCK_NW_Y;
						if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
							contactCandidates.add( tmpContact );
							
							// Vertical segment?
							if ( marginCmp(segmAngle,  90, ANG_ERR) ||
								 marginCmp(segmAngle, 270, ANG_ERR) )
								normalCandidates.add( Vector2D( -1, 0 ) );
							// Horizontal segment?
							else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
									  marginCmp(segmAngle, 180, ANG_ERR) )
								normalCandidates.add( Vector2D( 0, -1 ) );
							else
								normalCandidates.add( Vector2D( -1, -1 ).unit() );
						}
						// Top-right:
						tmpContact.x = blkX + BLOCK_NE_X;
						tmpContact.y = blkY + BLOCK_NE_Y;
						if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
							contactCandidates.add( tmpContact );
							
							// Vertical segment?
							if ( marginCmp(segmAngle,  90, ANG_ERR) ||
								 marginCmp(segmAngle, 270, ANG_ERR) )
								normalCandidates.add( Vector2D( 1, 0 ) );
							// Horizontal segment?
							else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
									  marginCmp(segmAngle, 180, ANG_ERR) )
								normalCandidates.add( Vector2D( 0, -1 ) );
							else
								normalCandidates.add( Vector2D( 1, -1 ).unit() );
						}
					}
					break;
					
					
				case RoomBlock::BLK_THINFLOOR_MID:
					// Only test if the segment's moving from top to bottom:
					if ( touchThinFloor && dy >= 0 ) {
						// Top-left:
						tmpContact.x = blkX + BLOCK_W_X;
						tmpContact.y = blkY + BLOCK_W_Y;
						if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
							contactCandidates.add( tmpContact );
							
							// Vertical segment?
							if ( marginCmp(segmAngle,  90, ANG_ERR) ||
								 marginCmp(segmAngle, 270, ANG_ERR) )
								normalCandidates.add( Vector2D( -1, 0 ) );
							// Horizontal segment?
							else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
									  marginCmp(segmAngle, 180, ANG_ERR) )
								normalCandidates.add( Vector2D( 0, -1 ) );
							else
								normalCandidates.add( Vector2D( -1, -1 ).unit() );
						}
						// Top-right:
						tmpContact.x = blkX + BLOCK_E_X;
						tmpContact.y = blkY + BLOCK_E_Y;
						if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
							contactCandidates.add( tmpContact );
							
							// Vertical segment?
							if ( marginCmp(segmAngle,  90, ANG_ERR) ||
								 marginCmp(segmAngle, 270, ANG_ERR) )
								normalCandidates.add( Vector2D( 1, 0 ) );
							// Horizontal segment?
							else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
									  marginCmp(segmAngle, 180, ANG_ERR) )
								normalCandidates.add( Vector2D( 0, -1 ) );
							else
								normalCandidates.add( Vector2D( 1, -1 ).unit() );
						}
					}
					break;
					
					
					case RoomBlock::BLK_THINFLOOR_LO:
					// Only test if the segment's moving from top to bottom:
					if ( touchThinFloor && dy >= 0 ) {
						// Bottom-left:
						tmpContact.x = blkX + BLOCK_SW_X;
						tmpContact.y = blkY + BLOCK_SW_Y - 1;
						if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
							contactCandidates.add( tmpContact );
							
							// Vertical segment?
							if ( marginCmp(segmAngle,  90, ANG_ERR) ||
								 marginCmp(segmAngle, 270, ANG_ERR) )
								normalCandidates.add( Vector2D( -1, 0 ) );
							// Horizontal segment?
							else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
									  marginCmp(segmAngle, 180, ANG_ERR) )
								normalCandidates.add( Vector2D( 0, -1 ) );
							else
								normalCandidates.add( Vector2D( -1, -1 ).unit() );
						}
						// Bottom-right:
						tmpContact.x = blkX + BLOCK_SE_X;
						tmpContact.y = blkY + BLOCK_SE_Y - 1;
						if ( Geom::insidePolygon( tmpContact, pathPolygon ) ) {
							contactCandidates.add( tmpContact );
							
							// Vertical segment?
							if ( marginCmp(segmAngle,  90, ANG_ERR) ||
								 marginCmp(segmAngle, 270, ANG_ERR) )
								normalCandidates.add( Vector2D( 1, 0 ) );
							// Horizontal segment?
							else if ( marginCmp(segmAngle,   0, ANG_ERR) ||
									  marginCmp(segmAngle, 180, ANG_ERR) )
								normalCandidates.add( Vector2D( 0, -1 ) );
							else
								normalCandidates.add( Vector2D( 1, -1 ).unit() );
						}
					}
					break;
					
					
				default:
					break;
			}
		}
	}
	
	
	// Have we found any candidates at all?
	if ( contactCandidates.getCount() > 0 ) {
		
		// Calculate which contact had the shortest displacement:
		
		// Quadrilateral parametrization: Use two vectors, (u, v), where u is
		// the direction from point1 to point2 and v is the displacement.
		Vector2D p1 = Vector2D( x1, y1 );
		Vector2D u  = Vector2D( x2 - x1, y2 - y1 );
		Vector2D v  = Vector2D( dx, dy );
		u.normalize();
		v.normalize();
		
		// I will now calculate the v coordinate of all contact points.
		// The contact point with the lowest v wins!
		// Given a point p, its coordinates are:
		// uCoord = (-p.y * v.x + v.y * p.x) / (u.x * v.y - v.x * u.y)
		// vCoord = -(p.x * u.y - u.x * p.y) / (u.x * v.y - v.x * u.y)
		
		// Start with the first...
		Vector2D & cPoint   = contactCandidates[0] - p1;
		float shortestV     = -(cPoint.x * u.y - u.x * cPoint.y) / (u.x * v.y - v.x * u.y);
		
		for ( int pInx = 1; pInx < contactCandidates.getCount(); pInx++ ) {
			cPoint     = contactCandidates[pInx] - p1;
			float newV = -(cPoint.x * u.y - u.x * cPoint.y) / (u.x * v.y - v.x * u.y);
			
			if ( newV < shortestV )
				shortestV = newV;
		}
		
		
		// Now, eliminate those contacts with a v coordinate higher than the
		// shortest:
		for ( int pInx = contactCandidates.getCount() - 1; pInx >= 0; pInx-- ) {
			cPoint      = contactCandidates[pInx] - p1;
			float thisV = -(cPoint.x * u.y - u.x * cPoint.y) / (u.x * v.y - v.x * u.y);
			
			if ( thisV > shortestV + EPSILON ) {
				contactCandidates.remove( pInx );
				normalCandidates.remove( pInx );
			}
		}
		
		// Merge contact points that are close by:
		// I'll keep going as long as I can find a suitable pair to be merged
		// among the candidates.
		bool keepGoing = true;
		
		while ( keepGoing ) {
			keepGoing = false;
			
			// Look for a pair of points that's close enough to be merged:
			Vector2D c1, c2, n1, n2;
			
			for ( int inx1 = 0; inx1 < contactCandidates.getCount(); inx1++ ) {
SEARCH:
				for ( int inx2 = 0; inx2 < contactCandidates.getCount(); inx2++ ) {
					if ( inx1 != inx2 ) {
						c1 = contactCandidates[inx1];
						c2 = contactCandidates[inx2];
						
						// Test the distance between c1 and c2:
						// Merge them if they are less than one pixel apart.
						if ( (c1 - c2).length() < 1.0f ) {
							n1 = normalCandidates[inx1];
							n2 = normalCandidates[inx2];
							
							// Remove the points from the candidate list...
							if ( inx1 < inx2 ) {
								contactCandidates.remove( inx2 );
								normalCandidates.remove( inx2 );
								contactCandidates.remove( inx1 );
								normalCandidates.remove( inx1 );
							}
							else {
								contactCandidates.remove( inx1 );
								normalCandidates.remove( inx1 );
								contactCandidates.remove( inx2 );
								normalCandidates.remove( inx2 );
							}
							
							// Merge points and re-insert them:
							contactCandidates.add( 0.5f * (c1 + c2) );
							normalCandidates.add( (n1 + n2).unit() );
							
							// Run the search once more:
							keepGoing = true;
							goto SEARCH;
						}
					}
				}
			}
		}
		
		
		// Now that I've calculated the contact points with the shortest
		// displacement... Store all of it!
		if ( contacts ) {
			contacts->removeAll();
			for ( int pInx = 0; pInx < contactCandidates.getCount(); pInx++ ) {
				contacts->add( contactCandidates[pInx] );
			}
		}
		
		if ( normals ) {
			normals->removeAll();
			for ( int pInx = 0; pInx < contactCandidates.getCount(); pInx++ ) {
				normals->add( normalCandidates[pInx] );
			}
		}
		
		if ( validDisplacement )
			// Store the valid displacement, which is the shortest v value.
			*validDisplacement = v * shortestV;
		
		
		return true;
	}
	// No contacts!
	else
		return false;
	
} // End of method: Room::segmentCollision (linear moving segment)	
