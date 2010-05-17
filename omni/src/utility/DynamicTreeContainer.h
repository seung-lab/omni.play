#ifndef DYNAMIC_TREE_CONTAINER
#define DYNAMIC_TREE_CONTAINER

#include "utility/DynamicTree.h"

template < typename T >
class DynamicTreeContainer {
 public:
	DynamicTreeContainer( const unsigned int size );
	~DynamicTreeContainer(){
		for( unsigned int i = 0; i < mSize; ++i ){
			delete mTree[ i ];
		}
		
		delete [] mTree;
		mTree = NULL;
	}
 
	DynamicTree<T> * get( const unsigned int index ){
		assert( index < mSize );
		return mTree[ index ];
	}

 private:
	const unsigned int mSize;
	DynamicTree<T> ** mTree;
};

#endif
