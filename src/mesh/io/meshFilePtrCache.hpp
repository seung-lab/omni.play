#pragma once

#include "common/common.h"
#include "mesh/iochunk/meshChunkAllocTable.hpp"
#include "mesh/iochunk/meshChunkDataReader.hpp"
#include "mesh/iochunk/meshChunkDataWriter.hpp"
#include "mesh/ioomRingBuffer.hpp"
#include "mesh/meshCoord.h"
#include "threads/omTaskManager.hpp"
#include "zi/omMutex.h"

class meshChunkAllocTableV2;
class meshChunkDataWriter;

class meshFilePtrCache {
private:
    segmentation *const segmentation_;
    const double threshold_;

    std::map<coords::chunkCoord, om::shared_ptr<meshChunkAllocTableV2> > tables_;
    std::map<coords::chunkCoord, om::shared_ptr<meshChunkDataWriter> > data_;
    zi::rwmutex lock_;

    OmThreadPool threadPool_;

    // limit number of memory-mapped files
    OmRingBuffer<meshChunkAllocTableV2> mappedFiles_;

public:
    meshFilePtrCache(segmentation* segmentation, const double threshold)
        : segmentation_(segmentation)
        , threshold_(threshold)
    {
        threadPool_.start();
    }

    ~meshFilePtrCache(){
        Stop();
    }

    void Join(){
        threadPool_.join();
    }

    void Stop(){
        threadPool_.stop();
    }

    void FlushMappedFiles()
    {
        std::cout << "flushing mesh allocation tables..." << std::flush;
        mappedFiles_.Clear();
        std::cout << "done\n";
    }

    void AddTaskBack(const om::shared_ptr<zi::runnable> job){
        threadPool_.push_back(job);
    }

    uint32_t NumTasks() const {
        return threadPool_.getTaskCount();
    }

    void RegisterMappedFile(meshChunkAllocTableV2* table){
        mappedFiles_.Put(table);
    }

    meshChunkAllocTableV2* GetAllocTable(const coords::chunkCoord& coord)
    {
        zi::rwmutex::write_guard g(lock_);

        if(!tables_.count(coord))
        {
            tables_[coord] = boost::make_shared<meshChunkAllocTableV2>(this, segmentation_,
                                                                      coord, threshold_);
        }

        return tables_[coord].get();
    }

    meshChunkDataWriter* GetWriter(const coords::chunkCoord& coord)
    {
        zi::rwmutex::write_guard g(lock_);

        if(!data_.count(coord))
        {
            data_[coord] = boost::make_shared<meshChunkDataWriter>(segmentation_,
                                                                    coord, threshold_);
        }

        return data_[coord].get();
    }
};

