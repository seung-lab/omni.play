#ifndef MESHING_CHUNK_THREAD_H
#define MESHING_CHUNK_THREAD_H

#include <QThread>

class MeshingChunkThreadManager;

class MeshingChunkThread : public QThread
{
 public:
	MeshingChunkThread( MeshingChunkThreadManager* chunkMan, const int threadNum );
	void run();

 private:
	MeshingChunkThreadManager* mChunkMan;
	void doMeshStuff();
	const int m_threadNum;
};

#endif
