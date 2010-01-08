#ifndef __VMML__AXIS_ALIGNED_BOUNDING_BOX__H__
#define __VMML__AXIS_ALIGNED_BOUNDING_BOX__H__

#include <vmmlib/vector3.h>
#include <vmmlib/vector4.h>



namespace vmml
{

template< typename T >
class AxisAlignedBoundingBox
{
public:
    AxisAlignedBoundingBox();
    AxisAlignedBoundingBox( const Vector3< T >& pMin, const Vector3< T >& pMax );
    AxisAlignedBoundingBox( const Vector4< T >& sphere );
    AxisAlignedBoundingBox( T cx, T cy, T cz, T size );
	/* bwarne: for easer construction */
	AxisAlignedBoundingBox( const Vector3< T >& pMin, T dimX, T dimY, T dimZ );
	AxisAlignedBoundingBox( const Vector3< T >& pMin, T dim);
    
    inline bool contains( const Vector3< T >& pos ) const;
    inline bool containsIn2d( const Vector3< T >& pos ); // only x and y components are checked
    inline bool contains( const Vector4< T >& sphere );
	inline bool contains( const AxisAlignedBoundingBox< T >& aabb );
	
    inline void set( const Vector3< T >& pMin, const Vector3< T >& pMax );
    inline void set( T cx, T cy, T cz, T size );
	inline void setMin( const Vector3< T >& pMin );
	inline void setMax( const Vector3< T >& pMax );
    inline const Vector3< T >& getMin() const;
    inline const Vector3< T >& getMax() const;
    
	inline Vector3< T > getDimensions() const;
    inline Vector3< T > getUnitDimensions() const;
	
    inline void merge( const AxisAlignedBoundingBox< T >& aabb );
	inline void intersect( const AxisAlignedBoundingBox< T >& aabb );
	inline void offset( const Vector3< T >& offset ); 
	
    inline void setEmpty( bool empty = true );
    inline bool isEmpty() const;
    inline void setDirty( bool dirty = true );
    inline bool isDirty() const;
    
    Vector3< T > getCenter() const;
	/*bwarne: for frustom culling */
	Vector3< T > getPosVertex(const Vector4< T > &plane) const;
	Vector3< T > getNegVertex(const Vector4< T > &plane) const;
	Vector3< T > getNearestVertex(const Vector3< T > &v) const;
	
	
	friend std::ostream& operator << ( std::ostream& os, const AxisAlignedBoundingBox& aabb )
    {
        os << "min:" << aabb._min << " max:" << aabb._max;
        return os;
    }; 
	
