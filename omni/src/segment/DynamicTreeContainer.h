#ifndef DYNAMIC_TREE_CONTAINER
#define DYNAMIC_TREE_CONTAINER

#include "segment/DynamicTree.h"

template < typename T >
class DynamicTreeContainer {
 public:
	DynamicTreeContainer( const quint32 size );
	~DynamicTreeContainer(){
		for( quint32 i = 0; i < mSize; ++i ){
			delete mNodeArray[ i ];
		}
	}
 
	DynamicTree<T> * get( const quint32 index ){
		assert( index );
		assert( index < mSize );
		if( NULL == mNodeArray[ index ] ){
			mNodeArray[ index ] = DynamicTree<T>::makeTree( index );
		}
		return mNodeArray[ index ];
	}

	quint32 getSize(){ return mSize; }
	const std::vector<DynamicTree<T>*> & getTreeNodeArray(){ return mNodeArray; }

 private:
	const quint32 mSize;
	std::vector<DynamicTree<T>*> mNodeArray;
};

#endif
