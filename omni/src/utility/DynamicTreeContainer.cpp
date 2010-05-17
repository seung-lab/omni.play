#include "utility/DynamicTreeContainer.h"

template <typename T>
DynamicTreeContainer<T>::DynamicTreeContainer( const unsigned int size )
	: mSize(size), mTree( new DynamicTree<T> * [ size ] )
{
	for( unsigned int i = 0; i < mSize; ++i ){
		mTree[ i ] = DynamicTree<T>::makeTree( i );
	}
}
