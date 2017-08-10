#include "Geom.h"
#include <cmath>

//
// Auxiliary function:
// Tests if point P2 is Left|On|Right of the line P0 to P1.
//      returns: >0 for left, 0 for on, and <0 for right of the line.
//
static inline float isLeft ( float p0X, float p0Y,
                             float p1X, float p1Y,
                             float p2X, float p2Y ) 
{
	return (p1X - p0X)*(p2Y - p0Y) - (p2X - p0X)*(p1Y - p0Y);
}

///////////////////////////////////////////////////////////////////////////////
// 
// Public methods:
// 
///////////////////////////////////////////////////////////////////////////////

bool Geom::segmentIntersection ( float s1x1, float s1y1,
                                 float s1x2, float s1y2,
                                 float s2x1, float s2y1, 
                                 float s2x2, float s2y2,
                                 Vector2D * point ) {
	
	float lsign, rsign;
	bool  p1Overlap = false, p2Overlap = false;
	
	lsign = isLeft(s1x1, s1y1, s1x2, s1y2, s2x1, s2y1);    // s2 left point sign
	rsign = isLeft(s1x1, s1y1, s1x2, s1y2, s2x2, s2y2);    // s2 right point sign
	
	if (lsign * rsign > 0) // s2 endpoints have same sign relative to s1
		return false;      // => on same side => no intersect is possible
	else if ( abs(lsign) < EPSILON && abs(rsign) < EPSILON )
		p1Overlap = true;
	
	lsign = isLeft(s2x1, s2y1, s2x2, s2y2, s1x1, s1y1);    // s1 left point sign
	rsign = isLeft(s2x1, s2y1, s2x2, s2y2, s1x2, s1y2);    // s1 right point sign
	
	if (lsign * rsign > 0) // s1 endpoints have same sign relative to s2
		return false;      // => on same side => no intersect is possible
	else if ( abs(lsign) < EPSILON && abs(rsign) < EPSILON )
		p2Overlap = true;
	
	// Segments overlap?
	if ( p1Overlap && p2Overlap ) {
		float minS1x = std::min(s1x1, s1x2), maxS1x = std::min(s1x1, s1x2);
		float minS1y = std::min(s1y1, s1y2), maxS1y = std::min(s1y1, s1y2);
		float minS2x = std::min(s2x1, s2x2), maxS2x = std::max(s2x1, s2x2);
		float minS2y = std::min(s2y1, s2y2), maxS2y = std::max(s2y1, s2y2);
		
		// Segment1 not contained in segment2?
		if ( maxS1x < minS2x || maxS2x < minS1x ||
			maxS1y < minS2y || maxS2y < minS1y )
			return false;
	}
	
	// the segments s1 and s2 straddle each other
	
	// Calculate intersection point...?
	if ( point ) {
		// Special cases:
		float dx1 = s1x1 - s1x2, dx2 = s2x1 - s2x2;
		if ( abs(dx1) < EPSILON && abs(dx2) < EPSILON ) {
			// Both segments are vertical!
			// Set the intersection at the midpoint:
			point->x = s1x1;
			point->y = (s1y1 + s1y2)/2;
		}
		else if ( abs(dx1) < EPSILON ) {
			// Segment 1 is vertical!
			point->x = s1x1;
			
			// Use proportions to calculate the Y coordinate in segment 2:
			float horizProportion = abs((float) (s1x1 - s2x1) / dx2);
			point->y = s2y1 + (horizProportion * (s2y2 - s2y1));
		}
		else if ( abs(dx2) < EPSILON ) {
			// Segment 2 is vertical!
			point->x = s2x1;
			
			// Use proportions to calculate the Y coordinate in segment 1:
			float horizProportion = abs((float) (s2x1 - s1x1) / dx1);
			point->y = s1y1 + (horizProportion * (s1y2 - s1y1));
		}
		else {
			// y = a * x + b
			float a1, a2, b1, b2;
			
			// Sort points by X coordinate:
			if ( s1x1 > s1x2 )
				a1 = (s1y1 - s1y2)/(s1x1 - s1x2);
			else
				a1 = (s1y2 - s1y1)/(s1x2 - s1x1);
			b1 = s1y1 - a1 * s1x1;
			
			if ( s2x1 > s2x2 )
				a2 = (s2y1 - s2y2)/(s2x1 - s2x2);
			else
				a2 = (s2y2 - s2y1)/(s2x2 - s2x1);
			b2 = s2y1 - a2 * s2x1;
			
			// Intersection:
			point->x = ((b2 - b1)/(a1 - a2));
			point->y = (a1 * (point->x) + b1);
		}
	}
		
	return true;           // => an intersect exists
} // End of method: segmentIntersection


