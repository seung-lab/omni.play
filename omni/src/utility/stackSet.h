#ifndef STACK_SET_H
#define STACK_SET_H

/*
 *	Stack-Set hybrid container to maintain an stack of unique elements.  Stack allows for ordering
 *	while the set allows for quick evaluation whether a value is already contained.
 *
 *	push/pop: 1 + log(n)
 *	clear: 2n
 *	count: log(n)
 *
 *	Brett Warne - bwarne@mit.edu - 4/12/09
 */


#include <set>
#include <deque>


template < typename T >
class StackSet {

public:

	void push(const T &value) {
		//std::cout << value << std::endl;
		//std::cout << "count" << std::endl;
		if(mSet.count(value)) return;
	
		//std::cout << "insert" << std::endl;
		mSet.insert(value);
		//std::cout << "push_back" << std::endl;
		mDeque.push_back(value);
		//std::cout << "done" << std::endl;
	}
		
	void pop() {
		if(!mDeque.size()) return;
		
		mSet.erase(mDeque.back());
		mDeque.pop_back();
	}
	
	T& top() {
		return mDeque.back();
	}
	
	void clear() {
		mSet.clear();
		mDeque.clear();
	}
	
	unsigned int count(const T &value) const {
		return mSet.count(value);
	}
	
	unsigned int size() const {
		return mDeque.size();
	}
	
	bool empty() const {
		return mDeque.empty();
	}

private:
	std::set<T> mSet;
	std::deque<T> mDeque;
};



#endif
