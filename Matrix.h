#ifndef _Matrix_h_
#define _Matrix_h_

#include <stdexcept>


/**
* General-purpose storage class to arrange data as rows and columns.
*/
template <typename T>
class Matrix 
{
public:
	/**
	* Matrix dimensions, initialized in the constructor.
	*/
	const int rows, columns;
		
		
	/**
	* The constructor will allocate a block of memory to store the data.
	* The Matrix's dimensions cannot be changed after creation.
	*/
	Matrix ( int _rows, int _columns ) : rows(_rows), columns(_columns) {
		data = new T[rows * columns * sizeof(T)];
		memset(data,-1,rows * columns * sizeof(T));
	}
		
	/**
	* Copy constructor: Copies data from the other matrix.
	*/
	Matrix ( Matrix<T> & orig ) : rows(orig.rows), columns(orig.columns) {
		data = new T[rows * columns * sizeof(T)];
			
		for ( int i = 0; i < rows; i++ )
			for ( int j = 0; j < columns; j++ )
				data[i * columns + j] = orig.data[i * columns + j];
	}
		
	/**
	* Cell access method.
	* @return A reference to the cell's element; this allows clients to
	*         modify the matrix's contents.
	* @throw std::out_of_range if either the row or column indices exceed
	*        the matrix's dimensions.
	*/
	inline T & cell ( int row, int column ) const {
		// Test index validity:
		if ( row < 0 || row >= rows || column < 0 || column >= columns )
			throw std::out_of_range("Matrix index out of range");
			
		return data[row * columns + column];
	}
		
		
	/**
	* Destructor: Deallocates the memory block used to store the 
	* matrix's data.
	*/
	virtual ~Matrix () {
		delete[] data;
	}

private:
	T * data;
};

#endif