	/* bwarne */
	static const AxisAlignedBoundingBox UNITBOX;
	
protected:
	Vector3< T > _min;
	Vector3< T > _max;
    bool _dirty;
    bool _empty;
	
};


/* bwarne */
template< typename T > 
const AxisAlignedBoundingBox< T > AxisAlignedBoundingBox< T >::UNITBOX(Vector3< T >::ZERO, Vector3< T >::ONE );
	
	

template< typename T >
AxisAlignedBoundingBox< T >::AxisAlignedBoundingBox()
    : _dirty( false )
    , _empty( true )
{}



template< typename T >
AxisAlignedBoundingBox< T >::AxisAlignedBoundingBox( const Vector3< T >& pMin, const Vector3< T >& pMax )
    : _min( pMin )
    , _max( pMax )
    , _dirty( false )
    , _empty( false )
{}

/* bwarne: add constructor to take position and dimension
	 note: dimensions must be positive
 */
template< typename T >
AxisAlignedBoundingBox< T >::AxisAlignedBoundingBox( const Vector3< T >& pMin, T dimX, T dimY, T dimZ)
	: _min( pMin)
	, _max( pMin + Vector3<T>(dimX - 1, dimY - 1, dimZ - 1) )
	, _dirty(false)
	, _empty(false)
{}
	
/* bwarne: add constructor to take position and dimension
	 note: dimensions must be positive
 */
template< typename T >
AxisAlignedBoundingBox< T >::AxisAlignedBoundingBox( const Vector3< T >& pMin, T dim)
	: _min( pMin)
	, _max( pMin + Vector3<T>(dim - 1, dim - 1, dim - 1) )
	, _dirty(false)
	, _empty(false)
{}
	

template< typename T >
AxisAlignedBoundingBox< T >::AxisAlignedBoundingBox( const Vector4< T >& sphere )
    : _dirty( false )
    , _empty( false )
{
    _max = _min = sphere.getCenter();
    _max += sphere.getRadius();
    _min -= sphere.getRadius();
}



template< typename T >
AxisAlignedBoundingBox< T >::AxisAlignedBoundingBox( T cx, T cy, T cz, T size )
    : _dirty( false )
    , _empty( false )
{
    _max = _min = Vector3f( cx, cy, cz );
    _max += size;
    _min -= size;
}



template< typename T >
inline bool AxisAlignedBoundingBox< T >::contains( const Vector4< T >& sphere )
{
    if ( _empty ) 
        return false;
    Vector3< T > sv ( sphere.getCenter() );
    sv += sphere.getRadius();
    if ( sv.x > _max.x || sv.y > _max.y || sv.z > _max.z )
        return false;
    sv -= sphere.getRadius() * 2.0f;
    if ( sv.x < _min.x || sv.y < _min.y || sv.z < _min.z )
        return false;
    return true;
}


/* bwarne: function is now const */
template< typename T >
inline bool AxisAlignedBoundingBox< T >::contains( const Vector3< T >& pos ) const
{
    if ( _empty ) 
        return false;
    if ( pos.x > _max.x || pos.y > _max.y || pos.z > _max.z 
            || pos.x < _min.x || pos.y < _min.y || pos.z < _min.z )
    {
        return false;
    }
    return true;
}


/* bwarne: added to compare with other bboxes */
template< typename T >
inline bool AxisAlignedBoundingBox< T >::contains( const AxisAlignedBoundingBox< T >& aabb )
{
	if ( _empty || aabb.isEmpty() ) 
        return false;
    if ( contains(aabb.getMin()) && contains(aabb.getMax()) )
    {
        return true;
    }
    return false;
}


template< typename T >
inline bool AxisAlignedBoundingBox< T >::containsIn2d( const Vector3< T >& pos )
{
    if ( _empty ) 
        return false;
    if ( pos.x > _max.x || pos.y > _max.y || pos.x < _min.x || pos.y < _min.y )
    {
        return false;
    }
    return true;
}




template< typename T >
inline void AxisAlignedBoundingBox< T >::set( const Vector3< T >& pMin, 
    const Vector3< T >& pMax )
{
    _min = pMin;
    _max = pMax;
    _empty = false;
}



template< typename T >
inline void AxisAlignedBoundingBox< T >::set( T cx, T cy, T cz, T size )
{
    Vector3< T > center( cx, cy, cz );
    _min = center - size;
    _max = center + size;
    _empty = false;
}



template< typename T >
inline void AxisAlignedBoundingBox< T >::setMin( const Vector3< T >& pMin ) 
{ 
    _min = pMin; 
}



template< typename T >
inline void AxisAlignedBoundingBox< T >::setMax( const Vector3< T >& pMax ) 
{ 
    _max = pMax; 
}



template< typename T >
inline const Vector3< T >& AxisAlignedBoundingBox< T >::getMin() const 
{   
    return _min; 
}



template< typename T >
inline const Vector3< T >& AxisAlignedBoundingBox< T >::getMax() const
{ 
    return _max; 
}



template< typename T >
Vector3< T > 
AxisAlignedBoundingBox< T >::getCenter() const
{
    return _min + ( ( _max - _min ) * 0.5f );
}

	
	
	
/*bwarne: for frustom culling */
template< typename T >	
Vector3< T > 
AxisAlignedBoundingBox< T >::getPosVertex(const Vector4< T > &plane) const { 
	Vector3< T > pos = _min;
	
	if(plane.normal.x >= 0)
		pos.x = _max.x;
	if(plane.normal.y >= 0)
		pos.y = _max.y;
	if(plane.normal.z >= 0)
		pos.z = _max.z;
	
	return pos;
}
	
template< typename T >	
Vector3< T > 
AxisAlignedBoundingBox< T >::getNegVertex(const Vector4< T > &plane) const { 
	Vector3< T > neg = _max;
	
	if(plane.normal.x >= 0)
		neg.x = _min.x;
	if(plane.normal.y >= 0)
		neg.y = _min.y;
	if(plane.normal.z >= 0)
		neg.z = _min.z;
	
	return neg;
}
	
	
template< typename T >	
Vector3< T > 
AxisAlignedBoundingBox< T >::getNearestVertex(const Vector3< T > &v) const { 
	Vector3< T > dist_min = (v - _min) * (v - _min);
	Vector3< T > dist_max = (v - _max) * (v - _max);
	
	Vector3< T > near = _min;
	
	if( dist_min.x > dist_max.x )
		near.x = _max.x;
	if( dist_min.y > dist_max.y )
		near.y = _max.y;
	if( dist_min.z > dist_max.z )
		near.z = _max.z;
	
	return near;
}
	
	
template< typename T >	
Vector3< T > 
AxisAlignedBoundingBox< T >::getDimensions() const { 
	return getMax() - getMin();
}

template< typename T >	
Vector3< T > 
AxisAlignedBoundingBox< T >::getUnitDimensions() const { 
	return getMax() - getMin() + Vector3< T >::ONE;
}




template< typename T >
void 
AxisAlignedBoundingBox< T >::merge( const AxisAlignedBoundingBox< T >& aabb )
{
    if ( aabb._empty )
        return; // nothing to do

    if ( _empty )
    {
        // copy non-empty aabb
        _min = aabb._min;
        _max = aabb._max;
        _empty = _dirty = false;
        return;
    }
    
    // else merge the two aabbs
    const Vector3< T >& min = aabb.getMin();
    const Vector3< T >& max = aabb.getMax();
    
    if ( min.x < _min.x )
        _min.x = min.x;
    if ( min.y < _min.y )
        _min.y = min.y;
    if ( min.z < _min.z )
        _min.z = min.z;

    if ( max.x > _max.x )
        _max.x = max.x;
    if ( max.y > _max.y )
        _max.y = max.y;
    if ( max.z > _max.z )
        _max.z = max.z;
}


	
/* bwarne: modifies this box to be interesection of self and given box */
template< typename T >
void
AxisAlignedBoundingBox< T >::intersect( const AxisAlignedBoundingBox< T >& aabb )
{
	if ( _empty )
		return; // nothing to do

    if ( aabb._empty ) {
		//then self is now empty
		_empty = true;
		_dirty = false;
		return;
	}
    
    // else intersect the two aabbs
    const Vector3< T >& min = aabb.getMin();
    const Vector3< T >& max = aabb.getMax();
    
    if ( min.x > _min.x )
        _min.x = min.x;
    if ( min.y > _min.y )
        _min.y = min.y;
    if ( min.z > _min.z )
        _min.z = min.z;
	
    if ( max.x < _max.x )
        _max.x = max.x;
    if ( max.y < _max.y )
        _max.y = max.y;
    if ( max.z < _max.z )
        _max.z = max.z;
	
	//check for invalid dimensions
	Vector3< T > diff = _max - _min;
	if ( diff.x < 0 || diff.y < 0 || diff.z < 0 ) {
		_empty = true;
		_dirty = false;	
	}
}
	
	
/* bwarne */
template < typename T > 
void 
AxisAlignedBoundingBox< T >::offset( const Vector3< T > &offset ) 
{ 
	_min += offset;
	_max += offset;
}

	

template< typename T >
inline void  
AxisAlignedBoundingBox< T >::setEmpty( bool empty )
{
    _empty = empty;
}



template< typename T >
inline bool 
AxisAlignedBoundingBox< T >::isEmpty() const
{
    return _empty;
}



template< typename T >
inline void  
AxisAlignedBoundingBox< T >::setDirty( bool dirty )
{
    _dirty = dirty;
}



template< typename T >
inline bool 
AxisAlignedBoundingBox< T >::isDirty() const
{
    return _dirty;
}



typedef AxisAlignedBoundingBox< float > Aabbf;

}; //namespace vmml

#endif

