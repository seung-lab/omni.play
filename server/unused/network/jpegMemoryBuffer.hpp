#pragma once

#include <vector>
#include "boost/bind.hpp"

extern "C"
{
#include <jpeglib.h>
#include <jerror.h>
}

#define BLOCK_SIZE 16384

namespace om {
namespace jpeg {

// based on: http://stackoverflow.com/questions/4559648/write-to-memory-buffer-instead-of-file-with-libjpeg
class memoryBuffer {
private:
    std::vector<JOCTET> * buffer_;

public:
    memoryBuffer(std::vector<JOCTET>* buffer)
        : buffer_(buffer)
    {}

    void init(j_compress_ptr cinfo)
    {
        cinfo->dest->init_destination = &memoryBuffer::my_init_destination;
        cinfo->dest->empty_output_buffer = &this->my_empty_output_buffer;
        cinfo->dest->term_destination = &this->my_term_destination;
    }

private:
    void my_init_destination(j_compress_ptr cinfo)
    {
        buffer_->resize(BLOCK_SIZE);
        cinfo->dest->next_output_byte = &(*buffer_)[0];
        cinfo->dest->free_in_buffer = buffer_->size();
    }

    boolean my_empty_output_buffer(j_compress_ptr cinfo)
    {
        size_t oldsize = buffer_->size();
        buffer_->resize(oldsize + BLOCK_SIZE);
        cinfo->dest->next_output_byte = &(*buffer_)[oldsize];
        cinfo->dest->free_in_buffer = buffer_->size() - oldsize;
        return true;
    }

    void my_term_destination(j_compress_ptr cinfo)
    {
        buffer_->resize(buffer_.size() - cinfo->dest->free_in_buffer);
    }

};

}
}
