#pragma once

#include <boost/unordered_map.hpp>
#include <boost/variant.hpp>
#include <string>
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

#include "thrift/storage_manager.h"
#include "thrift/storage_server.h"

#include "chunks/chunk.hpp"
#include "segment/page.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

namespace om {
namespace datalayer {

typedef boost::variant< chunks::Chunk<int8_t>,
                        chunks::Chunk<uint8_t>,
                        chunks::Chunk<int32_t>,
                        chunks::Chunk<uint32_t>,
                        chunks::Chunk<float>,
                        segment::page > cached;

// Thread local.  Lifetime of the data cached is tied to the lifetime of this object.
class cache
{
private:
    boost::unordered_map<std::string, std::string> data_;
    boost::unordered_map<std::string, cached> objects_;
    boost::shared_ptr<TTransport> managerTransport_;
    boost::shared_ptr<server::storage_managerClient> manager_;

public:
    cache();
    ~cache();

    template<typename T>
    T* Get(const std::string& key)
    {
        if(!objects_.count(key))
        {
            if(!loadData(key)) {
                return NULL; // Didn't find the data.
            } else {
                objects_[key] = T(const_cast<char*>(data_[key].data()), data_[key].size());
            }
        }

        T& ret = boost::get<T>(objects_[key]);

        return &ret;
    }

private:
    bool loadData(const std::string& key);
};

}} // namespace om::datalayer::
