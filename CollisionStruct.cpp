#include "CollisionStruct.h"
#include "Geom.h"


CollisionStruct::~CollisionStruct () {}

///////////////////////////////////////////////////////////////////////////////
// 
// BOX COLLISION STUCTURE:
// 
///////////////////////////////////////////////////////////////////////////////

CollisionStruct::Box::Box ( float _x, float _y, float _w, float _h ) {
	x = _x;
	y = _y;
	w = _w;
	h = _h;
}

CollisionStruct::Box::Box () {
	x = 0;
	y = 0;
	w = 0;
	h = 0;
}

bool CollisionStruct::Box::collidesWith ( CollisionStruct * otherStruct ) {
	
	if ( !otherStruct )
		return false;
	
	// What kind of struct is that?
	CollisionStruct::Box       * otherBox     = NULL;
	CollisionStruct::Circle    * otherCircle  = NULL;
	CollisionStruct::Composite * otherCompo   = NULL;
	CollisionStruct::Polygon   * otherPolygon = NULL;
	
	if ( (otherBox = dynamic_cast<CollisionStruct::Box*>( otherStruct )) ) {
		// Box with box... easiest of all:
		
		float top   = std::min( y, y + h );
		float bott  = std::max( y, y + h );
		float left  = std::min( x, x + w );
		float right = std::max( x, x + w );
		
		float otherTop   = std::min( otherBox->y, otherBox->y + otherBox->h );
		float otherBott  = std::max( otherBox->y, otherBox->y + otherBox->h );
		float otherLeft  = std::min( otherBox->x, otherBox->x + otherBox->w );
		float otherRight = std::max( otherBox->x, otherBox->x + otherBox->w );
		
		return !(right < otherLeft  ||
		         left  > otherRight ||
		         bott  < otherTop   ||
		         top   > otherBott);
	}
	else
	if ( (otherCircle = dynamic_cast<CollisionStruct::Circle*>( otherStruct )) ) {
		// Box with circle:
		return CollisionStruct::collide_Box_Circle( this, otherCircle );
	}
	else
	if ( (otherCompo = dynamic_cast<CollisionStruct::Composite*>( otherStruct )) ) {
		// Box with composite:
		// Test against all other structs:
		for ( int i = 0; i < otherCompo->getCount(); i++ )
			if ( collidesWith( otherCompo->elem( i ) ) )
				return true;
		
		// No collision:
		return false;
	}
	else
	if ( (otherPolygon = dynamic_cast<CollisionStruct::Polygon*>( otherStruct )) ) {
		// Box with polygon:
		return CollisionStruct::collide_Box_Polygon( this, otherPolygon );
	}
	else {
		// Unknown type...
		return false;
	}
	
}

bool CollisionStruct::Box::hasPoint ( float _x, float _y ) {
	return ((_x >= x) && (_x <= x + w) && (_y >= y) && (_y <= y + h));
}

bool CollisionStruct::Box::pointIntersection ( float x1, float y1,
			                                   float x2, float y2,
			                                   Vector2D * pContact,
			                                   Vector2D * pNormal ) {
	if ( hasPoint( x1, y1 ) ) {
		if ( pContact ) {
			pContact->x = x1;
			pContact->y = y1;
		}
		if ( pNormal ) {
			pNormal->x = x1 - x2;
			pNormal->y = y1 - y2;
			pNormal->normalize();
		}
		return true;
	}
	
	// Test point path against all segments:
	// Top segment:
	if ( y1 <= y &&
	     Geom::segmentIntersection( x,     y,     x + w, y,     x1, y1, x2, y2, pContact ) ) {
		if ( pNormal ) {
			pNormal->x = 0;
			pNormal->y = -1;
		}
		return true;
	}
	// Bottom segment:
	if ( y1 >= y + h &&
	     Geom::segmentIntersection( x,     y + h, x + w, y + h, x1, y1, x2, y2, pContact ) ) {
		if ( pNormal ) {
			pNormal->x = 0;
			pNormal->y = 1;
		}
		return true;
	}
	// Left segment:
	if ( x1 <= x &&
	     Geom::segmentIntersection( x,     y,     x,     y + h, x1, y1, x2, y2, pContact ) ) {
		if ( pNormal ) {
			pNormal->x = -1;
			pNormal->y = 0;
		}
		return true;
	}
	// Right segment:
	if ( x1 >= x + w &&
	     Geom::segmentIntersection( x + w, y,     x + w, y + h, x1, y1, x2, y2, pContact ) ) {
		if ( pNormal ) {
			pNormal->x = 1;
			pNormal->y = 0;
		}
		return true;
	}
	
	// No collision:
	return false;
}

