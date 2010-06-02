#ifndef DYNAMIC_TREE_CONTAINER
#define DYNAMIC_TREE_CONTAINER

#include "segment/DynamicTree.h"

template < typename T >
class DynamicTreeContainer {
 public:
	DynamicTreeContainer( const unsigned int size );
	~DynamicTreeContainer(){
		for( unsigned int i = 0; i < mSize; ++i ){
			delete mNodeArray[ i ];
		}
	}
 
	DynamicTree<T> * get( const unsigned int index ){
		assert( index );
		assert( index < mSize );
		if( NULL == mNodeArray[ index ] ){
			mNodeArray[ index ] = DynamicTree<T>::makeTree( index );
		}
		return mNodeArray[ index ];
	}

	unsigned int getSize(){ return mSize; }
	const std::vector<DynamicTree<T>*> & getTreeNodeArray(){ return mNodeArray; }

 private:
	const unsigned int mSize;
	std::vector<DynamicTree<T>*> mNodeArray;
};

#endif
