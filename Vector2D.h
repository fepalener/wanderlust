#ifndef _Vector2D_h_
#define _Vector2D_h_

#include <math.h>
#include "Def.h"


/**
* Vector in the R<sup>2</sup> space, with (x, y) components.
* 
* This class overloads several operators to make evaluating formulae
* easier.
* 
* The multiplication operator (*) can be used with a
* scalar argument (i.e., a float value), or with another vector,
* in which case it stands for the dot product.
*/ 
class Vector2D 
{
public:
	float x, y;
		
	Vector2D () : x(0), y(0) {}
	Vector2D ( float _x, float _y ) : x(_x),  y(_y) {}
	Vector2D ( int _x, int _y ) : x((float) _x),  y((float) _y) {}
		
	inline Vector2D & operator =  ( const Vector2D & v );
		
	/** Returns the Euclidian norm of this vector. */
	inline float      length      () const;
		
	/** Returns the distance between the points denoted by this and that. */
	inline float      dist        ( const Vector2D & that ) const;
		
	/** Returns a copy of the vector, with length equal to 1. */
	inline Vector2D   unit        () const;
		
	/** Resizes the vector so it will have length equal to 1. */
	inline void       normalize   ();
		
	/** Returns the projection of this vector onto another. */
	inline Vector2D   project     ( const Vector2D & onto ) const;
		
	/** Returns the reflection of this vector relative to <tt>around</tt>. */
	inline Vector2D   reflect     ( const Vector2D & around ) const;
		
	/** Returns a rotated copy of this vector. */
	inline Vector2D   rotate      ( float radians ) const;
		
	/** Returns the angle between this vector and <tt>v</tt>. */
	inline float      angle       ( const Vector2D & v ) const;
		
	inline Vector2D & operator += ( const Vector2D & v );
	inline Vector2D & operator -= ( const Vector2D & v );
	inline Vector2D   operator +  ( const Vector2D & v ) const;
	inline Vector2D   operator -  ( const Vector2D & v ) const;
	inline Vector2D   operator -  () const;
		
	inline Vector2D & operator *= ( float s );
	inline Vector2D   operator *  ( float s ) const;
	inline Vector2D & operator /= ( float s );
	inline Vector2D   operator /  ( float s ) const;
		
	/** Returns the dot product of this vector and <tt>v</tt>. */
	inline float      operator *  ( const Vector2D & v ) const;
		
};
	
inline Vector2D  operator *  ( float s, const Vector2D & v );
	
	
	
Vector2D & Vector2D::operator = ( const Vector2D & v ) {
	x = v.x;
	y = v.y;
		
	return *this;
}

float Vector2D::length () const {
	return sqrtf( x * x + y * y );
}


float Vector2D::dist ( const Vector2D & that ) const {
	return sqrtf( ((x - that.x) * (x - that.x)) +
		            ((y - that.y) * (y - that.y)) );
}


Vector2D Vector2D::unit () const {
	float abs = length();
	if ( abs <= EPSILON ) // Prevent division by zero
		return (*this);
	else
		return Vector2D(
			x / abs,
			y / abs );
}

void Vector2D::normalize () {
	float abs = length();
	if ( abs <= EPSILON ) // Prevent division by zero
		return;
	else {
		x /= abs;
		y /= abs;
	}
}


Vector2D Vector2D::project ( const Vector2D & onto ) const {
	// Prevent division by zero:
	if ( fabs(onto * onto) < EPSILON ) 
		return Vector2D( 0, 0 );

	// Dot product of this with the other, divided by the dot product of the
	// other with itself:
	float newScale = ((*this) * onto)/(onto * onto);

	// Resize the other vector, and assign to this:
	return Vector2D (
		onto.x * newScale,
		onto.y * newScale );
}


Vector2D Vector2D::reflect ( const Vector2D & around ) const {
	// Projection of this vector onto the "mirror":
	Vector2D vProj = this->project( around );

	// Distance to the "mirror":
	Vector2D vIncrH ( vProj.x - x, vProj.y - y );

	// Result:
	return Vector2D (
		vProj.x + vIncrH.x,
		vProj.y + vIncrH.y );
}


Vector2D Vector2D::rotate ( float radians ) const {
	float cosR  = cosf( radians );
	float cosMR = cosf( -radians );
	float sinMR = sinf( -radians );
		
	return Vector2D (
		x * cosMR + y * sinMR,
		y * cosR - x * sinMR );
}
	
	
float Vector2D::angle ( const Vector2D & v ) const {
	float dot    = (*this) * v;
	float zCross = v.x * y - x * v.y;
		
	float absAngle = acosf(dot / length() / v.length());
		
	if ( zCross > 0 )
		return -absAngle;
	else
		return absAngle;
}


Vector2D & Vector2D::operator += ( const Vector2D & v ) {
	x += v.x;
	y += v.y;
		
	return *this;
}

Vector2D & Vector2D::operator -= ( const Vector2D & v ) {
	x -= v.x;
	y -= v.y;
		
	return *this;
}

Vector2D Vector2D::operator + ( const Vector2D & v ) const {
	return Vector2D (
		x + v.x,
		y + v.y );
}

Vector2D Vector2D::operator - ( const Vector2D & v ) const {
	return Vector2D (
		x - v.x,
		y - v.y );
}

Vector2D Vector2D::operator - () const {
	return Vector2D ( -x, -y );
}


Vector2D & Vector2D::operator *= ( float s ) {
	x *= s;
	y *= s;
		
	return *this;
}

Vector2D Vector2D::operator * ( float s ) const {
	return Vector2D (
		x * s,
		y * s );
}

Vector2D operator *  ( float s, const Vector2D & t ) {
	return Vector2D (
		t.x * s,
		t.y * s );
}

Vector2D & Vector2D::operator /= ( float s ) {
	x /= s;
	y /= s;
		
	return *this;
}

Vector2D Vector2D::operator / ( float s ) const {
	return Vector2D (
		x / s,
		y / s );
}


float Vector2D::operator * ( const Vector2D & v ) const {
	return x * v.x + y * v.y;
}

#endif
