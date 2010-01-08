
/*
 * Omni Global Utility Header File
 *
 * Brett Warne - bwarne@mit.edu - 2/6/09
 */

#ifndef OM_UTILITY_H
#define OM_UTILITY_H


#include "omStd.h"


template< typename T >
void
setIntersection(const set<T> &source, const set<T> &target, set<T> &intersection) {
	//create intersection insert iterator
	std::insert_iterator< set<T> > insert_itr(intersection, intersection.begin());
	
	//perform intersection
	std::set_intersection(source.begin(), source.end(), 
						  target.begin(), target.end(),
						  insert_itr);	
}




/*
 *	Directional set differance - what is in source but not target.
 *	1,2,3 diff 3,4,5 -> 1,2
 */
template< typename T >
void
setDifference(const set<T> &source, const set<T> &target, set<T> &differance) {
	//create intersection insert iterator
	std::insert_iterator< set<T> > insert_itr(differance, differance.begin());
	
	//perform difference
	std::set_difference(source.begin(), source.end(), 
						target.begin(), target.end(),
						insert_itr);	
}




/*
 *	Union.
 */
template< typename T >
void
setUnion(const set<T> &source1, const set<T> &source2, set<T> &result) {
	//create intersection insert iterator
	std::insert_iterator< set<T> > insert_itr(result, result.begin());
	
	//perform difference
	std::set_union(source1.begin(), source1.end(), 
				   source2.begin(), source2.end(),
				   insert_itr);	
}




template< typename T >
void
setPrint(const set<T> &source) {
	//specify to treat ::iterator as a dependent qualified type (rather than a static member)
	//http://pages.cs.wisc.edu/~driscoll/typename.html
	typename set< T >::iterator it;

	cout << "[ ";
	
	for( it=source.begin(); it != source.end(); it++ )
		cout << (int) *it << " ";
	
	cout << "]" << endl;
}





#endif
