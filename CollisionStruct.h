#ifndef _CollisionStruct_h_
#define _CollisionStruct_h_

#include "Container.h"
#include "Vector2D.h"

   /**
	* Represents a geometric boundary for use in collision tests.
	* 
	* <tt>CollisionStruct</tt> is an abstract class that represents generic
	* boundaries. In your <tt>GameObj</tt>s, you should return one of the
	* available subclasses:
	*  - <tt>CollisionStruct::Box</tt>
	*  - <tt>CollisionStruct::Circle</tt>
	*  - <tt>CollisionStruct::Polygon</tt>.
	* 
	* @warning
	* Do <em>not</em> derive from this class to implement your own collision
	* tests.
	* 
	* @see GameObj::getCollisionStruct()
	*/
class CollisionStruct {
	public:
			
		/**
		 * @return true if this struct is colliding against the supplied
		 *         pointer.
		 */
		virtual bool collidesWith ( CollisionStruct * otherStruct ) = 0;
			
		/**
		 * @return true if the point lies inside the structure's
		 * boundaries.
		 */
		virtual bool hasPoint ( float x, float y ) = 0;
			
		/**
		 * Tests a "moving point" collision.
		 * 
		 * The method returns true if the point moving from (x1, y1) to
		 * (x2, y2) will collide against the structure. The point of
		 * intersection will be stored in the <tt>pContact</tt> parameter,
		 * and its normal will be stored in <tt>pNormal</tt>. These
		 * pointers may be NULL.
		 * 
		 * @return true if the point's path intersects the collision
		 *         structure.
		 */
		virtual bool pointIntersection ( float x1, float y1,
			                             float x2, float y2,
			                             Vector2D * pContact,
			                             Vector2D * pNormal ) = 0;
			
		/**
		 * Moves the collision structure by the specified amount of pixels.
		 */
		virtual void move ( float dx, float dy ) = 0;
			
		/**
		 * For debug purposes, this will render the current collision
		 * boundaries as a set of lines.
		 */
		virtual void render ( float scrollX, float scrollY, unsigned long color ) = 0;
			
		virtual ~CollisionStruct ();
			
		class Box;
		class Circle;
		class Composite;
		class Polygon;
		
	private:
		static bool collide_Box_Circle     ( Box    *pBox,  Circle  *pCirc );
		static bool collide_Box_Polygon    ( Box    *pBox,  Polygon *pPoly );
		static bool collide_Circle_Polygon ( Circle *pCirc, Polygon *pPoly );
			
		static bool collide_PolyPoints ( const Vector2D * points1, int nPoints1, const Vector2D * points2, int nPoints2 );
};
	
	
	
	
/**
 * Represents rectangular collision boundaries.
 */
class CollisionStruct::Box : public CollisionStruct {
	public:
		/**
		 * Initializes the box's boundaries.
		 */
		Box ( float x, float y, float w, float h );
			
		/**
		 * Initializes the box's boundaries to (0, 0, 0, 0)
		 */
		Box ();
			
			
		inline void setX ( float _x ) {
			x = _x;
		}
		inline void setY ( float _y ) {
			y = _y;
		}
		inline void setWidth ( float _w ) {
			w = _w;
		}
		inline void setHeight ( float _h ) {
			h = _h;
		}
			
		inline float getX () {
			return x;
		}
		inline float getY () {
			return y;
		}
		inline float getWidth () {
			return w;
		}
		inline float getHeight () {
			return h;
		}
			
		void render ( float scrollX, float scrollY, unsigned long color );
		bool collidesWith ( CollisionStruct * otherStruct );
		bool hasPoint ( float x, float y );
		bool pointIntersection ( float x1, float y1,
			                     float x2, float y2,
			                     Vector2D * pContact,
			                     Vector2D * pNormal );
		void move ( float dx, float dy );
			
	private:
		float x, y, w, h;
			
		friend CollisionStruct;
			
};
	
	
	
	
/**
	* Represents circular collision boundaries.
	*/
class CollisionStruct::Circle : public CollisionStruct {
	public:
		/**
			* Initializes the circle's parameters.
			*/
		Circle ( float xCenter, float yCenter, float radius );
			
		/**
			* Initializes the circle to (0, 0) and radius 0.
			*/
		Circle ();
			
		inline void setXCenter ( float x ) {
			xCenter = x;
		}
		inline void setYCenter ( float y ) {
			yCenter = y;
		}
		inline void setRadius ( float r ) {
			radius = r;
		}
			
		inline float getXCenter () {
			return xCenter;
		}
		inline float getYCenter () {
			return yCenter;
		}
		inline float getRadius () {
			return radius;
		}
			
