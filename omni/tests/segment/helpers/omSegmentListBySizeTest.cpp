
void OmSegmentCacheImpl::test()
{
	OmSegmentListBySize testList;
	OmSegment * seg1 = new OmSegment(100, mParentCache );
	seg1->mSize = 500;

	OmSegment * seg2 = new OmSegment(200, mParentCache );
	seg2->mSize = 1000;

	testList.insertSegment(seg1);
	testList.insertSegment(seg2);
	testList.updateFromJoin(seg1, seg2);
	const quint64 newsize = testList.getSegmentSize( seg1->mValue );
	assert( 1500 == newsize );
	assert( 1 == testList.size() );
	testList.dump();
	printf("size is: %d\n", testList.size() );
}
