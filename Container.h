#ifndef _Container_h_
#define _Container_h_

#include <math.h>
#include <stdexcept>
#include <stdlib.h>

/**
* A generic container to store data in a linear arrangement, similar to
* Java's <tt>Vector</tt>.
* 
* Elements are stored in an internal array; its size is increased
* automatically to accomodate new elements.
* 
* @warning
* The <tt>Container</tt> class may only work properly if the template's
* type argument (i.e., the stuff between \<angle brackets\>) meets the
* following criteria:
*  - It must implement a <b>default contructor</b>.
*  - It should implement a <b>copy constructor</b>.
*  - It should implement the \b assignment \b operator.
*  - It should implement the \b comparison \b operator (<tt>operator==</tt>).
* 
* Whether or not these items are necessary will depend on what kind of
* resources the class stores internally. Failing to provide the methods
* above may result in compiler errors, access violations or memory leaks.
* 
* You can safely use primitive types (<tt>char</tt>, <tt>float</tt>, any
* kind of pointer, etc.) with this class.
*/
template <typename T>
class Container 
{
public:
	/**
	* Creates an empty container.
	*/
	Container () {
		// No elements yet:
		count = 0;
			
		// Start with a capacity of 1:
		capacity = 1;
		data = new T[1];
	}
		
	/**
	* Copies all elements from the array into the container.
	*/
	Container ( const T * elems, int _count ) {
		count = _count;
		capacity = _count;
		data = new T[count];
			
		for ( int i = 0; i < count; i++ )
			data[i] = elems[i];
	}
		
	/**
	* Copy constructor: All the elements will be copied from the other
	* container.
	*/
	Container ( const Container & orig ) {
		count    = orig.count;
		capacity = orig.capacity;
		data     = new T[capacity];
			
		for ( int i = 0; i < count; i++ )
			data[i] = orig.data[i];
	}
		
	/**
	* Assignment operator: Copies all elements from the other container.
	*/
	inline Container & operator = ( const Container & other ) {
		reserve( other.count );
			
		for ( int i = 0; i < other.count; i++ )
			data[i] = other.data[i];
			
		count = other.count;
			
		return (*this);
	}
		
		
	/**
	* Appends an element to the end of the container. The internal array's
	* size will be increased automatically.
	*/
	inline void add ( const T & elem ) {
		// Ensure we've got room for the new element:
		reserve( count + 1 );
			
		// Store the new element:
		data[count] = elem;
			
		count++;
	}
		
	/**
	* Inserts an element at position <tt>index</tt> in the container,
	* "pushing" all elements after it towards the end.
	* 
	* A valid index is defined as
	* <center>  0 <= index <= count  </center>
	* 
	* @throw std::out_of_range if the index is not valid.
	*/
	inline void add ( int index, const T & elem ) {
		// Check for index validity:
		if ( index < 0 || index > count )
			throw std::out_of_range("Container index out of range");
			
		// Ensure we've got room for the new element:
		reserve( count + 1 );
			
		// Shift everyone from "index" onwards to higher indices:
		for ( int i = count; i > index; i-- )
			data[i] = data[i - 1];
			
		// Store the new element:
		data[index] = elem;
		count++;
	}
		
	/**
	* Searches the container for the specified element.
	* 
	* @return The index of the first occurrence found, or -1 if the
	*         element was not found.
	*/
	inline int indexOf ( const T & elem ) const {
		for ( int i = 0; i < count; i++ )
			if ( data[i] == elem )
				return i;
			
		return -1;
	}
		
	/**
	* Removes the element at the specified index, shifting all the elements
	* after it towards the beginning.
	* 
	* A valid index is defined as
	* <center>  0 <= index < count  </center>
	*
	* @throw std::out_of_range if an invalid index is supplied.
	*/
	inline void remove ( int index ) {
		// Check for index validity:
		if ( index < 0 || index >= count )
			throw std::out_of_range("Container index out of range");
			
		// Shift everyone from "index + 1" onwards to lower indices:
		for ( int i = index; i < count - 1; i++ )
			data[i] = data[i + 1];
			
		// One less element:
		count--;
	}
		
	/**
	* Searches the container for the first occurrence of the element, and
	* removes it if it was found.
	* 
	* @return true if the element was found and removed.
	*/
	inline bool removeFirst ( const T & elem ) {
		int index = indexOf( elem );
			
		if ( index == -1 )
			return false;
		else {
			remove( index );
			return true;
		}
	}
		
	/**
	* Searches the container for all occurrences of the element, removing
	* them as they are found.
	* 
	* @return true if at least one element was found and removed.
	*/
	inline bool removeAll ( const T & elem ) {
		bool foundIt = false;
			
		for ( int i = count - 1; i >= 0; i-- ) {
			if ( data[i] == elem ) {
				remove( i );
				foundIt = true;
			}
		}
			
		return foundIt;
	}
		