void CollisionStruct::Box::move ( float dx, float dy ) {
	x += dx;
	y += dy;
}

void CollisionStruct::Box::render ( float scrollX, float scrollY, unsigned long color ) {
	/*
	// Get an HGE interface:
	HGE * hge = hgeCreate( HGE_VERSION );
	
	// Render lines:
	hge->Gfx_RenderLine( x     - scrollX, y     - scrollY, x + w - scrollX, y     - scrollY, color );
	hge->Gfx_RenderLine( x + w - scrollX, y     - scrollY, x + w - scrollX, y + h - scrollY, color );
	hge->Gfx_RenderLine( x + w - scrollX, y + h - scrollY, x     - scrollX, y + h - scrollY, color );
	hge->Gfx_RenderLine( x     - scrollX, y + h - scrollY, x     - scrollX, y     - scrollY, color );
	
	hge->Release();
	*/
}



///////////////////////////////////////////////////////////////////////////////
// 
// CIRCLE COLLISION STUCTURE:
// 
///////////////////////////////////////////////////////////////////////////////

CollisionStruct::Circle::Circle ( float _xCenter, float _yCenter, float _radius ) {
	xCenter = _xCenter;
	yCenter = _yCenter;
	radius = _radius;
}

CollisionStruct::Circle::Circle () {
	xCenter = 0;
	yCenter = 0;
	radius = 0;
}

bool CollisionStruct::Circle::collidesWith ( CollisionStruct * otherStruct ) {
	
	if ( !otherStruct )
		return false;
	
	// What kind of struct is that?
	CollisionStruct::Box       * otherBox     = NULL;
	CollisionStruct::Circle    * otherCircle  = NULL;
	CollisionStruct::Composite * otherCompo   = NULL;
	CollisionStruct::Polygon   * otherPolygon = NULL;
	
	if ( (otherBox = dynamic_cast<CollisionStruct::Box*>( otherStruct )) ) {
		// Box with circle:
		return CollisionStruct::collide_Box_Circle( otherBox, this );
	}
	else
	if ( (otherCircle = dynamic_cast<CollisionStruct::Circle*>( otherStruct )) ) {
		// Circle with circle:
		// Distance between centers must be less than sum of radii:
		float dx = xCenter - otherCircle->xCenter;
		float dy = yCenter - otherCircle->yCenter;
		
		return (sqrtf(dx * dx + dy * dy) <= (radius + otherCircle->radius));
	}
	else
	if ( (otherCompo = dynamic_cast<CollisionStruct::Composite*>( otherStruct )) ) {
		// Circle with composite:
		// Test against all other structs:
		for ( int i = 0; i < otherCompo->getCount(); i++ )
			if ( collidesWith( otherCompo->elem( i ) ) )
				return true;
		
		// No collision:
		return false;
	}
	else
	if ( (otherPolygon = dynamic_cast<CollisionStruct::Polygon*>( otherStruct )) ) {
		// Circle with polygon:
		return CollisionStruct::collide_Circle_Polygon( this, otherPolygon );
	}
	else {
		// Unknown type...
		return false;
	}
	
}

bool CollisionStruct::Circle::hasPoint ( float x, float y ) {
	float dx = xCenter - x;
	float dy = yCenter - y;
	
	return (sqrtf(dx * dx + dy * dy) <= radius);
}

