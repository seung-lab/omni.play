#include "segment/DynamicTreeContainer.h"

template <typename T>
DynamicTreeContainer<T>::DynamicTreeContainer( const quint32 size )
	: mSize(size)
{
	mNodeArray.resize( size, NULL );
}