	/**
	* Removes all elements from the container.
	*/
	inline void removeAll () {
		count = 0;
	}
		
	/**
	* @return The number of elements in this container.
	*/
	inline int getCount () const {
		return count;
	}
		
	/**
	* Used to access the elements in the container.
	* 
	* A valid index is defined as
	* <center>  0 <= index < count  </center>
	* 
	* @return A reference to the element at the specified index. It is
	*         possible to either set or get the elements with this operator.
	* @throw std::out_of_range if an invalid index is supplied.
	*/
	inline const T & operator[] ( int index ) const {
		// Check for index validity:
		if ( index < 0 || index >= count )
			throw std::out_of_range("Container index out of range");
			
		return data[index];
	}
		
	/**
	* Used to access the elements in the container.
	* 
	* A valid index is defined as
	* <center>  0 <= index < count  </center>
	* 
	* @return A reference to the element at the specified index. It is
	*         possible to either set or get the elements with this operator.
	* @throw std::out_of_range if an invalid index is supplied.
	*/
	inline T & operator[] ( int index ) {
		// Check for index validity:
		if ( index < 0 || index >= count )
			throw std::out_of_range("Container index out of range");
			
		return data[index];
	}
		
	/**
	* An alias for the subscript operator.
	* @see operator[]()
	*/
	inline const T & elem ( int index ) const {
		return operator[]( index );
	}
		
	/**
	* An alias for the subscript operator.
	* @see operator[]()
	*/
	inline T & elem ( int index ) {
		return operator[]( index );
	}
		
	/**
	* @return A reference to the last element in this container.
	* @throw std::out_of_range if the container is empty.
	*/
	inline const T & tail () const {
		return operator[]( count - 1 );
	}
		
		
	/**
	* @return A reference to the last element in this container.
	* @throw std::out_of_range if the container is empty.
	*/
	inline T & tail () {
		return operator[]( count - 1 );
	}
		
		
	/**
	* Returns a pointer to the container's internal buffer. This pointer
	* must not be deleted or modified in any way -- use other methods for
	* those purposes. This is useful for functions requiring an array as a
	* parameter.
	*/
	inline const T * getData () const {
		return data;
	}
		
	/**
	* Moves an element to the beginning of the container, shifting the
	* necessary elements to higher indices.
	* 
	* A valid index is defined as
	* <center>  0 <= index < count  </center>
	* 
	* @throw std::out_of_range if an invalid index is supplied.
	*/
	inline void toBeginning ( int index ) {
		// Check for index validity:
		if ( index < 0 || index >= count )
			throw std::out_of_range("Container index out of range");
			
		// Ignore if the object is already at the head of the Container:
		if ( index == 0 )
			return;
			
		// The element being moved:
		T elem = data[index];
			
		// Shift everyone towards the end:
		for ( int i = index; i >= 1; i-- )
			data[i] = data[i - 1];
			
		// Place the element at index 0:
		data[0] = elem;
	}
		
	/**
	* Moves an element to the end of the container, shifting the
	* necessary elements to lower indices.
	* 
	* A valid index is defined as
	* <center>  0 <= index < count  </center>
	* 
	* @throw std::out_of_range if an invalid index is supplied.
	*/
	inline void toEnd ( int index ) {
		// Check for index validity:
		if ( index < 0 || index >= count )
			throw std::out_of_range("Container index out of range");
			
		// Ignore if the object is already at the tail of the Container:
		if ( index == count - 1 )
			return;
			
		// The element being moved:
		T elem = data[index];
			
		// Shift everyone towards the beginning:
		for ( int i = index + 1; i < count; i++ )
			data[i - 1] = data[i];
			
		// Place the element at index "count - 1":
		data[count - 1] = elem;
	}
		
	/**
	* Swaps the position of two objects in the container.
	* @throw std::out_of_range if an invalid index is supplied.
	*/
	inline void swap ( int i, int j ) {
		// Check for index validity:
		if ( i < 0 || i >= count ||
			    j < 0 || j >= count )
			throw std::out_of_range("Container index out of range");
			
		// Nothing to do if the indices are the same:
		if ( i == j )
			return;
			
		T tmp = data[j];
		data[j] = data[i];
		data[i] = tmp;
	}
		
		
	/**
	* Destructor: Frees the allocated memory.
	*/
	virtual ~Container () {
		delete[] data;
	}
		
private:
	T * data;
	int capacity;
	int count;
		
	// Ensures the container's capacity will be "size" or larger.
	void reserve ( int size ) {
		if ( capacity < size ) {
			// Increase capacity in steps of 125%, until it's large enough.
			do {
				capacity = (int) ceil( capacity * 1.25 );
			} while ( capacity < size );
				
			// Allocate new storage space:
			T * newData = new T[capacity];
				
			// Copy data to the new storage space:
			for ( int i = 0; i < count; i++ )
				newData[i] = data[i];
				
			// Replace old data:
			delete[] data;
			data = newData;
		}
	}
};



#endif
