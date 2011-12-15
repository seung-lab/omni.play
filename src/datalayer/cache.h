#pragma once

#include <boost/unordered_map.hpp>
#include <string>
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

#include "thrift/storage_manager.h"
#include "thrift/storage_server.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

namespace om {
namespace datalayer {

// Thread local.  Lifetime of the data cached is tied to the lifetime of this object.
class cache
{
private:
    boost::unordered_map<std::string, std::string> data_;
    boost::shared_ptr<TTransport> managerTransport_;
    boost::shared_ptr<server::storage_managerClient> manager_;

public:
    cache();
    ~cache();

    template<typename T>
    T Get(const std::string& key)
    {
        if(data_.find(key) == data_.end()) {
            loadData(key);
        }

        return T(data_[key].data, data_[key].size);
    }

private:
    void loadData(const std::string& key);
};

}} // namespace om::datalayer::