bool CollisionStruct::Circle::pointIntersection ( float x1, float y1,
                                                  float x2, float y2,
                                                  Vector2D * pContact,
                                                  Vector2D * pNormal ) {
	if ( hasPoint( x1, y1 ) ) {
		if ( pContact ) {
			pContact->x = x1;
			pContact->y = y1;
		}
		if ( pNormal ) {
			pNormal->x = x1 - x2;
			pNormal->y = y1 - y2;
			pNormal->normalize();
		}
		return true;
	}
	
	// Ask for Geom's help:
	Vector2D vPoint1, vPoint2;
	int nPoints = Geom::segmentCircleIntersection( xCenter, yCenter, radius, x1, y1, x2, y2, &vPoint1, &vPoint2 );
	
	// No intersection?
	if ( nPoints == 0 )
		return false;
	// One intersection?
	if ( nPoints == 1 ) {
		if ( pContact )
			*pContact = vPoint1;
		if ( pNormal ) {
			// From center to contact point:
			pNormal->x = vPoint1.x - xCenter;
			pNormal->y = vPoint1.y - yCenter;
			pNormal->normalize();
		}
		return true;
	}
	// Two intersections:
	// Pick closest one:
	Vector2D vSrc(x1, y1);
	float fDist1 = (vPoint1 - vSrc).length();
	float fDist2 = (vPoint2 - vSrc).length();
	
	if ( fDist1 < fDist2 ) {
		if ( pContact )
			*pContact = vPoint1;
		if ( pNormal ) {
			// From center to contact point:
			pNormal->x = vPoint1.x - xCenter;
			pNormal->y = vPoint1.y - yCenter;
			pNormal->normalize();
		}
	}
	else {
		if ( pContact )
			*pContact = vPoint2;
		if ( pNormal ) {
			// From center to contact point:
			pNormal->x = vPoint2.x - xCenter;
			pNormal->y = vPoint2.y - yCenter;
			pNormal->normalize();
		}
	}
	
	return true;
}

void CollisionStruct::Circle::move ( float dx, float dy ) {
	xCenter += dx;
	yCenter += dy;
}

void CollisionStruct::Circle::render ( float scrollX, float scrollY, unsigned long color ) {
	//Geom::renderCircle( xCenter - scrollX, yCenter - scrollY, radius, color );
}


///////////////////////////////////////////////////////////////////////////////
// 
// COMPOSITE COLLISION STRUCTURE:
// 
///////////////////////////////////////////////////////////////////////////////

CollisionStruct::Composite::Composite ( CollisionStruct * const elems[], int nCount )
: Container<CollisionStruct*>( elems, nCount )
{
	// Nothing to do here.
}

CollisionStruct::Composite::Composite () {
	// Nothing to do here.
}

void CollisionStruct::Composite::render ( float scrollX, float scrollY, unsigned long color ) {
	// Forward the call to all elements:
	/*
	for ( int i = 0; i < getCount(); i++ )
		elem( i )->render( scrollX, scrollY, color );
	*/
}

bool CollisionStruct::Composite::collidesWith ( CollisionStruct * otherStruct ) {
	// Forward the call to all elements:
	for ( int i = 0; i < getCount(); i++ ) {
		if ( elem( i )->collidesWith( otherStruct ) ) {
			return true;
		}
	}
	
	// No collisions found:
	return false;
}

bool CollisionStruct::Composite::hasPoint ( float x, float y ) {
	// Forward the call to all elements:
	for ( int i = 0; i < getCount(); i++ ) {
		if ( elem( i )->hasPoint( x, y ) ) {
			return true;
		}
	}
	
	// No collisions found:
	return false;
}

bool CollisionStruct::Composite::pointIntersection ( float x1, float y1,
                                                     float x2, float y2,
                                                     Vector2D * pContact,
                                                     Vector2D * pNormal ) {
	Vector2D vSrc( x1, y1 );
	float fMinDist = FLT_MAX;
	Vector2D vMinContact, vMinNormal;
	
	// Forward the call to all elements:
	for ( int i = 0; i < getCount(); i++ ) {
		Vector2D vContact, vNormal;
		if ( elem( i )->pointIntersection( x1, y1, x2, y2, &vContact, &vNormal ) ) {
			// Intersection found.
			// Don't care about contact or normal?
			if ( !pContact && !pNormal )
				return true;
			
			// Is it closer?
			float fDist = (vContact - vSrc).length();
			if ( fDist < fMinDist ) {
				fMinDist = fDist;
				vMinContact = vContact;
				vMinNormal = vNormal;
			}
		}
	}
	
	// Found a contact?
	if ( fMinDist != FLT_MAX ) {
		if ( pContact )
			*pContact = vMinContact;
		if ( pNormal )
			*pNormal = vMinNormal;
		
		return true;
	}
	else
		return false;
}

void CollisionStruct::Composite::move ( float dx, float dy ) {
	// Forward the call to all elements:
	for ( int i = 0; i < getCount(); i++ )
		elem( i )->move( dx, dy );
}


