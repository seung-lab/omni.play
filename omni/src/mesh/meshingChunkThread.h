#ifndef MESHING_CHUNK_THREAD_H
#define MESHING_CHUNK_THREAD_H

#include <QThread>

class MeshingChunkThreadManager;

class MeshingChunkThread : public QThread
{
 public:
	MeshingChunkThread( MeshingChunkThreadManager* chunkMan );
	void run();

 private:
	MeshingChunkThreadManager* mChunkMan;
};

#endif
