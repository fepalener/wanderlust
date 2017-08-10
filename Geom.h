#ifndef _Geom_h_
#define _Geom_h_

#include "Vector2D.h"
#include "Container.h"
#include "Def.h"

/**
 * Provides static methods for useful geometric operations.
 */
class Geom
{
public:
	/**
	 * Tests for segment intersection.
	 * Stores the intersection point, if any, in the "point" vector, if
	 * it is not NULL.
	 * 
	 * @return true if the two segments intersect each other
	 */
	static bool segmentIntersection ( float s1x1, float s1y1,
			                          float s1x2, float s1y2,
									  float s2x1, float s2y1, 
			                          float s2x2, float s2y2,
			                          Vector2D * point = NULL );


	/** 
	 * Given the polygon described by <tt>points</tt>, returns true if
	 * the point <tt>p</tt> lies inside it.
	 * 
	 */ 
	static bool insidePolygon ( const Vector2D & p, const Vector2D * points, int nPoints);
			
	static inline bool insidePolygon ( const Vector2D & p, const Container<Vector2D> & points ) {
		return insidePolygon( p, points.getData(), points.getCount() );
	}
	
	/**
	 * Renders the specified polygon as a set of lines in the specified
	 * color, translated as requested.
	 */
	static void renderPolygon ( const Vector2D * points, int nPoints, float translateX, float translateY, unsigned long color );
			
	static inline void renderPolygon ( const Container<Vector2D> & points, float translateX, float translateY, unsigned long color ) {
		renderPolygon( points.getData(), points.getCount(), translateX, translateY, color );
	}


	/**
	 * Tests if a segment is intersecting a circle. This will
	 * <em>not</em> detect a collision if the segment is completely
	 * contained in the circle.
	 * 
	 * There are three possible outcomes:
	 * - The segment does not intersect the circle. The method returns
	 *   zero; <tt>intPoint1</tt> and <tt>intPoint2</tt> are left
	 *   unchanged.
	 * - The segment intersects the circle at just one point. The
	 *   method returns 1; if <tt>pvIntPoint1</tt> is not NULL, the
	 *   intersection point will be stored there.
	 * - The segment intersects the circle at two points. The method
	 *   returns 2; if <tt>pvIntPoint1</tt> and <tt>pvIntPoint2</tt>
	 *   are not NULL, the intersection points will be stored there.
	 * 
	 * @param xCenter     X coordinate of the circle's center.
	 * @param yCenter     Y coordinate of the circle's center.
	 * @param radius      Circle's radius.
	 * @param x1          X coordinate of the segment's first point.
	 * @param y1          Y coordinate of the segment's first point.
	 * @param x2          X coordinate of the segment's second point.
	 * @param y2          Y coordinate of the segment's second point.
	 * @param pvIntPoint1 Return value of the first intersection point.
	 * @param pvIntPoint2 Return value of the second intersection point.
	 */
	static int segmentCircleIntersection ( float xCenter, float yCenter,
			                               float radius,
			                               float x1, float y1,
			                               float x2, float y2,
										   Vector2D * pvIntPoint1 = NULL,
			                               Vector2D * pvIntPoint2 = NULL );
			
	/**
	 * Renders a circle as a series of lines. The method attempts to
	 * adapt the number of segments to the size of the circle so it
	 * will not appear too fragmented.
	 */
	static void renderCircle ( float xCenter, float yCenter, float radius, unsigned int color );
			
	/**
	 * The infamous "fast inverse square root" function.
	 */
	static inline float invSqrt ( float x ) {
		float xhalf = 0.5f*x;
		int i = *(int*)&x; // get bits for floating value
		i = 0x5f3759df - (i>>1); // gives initial guess y0
		x = *(float*)&i; // convert bits back to float
		x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
				
		return x;
	}

	/**
	 * Clips the polygon so it will be contained within a semiplane.
	 * @param poly   The polygon to be clipped. The resulting (clipped) polygon
	 *               will be stored here as well.
	 * @param planeP A point determining the clipping plane's position.
     * @param planeD The clipping plane's normal, pointing towards the interior of
	 *               the clipping region.
	 */
	static void clipPolygon ( Container<Vector2D> * poly,
			                  const Vector2D & planeP,
			                  const Vector2D & planeN );
};

#endif