bool Geom::insidePolygon ( const Vector2D & p, const Vector2D * points, int nPoints ) {
	int counter = 0;
	int i;
	double xinters;
	Vector2D p1, p2;
	
	p1 = points[0];
	for ( i = 1; i <= nPoints; i++ ) {
		p2 = points[i % nPoints];
		if (p.y > std::min(p1.y, p2.y)) {
			if (p.y <= std::max(p1.y, p2.y)) {
				if (p.x <= std::max(p1.x, p2.x)) {
					if (p1.y != p2.y) {
						xinters = (p.y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y) + p1.x;
						if (p1.x == p2.x || p.x <= xinters)
							counter++;
					}
				}
			}
		}
		p1 = p2;
	}
	
	if (counter % 2 == 0)
		return false;
	else
		return true;
} // End of method: insidePolygon


void Geom::renderPolygon ( const Vector2D * points, int nPoints, float translateX, float translateY, unsigned long color ) {
	/*
	// Get an HGE interface:
	HGE * hge = hgeCreate( HGE_VERSION );
	
	// Iterate over all points:
	for ( int i = 0, j = 1; i < nPoints; i++, j++ ) {
		// "j" index has passed the container's limits?
		if ( j >= nPoints )
			j = 0;
		
		hge->Gfx_RenderLine(
			points[i].x + translateX, // x1
			points[i].y + translateY, // y1
			points[j].x + translateX, // x2
			points[j].y + translateY, // y2
			color );                  // color
	}
	
	// Release HGE interface:
	hge->Release();
	*/

} // End of method: Geom::renderPolygon


int Geom::segmentCircleIntersection ( float xCenter, float yCenter,
                                      float radius,
                                      float x1, float y1,
                                      float x2, float y2,
                                      Vector2D * pvIntPoint1,
                                      Vector2D * pvIntPoint2 ) {
	Vector2D vRay( x2 - x1, y2 - y1 );
	Vector2D vCircleToP1( x1 - xCenter, y1 - yCenter );
	
	// Solve a quadratic equation:
	float a = vRay * vRay;
	float b = 2 * vRay * vCircleToP1;
	float c = vCircleToP1 * vCircleToP1 - radius * radius;
	
	float delta = (b * b) - 4 * a * c;
	
	// No solution?
	if ( delta < -EPSILON ) {
		return 0;
	}
	
	// Calculate solutions!
	// Only one solution?
	if ( delta < EPSILON ) {
		// Line is tangential to the circle, but does the intersection point
		// lie on the segment?
		float fIntersDist = (-b / (2 * a));
		
		if ( fIntersDist >= 0 && fIntersDist <= 1 ) {
			// Intersection! Store it?
			if ( pvIntPoint1 ) {
				float fLength = vRay.length();
				vRay /= fLength;
				
				pvIntPoint1->x = x1;
				pvIntPoint1->y = y1;
				(*pvIntPoint1) += vRay * fIntersDist * fLength;
			}
			
			return 1;
		}
		else {
			// No intersection.
			return 0;
		}
	}
	// Two solutions.
	else {
		// Line is secant to the circle, but do the intersection points
		// lie on the segment?
		float fRoot = sqrtf( delta );
		float fIntersDist1 = ((-b + fRoot) / (2 * a));
		float fIntersDist2 = ((-b - fRoot) / (2 * a));
		
		float fLength = vRay.length();
		vRay /= fLength;
		
		int iNumResults = 0;
		
		if ( fIntersDist1 >= 0 && fIntersDist1 <= 1 ) {
			// Intersection #1!
			iNumResults++;
			
			// Store it?
			if ( pvIntPoint1 ) {
				pvIntPoint1->x = x1;
				pvIntPoint1->y = y1;
				(*pvIntPoint1) += vRay * fIntersDist1 * fLength;
			}
		}
		
		if ( fIntersDist2 >= 0 && fIntersDist2 <= 1 ) {
			// Intersection #2!
			iNumResults++;
			
			// Store it?
			Vector2D * pvResult = (iNumResults == 1 ? pvIntPoint1 : pvIntPoint2);
			
			if ( pvResult ) {
				pvResult->x = x1;
				pvResult->y = y1;
				(*pvResult) += vRay * fIntersDist2 * fLength;
			}
		}
		
		return iNumResults;
	}
	
} // End of method: Geom::segmentCircleIntersection


