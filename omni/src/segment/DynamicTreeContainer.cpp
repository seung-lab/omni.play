#include "segment/DynamicTreeContainer.h"

template <typename T>
DynamicTreeContainer<T>::DynamicTreeContainer( const unsigned int size )
	: mSize(size)
{
	mNodeArray.resize( size, NULL );
}