///////////////////////////////////////////////////////////////////////////////
// 
// POLYGON COLLISION STUCTURE:
// 
///////////////////////////////////////////////////////////////////////////////

CollisionStruct::Polygon::Polygon () {
	// Set translation to zero:
	pos.x = 0;
	pos.y = 0;
	
	// Set rotation to zero:
	rotation = 0;
	
	// Set scaling to 100%:
	scale = 1;
}

CollisionStruct::Polygon::Polygon ( const Container<Vector2D> & _points )
: userPoints( _points ), realPoints( _points )
{
	// Set translation to zero:
	pos.x = 0;
	pos.y = 0;
	
	// Set rotation to zero:
	rotation = 0;
	
	// Set scaling to 100%:
	scale = 1;
}

CollisionStruct::Polygon::Polygon ( const Vector2D * _points, int nPoints )
: userPoints( _points, nPoints ), realPoints( _points, nPoints )
{
	// Set translation to zero:
	pos.x = 0;
	pos.y = 0;
	
	// Set rotation to zero:
	rotation = 0;
	
	// Set scaling to 100%:
	scale = 1;
}

void CollisionStruct::Polygon::render ( float scrollX, float scrollY, unsigned long color ) {
	// Ask the "Geom" module to render the polygon:
	/*
	Geom::renderPolygon(
		realPoints,
		(float) -scrollX,
		(float) -scrollY,
		color );
	*/
}

void CollisionStruct::Polygon::setPoints( const Vector2D * points, int nPoints ) {
	// Copy all points to the userPoints container... 
	userPoints.removeAll();
	
	for ( int i = 0; i < nPoints; i++ )
		userPoints.add( points[i] );
	
	// Now, recalculate the real points:
	updateRealPoints();
}

void CollisionStruct::Polygon::setPosition ( float x, float y ) {
	// Set the position:
	pos.x = x;
	pos.y = y;
	
	// Update real points:
	updateRealPoints();
}

void CollisionStruct::Polygon::getPosition ( float * x, float * y ) {
	*x = pos.x;
	*y = pos.y;
}

void CollisionStruct::Polygon::setRotation ( float radians ) {
	// Set the rotation:
	rotation = radians;
	
	// Update real points:
	updateRealPoints();
}

float CollisionStruct::Polygon::getRotation () {
	return rotation;
}

bool CollisionStruct::Polygon::collidesWith ( CollisionStruct * otherStruct ) {
	
	if ( !otherStruct )
		return false;
	
	// What kind of struct is that?
	CollisionStruct::Box       * otherBox     = NULL;
	CollisionStruct::Circle    * otherCircle  = NULL;
	CollisionStruct::Composite * otherCompo   = NULL;
	CollisionStruct::Polygon   * otherPolygon = NULL;
	
	if ( (otherBox = dynamic_cast<CollisionStruct::Box*>( otherStruct )) ) {
		// Box with polygon:
		return CollisionStruct::collide_Box_Polygon( otherBox, this );
	}
	else if ( (otherCircle = dynamic_cast<CollisionStruct::Circle*>( otherStruct )) ) {
		// Circle with polygon:
		return CollisionStruct::collide_Circle_Polygon( otherCircle, this );
	}
	else
	if ( (otherCompo = dynamic_cast<CollisionStruct::Composite*>( otherStruct )) ) {
		// Composite with polygon:
		// Test against all other structs:
		for ( int i = 0; i < otherCompo->getCount(); i++ )
			if ( collidesWith( otherCompo->elem( i ) ) )
				return true;
		
		// No collision:
		return false;
	}
	else if ( (otherPolygon = dynamic_cast<CollisionStruct::Polygon*>( otherStruct )) ) {
		// Test polygon-polygon intersection:
		return CollisionStruct::collide_PolyPoints(
			realPoints.getData(), realPoints.getCount(),
			otherPolygon->realPoints.getData(), otherPolygon->realPoints.getCount() );
	}
	else {
		// Unknown type...
		return false;
	}
}

bool CollisionStruct::Polygon::hasPoint ( float x, float y ) {
	return Geom::insidePolygon( Vector2D(x, y), realPoints );
}

