#include "datalayer/cache.h"
#include <boost/make_shared.hpp>

namespace om {
namespace datalayer {

cache::cache()
{
    boost::shared_ptr<TSocket> socket(new TSocket("localhost", 9090));
    managerTransport_ = boost::make_shared<TBufferedTransport>(socket);
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(managerTransport_));

    manager_ = boost::make_shared<server::storage_managerClient>(protocol);
    managerTransport_->open();
}

cache::~cache() {
    managerTransport_->close();
}

void cache::loadData(const std::string& key)
{
    server::server_id id;
    manager_->get_server(id, key);

    boost::shared_ptr<TSocket> socket(new TSocket(id.address, id.port));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

    server::storage_serverClient s(protocol);
    transport->open();
    s.get(data_[key], key);
    transport->close();
}

}} // namespace om::datalayer::
