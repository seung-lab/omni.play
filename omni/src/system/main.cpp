
#include <fstream>

#include "omPreferences.h"

#include "system/omThreadedCache.h"
#include "system/omCacheableBase.h"

#include "common/omSerialization.h"
#include "common/omStd.h"
#include "common/omDebug.h"

/*
class A : public OmCacheableBase {

public:	
	A(OmCacheBase *cache) : OmCacheableBase(cache) {
		cout << "A::A()" << endl;
		UpdateSize( sizeof(A) );
	}
	
	~A() {
		UpdateSize( -sizeof(A) );
	}
	
	
	int a;
};

class ACache : public OmThreadedCache< int, A > {
	
public:
	ACache() : OmThreadedCache<int,A>(VRAM_CACHE_GROUP, 0.5, false, true) { 
	
	}
	
	bool InitializeFetchThread() { return true; }
	
	A* HandleCacheMiss(const int &key) {
		cout << "miss" << endl;
		return new A(this);
	}
};
*/

int main(int argc, char *argv[])
{
	/*
	   switch(argc) {

	   case 3:
	   cout << argv[1] << argv[2] << endl;
	   break;

	   default:
	   cout << "Usage:" << endl;
	   cout << argv[0] << " [directory] [pattern]" << endl;
	   return 0;
	   }
	 */

	/*
	   ACache a_cache;

	   for(int i=0; i<10000; ++i) {
	   //sleep(0.1);
	   a_cache.Get(i);
	   }

	   while(true) { }
	 */
//      sleep(1);

	/*
	   shared_ptr<A> a = a_cache.Get(1);
	   shared_ptr<A> b = a_cache.Get(2, true);
	   shared_ptr<A> c = a_cache.Get(3);

	   sleep(1);
	   shared_ptr<A> d = a_cache.Get(4);
	   sleep(1);
	   a_cache.Get(1);
	   a_cache.Get(2);

	   sleep(5);
	 */

	//delete &a_cache;

}
