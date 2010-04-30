#ifndef OM_FETCHING_THREAD_H
#define OM_FETCHING_THREAD_H

#include <QThread>
#include <QHash>
#include <QQueue>
#include <QStack>
#include <QExplicitlySharedDataPointer>

template < typename KEY, typename PTR  >
class OmFetchingThread : public QThread 
{
 public:
	OmFetchingThread();
	~OmFetchingThread();

 protected:
	void run();

 private:
	
	
	QHash< KEY, QExplicitlySharedDataPointer< PTR > > mCache;
	QQueue< KEY > mKeysByRequestOrder;
	QStack< KEY > mFetchStack;
};

#endif