void Geom::renderCircle ( float xCenter, float yCenter, float radius, unsigned int color ) {
	/*
	radius = ABS(radius);
	
	// Get an HGE interface:
	HGE * hge = hgeCreate( HGE_VERSION );
	
	// Calculate decent number of divisions for the circle's outline:
	#define CIRCLE_MAX_SEGMLENGTH 5.0f
	#define CIRCLE_MIN_DIVISIONS  8
	int nDiv = (int) ceil((radius * 2 * M_PI) / CIRCLE_MAX_SEGMLENGTH);
	nDiv = MAX(nDiv, CIRCLE_MIN_DIVISIONS);
	
	for ( int i = 0; i < nDiv; i++ ) {
		float angle1 = i * 2 * M_PI / nDiv;
		float angle2 = (i + 1) * 2 * M_PI / nDiv;
		
		hge->Gfx_RenderLine(
			xCenter + radius * cosf( angle1 ), yCenter + radius * sinf( angle1 ), 
			xCenter + radius * cosf( angle2 ), yCenter + radius * sinf( angle2 ), 
			color );
	}
		
	hge->Release();
	*/
} // End of method: Geom::renderCircle


#define MINUSEPSILON -0.0001
void Geom::clipPolygon ( Container<Vector2D> * poly,
                         const Vector2D & planeP,
                         const Vector2D & planeN ) {
	// Start with an empty polygon:
	static Container<Vector2D> result;
	result.removeAll();
	
	// For each segment in the original polygon...
	for ( int i = 0; i < poly->getCount(); i++ ) {
		// Get the segment's two vertices:
		Vector2D v1 = poly->elem(i);
		Vector2D v2 = poly->elem((i + 1) % poly->getCount());
		
		Vector2D pToV1 = v1 - planeP;
		Vector2D pToV2 = v2 - planeP;
		
		float dotProd1 = pToV1 * planeN;
		float dotProd2 = pToV2 * planeN;
		
		// Both outside of clipping region?
		if ( dotProd1 < MINUSEPSILON && dotProd2 < MINUSEPSILON ) {
			// Don't add.
		}
		else
		// Both inside clipping region?
		if ( dotProd1 >= MINUSEPSILON && dotProd2 >= MINUSEPSILON ) {
			// Add v1:
			result.add( v1 );
		}
		else
		// Segment entering the region?
		if ( dotProd1 < MINUSEPSILON && dotProd2 >= MINUSEPSILON ) {
			// Add intersection of segment with the clipping plane.
			Vector2D v1ToV2 = v2; v1ToV2 -= v1;
			Vector2D v1ToPlane = (planeP - v1).project( planeN );
			Vector2D v1ToV2OrthoPlane = v1ToV2.project( planeN );
			
			float scaleFactor = v1ToPlane.length() / v1ToV2OrthoPlane.length();
			
			result.add( v1 + (v1ToV2 * scaleFactor) );
		}
		else
		// Segment leaving the region?
		{
			// Add v1...
			result.add( v1 );
			
			// ...and the intersection of segment with the clipping plane:
			Vector2D v1ToV2 = v2; v1ToV2 -= v1;
			Vector2D v1ToPlane = (planeP - v1).project( planeN );
			Vector2D v1ToV2OrthoPlane = v1ToV2.project( planeN );
			
			float scaleFactor = v1ToPlane.length() / v1ToV2OrthoPlane.length();
			
			result.add( v1 + (v1ToV2 * scaleFactor) );
		}
	}
	
	// Store the result:
	*poly = result;
}