bool CollisionStruct::Polygon::pointIntersection ( float x1, float y1,
                                                   float x2, float y2,
                                                   Vector2D * pContact,
                                                   Vector2D * pNormal ) {
	if ( hasPoint( x1, y1 ) ) {
		if ( pContact ) {
			pContact->x = x1;
			pContact->y = y1;
		}
		if ( pNormal ) {
			pNormal->x = x1 - x2;
			pNormal->y = y1 - y2;
			pNormal->normalize();
		}
		return true;
	}
	
	Vector2D vSrc(x1, y1);
	
	// Test all segments, and store the closest point.
	float fMinDist = FLT_MAX;
	Vector2D vMinContact, vMinDir;
	for ( int i = 0; i < realPoints.getCount(); i++ ) {
		int j = (i + 1) % realPoints.getCount();
		const Vector2D &v1 = realPoints[i],
		               &v2 = realPoints[j];
		
		// This segment intersects?
		Vector2D vContact;
		if ( Geom::segmentIntersection( x1,   y1,   x2,   y2,
		                                v1.x, v1.y, v2.x, v2.y,
		                                &vContact ) ) {
			// Don't care about position?
			if ( !pContact && !pNormal ) {
				return true;
			}
			
			// Calculate distance:
			float fDist = (vContact - vSrc).length();
			
			// Closer?
			if ( fDist < fMinDist ) {
				fMinDist = fDist;
				vMinContact = vContact;
				vMinDir = (v1 - v2);
			}
		}
	}
	
	// Found an intersection?
	if ( fMinDist != FLT_MAX ) {
		// Store contact point:
		if ( pContact )
			*pContact = vMinContact;
		
		// Calculate normal?
		if ( pNormal ) {
			// Rotate vMinDir 90 degrees, towards the source:
			vMinDir = vMinDir.rotate( M_PI/2 );
			
			// Wrong direction?
			if ( (vMinDir * (vMinContact - vSrc)) > 0 )
				vMinDir *= -1;
			
			// Store it:
			*pNormal = vMinDir;
			pNormal->normalize();
		}
		
		return true;
	}
	// No intersection found.
	else {
		return false;
	}
}

void CollisionStruct::Polygon::move ( float dx, float dy ) {
	setPosition( pos.x + dx, pos.y + dy );
}

void CollisionStruct::Polygon::updateRealPoints () {
	realPoints.removeAll();
	
	for ( int i = 0; i < userPoints.getCount(); i++ )
		realPoints.add( scale * userPoints[i].rotate( rotation ) + pos );
}



///////////////////////////////////////////////////////////////////////////////
// 
// Static methods:
// 
///////////////////////////////////////////////////////////////////////////////

bool CollisionStruct::collide_PolyPoints ( const Vector2D * p1, int n1, const Vector2D * p2, int n2 ) {
	// Here's the strategy:
	// - For every point in p2, test if it is contained in p1.
	// - If that fails: For every point in p2, test if it is contained in p2.
	// - If that fails: For every two segments (one from each polygon), test if
	//   they intersect.
	int i2;
	for ( i2 = 0; i2 < n2; i2++ )
		if ( Geom::insidePolygon( p2[i2], p1, n1 ) )
			// Collision!
			return true;
	
	int i1;
	for ( i1 = 0; i1 < n1; i1++ )
		if ( Geom::insidePolygon( p1[i1], p2, n2 ) )
			// Collision!
			return true;
	
	for ( i1 = 0; i1 < n1; i1++ ) {
		Vector2D v1 = p1[i1];
		Vector2D v1Next = p1[((i1 + 1) % n1)];
		
		for ( i2 = 0; i2 < n2; i2++ ) {
			Vector2D v2 = p2[i2];
			Vector2D v2Next = p2[((i2 + 1) % n2)];
			
			if ( Geom::segmentIntersection(
					v1.x, v1.y,
					v1Next.x, v1Next.y,
					v2.x, v2.y, 
					v2Next.x, v2Next.y,
					NULL ) )
				// Collision!
				return true;
		}
	}
	
	// No collision!
	return false;
}