		void render ( float scrollX, float scrollY, unsigned long color );
		bool collidesWith ( CollisionStruct * otherStruct );
		bool hasPoint ( float x, float y );
		bool pointIntersection ( float x1, float y1,
			                     float x2, float y2,
			                     Vector2D * pContact,
			                     Vector2D * pNormal );
		void move ( float dx, float dy );
			
	private:
		float xCenter, yCenter, radius;
			
		friend CollisionStruct;
};
	
	
	
	
/**
	* Represents disjointed collision shapes.
	* 
	* Using a composite collision structure allows the representation of
	* non-contiguous areas, or complex shapes that would otherwise require a
	* polygon.
	* 
	* The <tt>Composite</tt> class is, essentially, a <tt>Container</tt> of
	* <tt>CollisionStruct</tt>s, so you can use any method from the container
	* class to add, remove, and manipulate its elements.
	* 
	* @warning Elements added to the composite will <em>not</em> be <tt>delete</tt>d
	*          automatically. You must <tt>delete</tt> them yourself.
	*/
class CollisionStruct::Composite : public CollisionStruct, public Container<CollisionStruct*> {
	public:
		/**
			* Initializes the composite with the supplied structures.
			*/
		Composite ( CollisionStruct * const elems[], int nCount );
			
		/**
			* Initializes an empty composite.
			*/
		Composite ();
			
		void render ( float scrollX, float scrollY, unsigned long color );
		bool collidesWith ( CollisionStruct * otherStruct );
		bool hasPoint ( float x, float y );
		bool pointIntersection ( float x1, float y1,
			                     float x2, float y2,
			                     Vector2D * pContact,
			                     Vector2D * pNormal );
		void move ( float dx, float dy );
			
	private:
		friend CollisionStruct;
};
	
	
	
	
/**
	* Represents a polygonal collision boundary.
	* 
	* The polygon may be concave and its edges may intersect. You could hardly
	* ask for anything more generic than this, although it is much less
	* efficient than other collision structures.
	* 
	* The polygon defined by <tt>points</tt> is assumed to be the
	* "untranslated" version of the collision boundary. If this were an
	* <tt>hgeSprite</tt>, think of the point (0, 0) as the polygon's hot spot.
	* 
	* Typically, you would specify the object's polygon points relative to its
	* sprite's hot spot, and then update the polygon's position to the
	* object's position once per frame.
	*/
class CollisionStruct::Polygon : public CollisionStruct {
	public:
		/// @{
		/**
			* The constructor receives a series of points that represent the
			* polygon's outline. The polygon is always assumed to be closed
			* (i.e., the last point will be "linked" with the first point).
			*/
		Polygon ( const Vector2D * points, int nPoints );
		Polygon ( const Container<Vector2D> & points );
		/// @}
			
		/**
			* Default constructor, that creates an "empty" polygon.
			*/
		Polygon ();
			
			
		/// @{
		/**
			* Redefines the points that represent the polygon's outline.
			*/
		void setPoints( const Vector2D * points, int nPoints );
			
		inline void setPoints( const Container<Vector2D> & points ) {
			setPoints( points.getData(), points.getCount() );
		}
		/// @}
			
		/**
			* Returns the polygon points with scaling/translation applied.
			* @see setPoints(), setScale(), setPosition()
			*/
		inline void getTransformedPoints ( Container<Vector2D> * points ) {
			*points = realPoints;
		}
			
		/**
			* Sets the object's displacement relative to (0, 0).
			*/
		void setPosition ( float x, float y );
			
		/**
			* Gets the object displacement relative to (0, 0).
			*/
		void getPosition ( float * x, float * py );
			
		/**
			* Sets the object's rotation around its hot spot.
			*/
		void setRotation ( float radians );
			
		/**
			* Gets the object's rotation around its hot spot.
			*/
		float getRotation ();
			
		/**
			* Sets the object's scale.
			*/
		void setScale ( float s );
			
		/**
			* Gets the object's current scale.
			*/
		void getScale ();
			 
			
			
		void render ( float scrollX, float scrollY, unsigned long color );
		bool collidesWith ( CollisionStruct * otherStruct );
		bool hasPoint ( float x, float y );
		bool pointIntersection ( float x1, float y1,
			                     float x2, float y2,
			                     Vector2D * pContact,
			                     Vector2D * pNormal );
		void move ( float dx, float dy );
			
	private:
		Container<Vector2D> userPoints, realPoints;
		Vector2D pos;
		float rotation, scale;
			
		void updateRealPoints ();
			
		friend CollisionStruct;
};

#endif