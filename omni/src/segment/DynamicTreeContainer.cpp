#include "segment/DynamicTreeContainer.h"

template <typename T>
DynamicTreeContainer<T>::DynamicTreeContainer( const unsigned int size )
	: mSize(size), mNodeArray( new DynamicTree<T> * [ size ] )
{
	memset( mNodeArray, 0, sizeof(DynamicTree<T>*) * mSize);
}