bool CollisionStruct::collide_Box_Circle ( Box *pBox, Circle *pCirc ) {
	// Here's the strategy:
	// - Test if the circle's center is contained in the box.
	// - For every corner of the box, test if it is contained in the circle.
	// - For every segment of the box, test if it intersects the circle.
	
	// Circle center in box?
	if ( pCirc->xCenter >= pBox->x &&
	     pCirc->xCenter <= pBox->x + pBox->w &&
	     pCirc->yCenter >= pBox->y &&
	     pCirc->yCenter <= pBox->y + pBox->h )
		return true;
	
	// Box corners in circle?
	// Top-left:
	float dx = pBox->x - pCirc->xCenter;
	float dy = pBox->y - pCirc->yCenter;
	if ( sqrtf( dx * dx + dy * dy ) <= pCirc->radius )
		return true;
	// Top-right:
	dx += pBox->w;
	if ( sqrtf( dx * dx + dy * dy ) <= pCirc->radius )
		return true;
	// Bottom-right:
	dy += pBox->h;
	if ( sqrtf( dx * dx + dy * dy ) <= pCirc->radius )
		return true;
	// Bottom-left:
	dx -= pBox->w;
	if ( sqrtf( dx * dx + dy * dy ) <= pCirc->radius )
		return true;
	
	// Box segments intersect circle?
	// Top:
	if ( Geom::segmentCircleIntersection(
			pCirc->xCenter, pCirc->yCenter, pCirc->radius,  
			pBox->x,           pBox->y,
			pBox->x + pBox->w, pBox->y ) )
		return true;
	// Bottom:
	if ( Geom::segmentCircleIntersection(
			pCirc->xCenter, pCirc->yCenter, pCirc->radius,  
			pBox->x,           pBox->y + pBox->h,
			pBox->x + pBox->w, pBox->y + pBox->h ) )
		return true;
	// Left:
	if ( Geom::segmentCircleIntersection(
			pCirc->xCenter, pCirc->yCenter, pCirc->radius,  
			pBox->x, pBox->y,
			pBox->x, pBox->y + pBox->h ) )
		return true;
	// Right:
	if ( Geom::segmentCircleIntersection(
			pCirc->xCenter, pCirc->yCenter, pCirc->radius,  
			pBox->x + pBox->w, pBox->y,
			pBox->x + pBox->w, pBox->y + pBox->h ) )
		return true;
	
	// Not colliding!
	return false;
}

bool CollisionStruct::collide_Box_Polygon ( Box *pBox, Polygon *pPoly ) {
	// Here's the strategy:
	// Test the box as if it were a polygon.
	Vector2D avCorners[4];
	avCorners[0].x = pBox->x;           avCorners[0].y = pBox->y;
	avCorners[1].x = pBox->x + pBox->w; avCorners[1].y = pBox->y;
	avCorners[2].x = pBox->x + pBox->w; avCorners[2].y = pBox->y + pBox->h;
	avCorners[3].x = pBox->x;           avCorners[3].y = pBox->y + pBox->h;
	
	return collide_PolyPoints(
		avCorners, 4,
		pPoly->realPoints.getData(), pPoly->realPoints.getCount() );
}

bool CollisionStruct::collide_Circle_Polygon ( Circle *pCirc, Polygon *pPoly ) {
	// Here's the strategy:
	// - Test if the circle's center is contained in the polygon.
	// - For vertex corner of the polygon, test if it is contained in the circle.
	// - For every segment of the polygon, test if it intersects the circle.
	
	// Circle center in polygon?
	if ( Geom::insidePolygon( Vector2D(pCirc->xCenter, pCirc->yCenter), pPoly->realPoints ) )
		return true;
	
	// Polygon vertices in circle?
	int i;
	for ( i = 0; i < pPoly->realPoints.getCount(); i++ ) {
		// Vertex-to-center distance less than circle radius?
		float dx = pPoly->realPoints[i].x - pCirc->xCenter;
		float dy = pPoly->realPoints[i].y - pCirc->yCenter;
		
		if ( sqrtf( dx * dx + dy * dy ) < pCirc->radius )
			return true;
	}
	
	// Polygon segments against circle?
	for ( i = 0; i < pPoly->realPoints.getCount(); i++ ) {
		Vector2D v1 = pPoly->realPoints[i];
		Vector2D v2 = pPoly->realPoints[(i + 1) % pPoly->realPoints.getCount()];
		
		if ( Geom::segmentCircleIntersection(
				pCirc->xCenter, pCirc->yCenter, pCirc->radius,
				v1.x, v1.y,
				v2.x, v2.y ) )
			return true;
	}
	
	// Not colliding!
	return false;
}
