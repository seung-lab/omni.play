#ifndef BI_SET_MAP_H
#define BI_SET_MAP_H

/*
 *	Map A to a set of B.  Each B in the set is mapped to A.
 *	Brett Warne - bwarne@mit.edu - 4/21/09
 */

#include <map>
#include <set>

using std::set;

template < typename T, typename U > class biSetMap {

 public:

	set < U > &operator[](const T & t) {
		return mT2USetMap[t];
	}

	T & operator[](const U & u) {
		return mU2TMap[u];
	}

	size_t size() {
		return mT2USetMap.size();
	}

	size_t count(const T & t) {
		return mT2USetMap.count(t);
	}

	size_t count(const U & u) {
		return mU2TMap.count(u);
	}

	void insert(const T & t, const U & u) {
		mT2USetMap[t].insert(u);
		mU2TMap[u] = t;
	}

	void insert(const T & t, const set < U > &u_set) {
		mT2USetMap[t].insert(u_set.begin(), u_set.end());

		typename set < U >::iterator itr;
		for (itr = u_set.begin(); itr != u_set.end(); itr++) {
			mU2TMap[*itr] = t;
		}
	}

	void erase(const T & t) {
		//if no t mapped, return
		if (!mT2USetMap.count(t))
			return;

		//get set mapped to t
		set < U > &u_set = mT2USetMap[t];

		//unmap each u
		typename set < U >::iterator itr;
		for (itr = u_set.begin(); itr != u_set.end(); itr++) {
			mU2TMap.erase(*itr);
		}

		//unmap t to set of u
		mT2USetMap.erase(t);
	}

	void erase(const U & u) {
		//if no u mapped, then return
		if (!mU2TMap.count(u))
			return;

		//get t
		T & r_t = mU2TMap[u];

		//remove u from set with key t
		(mT2USetMap[r_t]).erase(u);

		//unmap u to t
		mU2TMap.erase(u);

	}

 private:
	std::map < T, set < U > >mT2USetMap;
	std::map < U, T > mU2TMap;

};

#endif
