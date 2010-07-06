#include "segment/lowLevel/DynamicTreeContainer.h"

template <typename T>
DynamicTreeContainer<T>::DynamicTreeContainer( const quint32 size )
	: mSize(size)
{
	mNodeArray.resize( size, NULL );
}

template <typename T>
void DynamicTreeContainer<T>::growIfNeeded(const quint32 size)
{
	assert(size >= mSize && "growing only");

	if(size > mSize){
		mNodeArray.resize( size, NULL );
	}
}
