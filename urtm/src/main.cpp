
#include <iostream>
#include <protocol/TBinaryProtocol.h>
#include <transport/TSocket.h>
#include <transport/TTransportUtils.h>
#include "RealTimeMesher.h"

#include <boost/shared_ptr.hpp>

#include "volume/segmentation.h"
#include "handler/handler.h"
#include <sstream>
#include "zi/arguments.hpp"
#include "datalayer/file.h"
#include "datalayer/paths.hpp"

ZiARG_string(mesher, "localhost", "Mesher's address");
ZiARG_int32(mport, 9099, "Mesher's port");
ZiARG_int32(cell, 0, "Cell to add to.");
ZiARG_string(path, "", "Path to the volume to add.");
ZiARG_uint32_set(segs, "List of segments to be added.");

using namespace om;

typedef boost::shared_ptr<zi::mesh::RealTimeMesherIf> MesherPtr;
MesherPtr makeMesher(std::string host, int port) {
  using namespace apache::thrift;
  using namespace apache::thrift::transport;
  using namespace apache::thrift::protocol;

  boost::shared_ptr<TSocket> socket =
      boost::shared_ptr<TSocket>(new TSocket(host, port));

  boost::shared_ptr<TTransport> transport =
      boost::shared_ptr<TTransport>(new TFramedTransport(socket));

  boost::shared_ptr<TProtocol> protocol =
      boost::shared_ptr<TProtocol>(new TBinaryProtocol(transport));

  boost::shared_ptr<zi::mesh::RealTimeMesherClient> mesher =
      boost::shared_ptr<zi::mesh::RealTimeMesherClient>(
          new zi::mesh::RealTimeMesherClient(protocol));

  try {
    transport->open();
    if (!transport->isOpen()) {
      log_errors(io) << "WTF!!!!";
    }
  }
  catch (apache::thrift::TException & tx) {
    throw(tx);
  }

  return mesher;
}

int main(int argc, char *argv[]) {
  zi::parse_arguments(argc, argv, true);

  if (!file::exists(ZiARG_path)) {
    log_errors(io) << "Project not found.";
  }

  om::file::Paths p(ZiARG_path);
  volume::Segmentation v(p.Segmentation(1));

  return handler::modify_global_mesh_data(
      std::bind(makeMesher, ZiARG_mesher, ZiARG_mport), v, ZiARG_segs,
      ZiARG_segs, ZiARG_cell * 10 + 1);
}