/**
 * Autogenerated by Thrift Compiler (0.7.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 */
#ifndef server_H
#define server_H

#include <TProcessor.h>
#include "server_types.h"

namespace om { namespace server {

class serverIf {
 public:
  virtual ~serverIf() {}
  virtual void get_chan_tile(tile& _return, const metadata& vol, const vector3d& point, const viewType::type view) = 0;
  virtual void get_seg_tiles(std::map<std::string, tile> & _return, const metadata& vol, const int32_t segId, const bbox& segBbox, const viewType::type view) = 0;
  virtual int32_t get_seg_id(const metadata& vol, const vector3d& point) = 0;
  virtual void get_seg_ids(std::vector<int32_t> & _return, const metadata& vol, const vector3d& point, const double radius, const viewType::type view) = 0;
  virtual void get_mesh(std::string& _return, const std::string& uri, const vector3i& chunk, const int32_t segId) = 0;
  virtual double compare_results(const std::vector<result> & old_results, const result& new_result) = 0;
};

class serverNull : virtual public serverIf {
 public:
  virtual ~serverNull() {}
  void get_chan_tile(tile& /* _return */, const metadata& /* vol */, const vector3d& /* point */, const viewType::type /* view */) {
    return;
  }
  void get_seg_tiles(std::map<std::string, tile> & /* _return */, const metadata& /* vol */, const int32_t /* segId */, const bbox& /* segBbox */, const viewType::type /* view */) {
    return;
  }
  int32_t get_seg_id(const metadata& /* vol */, const vector3d& /* point */) {
    int32_t _return = 0;
    return _return;
  }
  void get_seg_ids(std::vector<int32_t> & /* _return */, const metadata& /* vol */, const vector3d& /* point */, const double /* radius */, const viewType::type /* view */) {
    return;
  }
  void get_mesh(std::string& /* _return */, const std::string& /* uri */, const vector3i& /* chunk */, const int32_t /* segId */) {
    return;
  }
  double compare_results(const std::vector<result> & /* old_results */, const result& /* new_result */) {
    double _return = (double)0;
    return _return;
  }
};

typedef struct _server_get_chan_tile_args__isset {
  _server_get_chan_tile_args__isset() : vol(false), point(false), view(false) {}
  bool vol;
  bool point;
  bool view;
} _server_get_chan_tile_args__isset;

class server_get_chan_tile_args {
 public:

  server_get_chan_tile_args() {
  }

  virtual ~server_get_chan_tile_args() throw() {}

  metadata vol;
  vector3d point;
  viewType::type view;

  _server_get_chan_tile_args__isset __isset;

  void __set_vol(const metadata& val) {
    vol = val;
  }

  void __set_point(const vector3d& val) {
    point = val;
  }

  void __set_view(const viewType::type val) {
    view = val;
  }

  bool operator == (const server_get_chan_tile_args & rhs) const
  {
    if (!(vol == rhs.vol))
      return false;
    if (!(point == rhs.point))
      return false;
    if (!(view == rhs.view))
      return false;
    return true;
  }
  bool operator != (const server_get_chan_tile_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const server_get_chan_tile_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class server_get_chan_tile_pargs {
 public:


  virtual ~server_get_chan_tile_pargs() throw() {}

  const metadata* vol;
  const vector3d* point;
  const viewType::type* view;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _server_get_chan_tile_result__isset {
  _server_get_chan_tile_result__isset() : success(false) {}
  bool success;
} _server_get_chan_tile_result__isset;

class server_get_chan_tile_result {
 public:

  server_get_chan_tile_result() {
  }

  virtual ~server_get_chan_tile_result() throw() {}

  tile success;

  _server_get_chan_tile_result__isset __isset;

  void __set_success(const tile& val) {
    success = val;
  }

  bool operator == (const server_get_chan_tile_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const server_get_chan_tile_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const server_get_chan_tile_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _server_get_chan_tile_presult__isset {
  _server_get_chan_tile_presult__isset() : success(false) {}
  bool success;
} _server_get_chan_tile_presult__isset;

class server_get_chan_tile_presult {
 public:


  virtual ~server_get_chan_tile_presult() throw() {}

  tile* success;

  _server_get_chan_tile_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _server_get_seg_tiles_args__isset {
  _server_get_seg_tiles_args__isset() : vol(false), segId(false), segBbox(false), view(false) {}
  bool vol;
  bool segId;
  bool segBbox;
  bool view;
} _server_get_seg_tiles_args__isset;

class server_get_seg_tiles_args {
 public:

  server_get_seg_tiles_args() : segId(0) {
  }

  virtual ~server_get_seg_tiles_args() throw() {}

  metadata vol;
  int32_t segId;
  bbox segBbox;
  viewType::type view;

  _server_get_seg_tiles_args__isset __isset;

  void __set_vol(const metadata& val) {
    vol = val;
  }

  void __set_segId(const int32_t val) {
    segId = val;
  }

  void __set_segBbox(const bbox& val) {
    segBbox = val;
  }

  void __set_view(const viewType::type val) {
    view = val;
  }

  bool operator == (const server_get_seg_tiles_args & rhs) const
  {
    if (!(vol == rhs.vol))
      return false;
    if (!(segId == rhs.segId))
      return false;
    if (!(segBbox == rhs.segBbox))
      return false;
    if (!(view == rhs.view))
      return false;
    return true;
  }
  bool operator != (const server_get_seg_tiles_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const server_get_seg_tiles_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class server_get_seg_tiles_pargs {
 public:


  virtual ~server_get_seg_tiles_pargs() throw() {}

  const metadata* vol;
  const int32_t* segId;
  const bbox* segBbox;
  const viewType::type* view;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _server_get_seg_tiles_result__isset {
  _server_get_seg_tiles_result__isset() : success(false) {}
  bool success;
} _server_get_seg_tiles_result__isset;

class server_get_seg_tiles_result {
 public:

  server_get_seg_tiles_result() {
  }

  virtual ~server_get_seg_tiles_result() throw() {}

  std::map<std::string, tile>  success;

  _server_get_seg_tiles_result__isset __isset;

  void __set_success(const std::map<std::string, tile> & val) {
    success = val;
  }

  bool operator == (const server_get_seg_tiles_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const server_get_seg_tiles_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const server_get_seg_tiles_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _server_get_seg_tiles_presult__isset {
  _server_get_seg_tiles_presult__isset() : success(false) {}
  bool success;
} _server_get_seg_tiles_presult__isset;

class server_get_seg_tiles_presult {
 public:


  virtual ~server_get_seg_tiles_presult() throw() {}

  std::map<std::string, tile> * success;

  _server_get_seg_tiles_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _server_get_seg_id_args__isset {
  _server_get_seg_id_args__isset() : vol(false), point(false) {}
  bool vol;
  bool point;
} _server_get_seg_id_args__isset;

class server_get_seg_id_args {
 public:

  server_get_seg_id_args() {
  }

  virtual ~server_get_seg_id_args() throw() {}

  metadata vol;
  vector3d point;

  _server_get_seg_id_args__isset __isset;

  void __set_vol(const metadata& val) {
    vol = val;
  }

  void __set_point(const vector3d& val) {
    point = val;
  }

  bool operator == (const server_get_seg_id_args & rhs) const
  {
    if (!(vol == rhs.vol))
      return false;
    if (!(point == rhs.point))
      return false;
    return true;
  }
  bool operator != (const server_get_seg_id_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const server_get_seg_id_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class server_get_seg_id_pargs {
 public:


  virtual ~server_get_seg_id_pargs() throw() {}

  const metadata* vol;
  const vector3d* point;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _server_get_seg_id_result__isset {
  _server_get_seg_id_result__isset() : success(false) {}
  bool success;
} _server_get_seg_id_result__isset;

class server_get_seg_id_result {
 public:

  server_get_seg_id_result() : success(0) {
  }

  virtual ~server_get_seg_id_result() throw() {}

  int32_t success;

  _server_get_seg_id_result__isset __isset;

  void __set_success(const int32_t val) {
    success = val;
  }

  bool operator == (const server_get_seg_id_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const server_get_seg_id_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const server_get_seg_id_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _server_get_seg_id_presult__isset {
  _server_get_seg_id_presult__isset() : success(false) {}
  bool success;
} _server_get_seg_id_presult__isset;

class server_get_seg_id_presult {
 public:


  virtual ~server_get_seg_id_presult() throw() {}

  int32_t* success;

  _server_get_seg_id_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _server_get_seg_ids_args__isset {
  _server_get_seg_ids_args__isset() : vol(false), point(false), radius(false), view(false) {}
  bool vol;
  bool point;
  bool radius;
  bool view;
} _server_get_seg_ids_args__isset;

class server_get_seg_ids_args {
 public:

  server_get_seg_ids_args() : radius(0) {
  }

  virtual ~server_get_seg_ids_args() throw() {}

  metadata vol;
  vector3d point;
  double radius;
  viewType::type view;

  _server_get_seg_ids_args__isset __isset;

  void __set_vol(const metadata& val) {
    vol = val;
  }

  void __set_point(const vector3d& val) {
    point = val;
  }

  void __set_radius(const double val) {
    radius = val;
  }

  void __set_view(const viewType::type val) {
    view = val;
  }

  bool operator == (const server_get_seg_ids_args & rhs) const
  {
    if (!(vol == rhs.vol))
      return false;
    if (!(point == rhs.point))
      return false;
    if (!(radius == rhs.radius))
      return false;
    if (!(view == rhs.view))
      return false;
    return true;
  }
  bool operator != (const server_get_seg_ids_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const server_get_seg_ids_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class server_get_seg_ids_pargs {
 public:


  virtual ~server_get_seg_ids_pargs() throw() {}

  const metadata* vol;
  const vector3d* point;
  const double* radius;
  const viewType::type* view;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _server_get_seg_ids_result__isset {
  _server_get_seg_ids_result__isset() : success(false) {}
  bool success;
} _server_get_seg_ids_result__isset;

class server_get_seg_ids_result {
 public:

  server_get_seg_ids_result() {
  }

  virtual ~server_get_seg_ids_result() throw() {}

  std::vector<int32_t>  success;

  _server_get_seg_ids_result__isset __isset;

  void __set_success(const std::vector<int32_t> & val) {
    success = val;
  }

  bool operator == (const server_get_seg_ids_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const server_get_seg_ids_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const server_get_seg_ids_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _server_get_seg_ids_presult__isset {
  _server_get_seg_ids_presult__isset() : success(false) {}
  bool success;
} _server_get_seg_ids_presult__isset;

class server_get_seg_ids_presult {
 public:


  virtual ~server_get_seg_ids_presult() throw() {}

  std::vector<int32_t> * success;

  _server_get_seg_ids_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _server_get_mesh_args__isset {
  _server_get_mesh_args__isset() : uri(false), chunk(false), segId(false) {}
  bool uri;
  bool chunk;
  bool segId;
} _server_get_mesh_args__isset;

class server_get_mesh_args {
 public:

  server_get_mesh_args() : uri(""), segId(0) {
  }

  virtual ~server_get_mesh_args() throw() {}

  std::string uri;
  vector3i chunk;
  int32_t segId;

  _server_get_mesh_args__isset __isset;

  void __set_uri(const std::string& val) {
    uri = val;
  }

  void __set_chunk(const vector3i& val) {
    chunk = val;
  }

  void __set_segId(const int32_t val) {
    segId = val;
  }

  bool operator == (const server_get_mesh_args & rhs) const
  {
    if (!(uri == rhs.uri))
      return false;
    if (!(chunk == rhs.chunk))
      return false;
    if (!(segId == rhs.segId))
      return false;
    return true;
  }
  bool operator != (const server_get_mesh_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const server_get_mesh_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class server_get_mesh_pargs {
 public:


  virtual ~server_get_mesh_pargs() throw() {}

  const std::string* uri;
  const vector3i* chunk;
  const int32_t* segId;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _server_get_mesh_result__isset {
  _server_get_mesh_result__isset() : success(false) {}
  bool success;
} _server_get_mesh_result__isset;

class server_get_mesh_result {
 public:

  server_get_mesh_result() : success("") {
  }

  virtual ~server_get_mesh_result() throw() {}

  std::string success;

  _server_get_mesh_result__isset __isset;

  void __set_success(const std::string& val) {
    success = val;
  }

  bool operator == (const server_get_mesh_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const server_get_mesh_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const server_get_mesh_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _server_get_mesh_presult__isset {
  _server_get_mesh_presult__isset() : success(false) {}
  bool success;
} _server_get_mesh_presult__isset;

class server_get_mesh_presult {
 public:


  virtual ~server_get_mesh_presult() throw() {}

  std::string* success;

  _server_get_mesh_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _server_compare_results_args__isset {
  _server_compare_results_args__isset() : old_results(false), new_result(false) {}
  bool old_results;
  bool new_result;
} _server_compare_results_args__isset;

class server_compare_results_args {
 public:

  server_compare_results_args() {
  }

  virtual ~server_compare_results_args() throw() {}

  std::vector<result>  old_results;
  result new_result;

  _server_compare_results_args__isset __isset;

  void __set_old_results(const std::vector<result> & val) {
    old_results = val;
  }

  void __set_new_result(const result& val) {
    new_result = val;
  }

  bool operator == (const server_compare_results_args & rhs) const
  {
    if (!(old_results == rhs.old_results))
      return false;
    if (!(new_result == rhs.new_result))
      return false;
    return true;
  }
  bool operator != (const server_compare_results_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const server_compare_results_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class server_compare_results_pargs {
 public:


  virtual ~server_compare_results_pargs() throw() {}

  const std::vector<result> * old_results;
  const result* new_result;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _server_compare_results_result__isset {
  _server_compare_results_result__isset() : success(false) {}
  bool success;
} _server_compare_results_result__isset;

class server_compare_results_result {
 public:

  server_compare_results_result() : success(0) {
  }

  virtual ~server_compare_results_result() throw() {}

  double success;

  _server_compare_results_result__isset __isset;

  void __set_success(const double val) {
    success = val;
  }

  bool operator == (const server_compare_results_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const server_compare_results_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const server_compare_results_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _server_compare_results_presult__isset {
  _server_compare_results_presult__isset() : success(false) {}
  bool success;
} _server_compare_results_presult__isset;

class server_compare_results_presult {
 public:


  virtual ~server_compare_results_presult() throw() {}

  double* success;

  _server_compare_results_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

class serverClient : virtual public serverIf {
 public:
  serverClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) :
    piprot_(prot),
    poprot_(prot) {
    iprot_ = prot.get();
    oprot_ = prot.get();
  }
  serverClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) :
    piprot_(iprot),
    poprot_(oprot) {
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void get_chan_tile(tile& _return, const metadata& vol, const vector3d& point, const viewType::type view);
  void send_get_chan_tile(const metadata& vol, const vector3d& point, const viewType::type view);
  void recv_get_chan_tile(tile& _return);
  void get_seg_tiles(std::map<std::string, tile> & _return, const metadata& vol, const int32_t segId, const bbox& segBbox, const viewType::type view);
  void send_get_seg_tiles(const metadata& vol, const int32_t segId, const bbox& segBbox, const viewType::type view);
  void recv_get_seg_tiles(std::map<std::string, tile> & _return);
  int32_t get_seg_id(const metadata& vol, const vector3d& point);
  void send_get_seg_id(const metadata& vol, const vector3d& point);
  int32_t recv_get_seg_id();
  void get_seg_ids(std::vector<int32_t> & _return, const metadata& vol, const vector3d& point, const double radius, const viewType::type view);
  void send_get_seg_ids(const metadata& vol, const vector3d& point, const double radius, const viewType::type view);
  void recv_get_seg_ids(std::vector<int32_t> & _return);
  void get_mesh(std::string& _return, const std::string& uri, const vector3i& chunk, const int32_t segId);
  void send_get_mesh(const std::string& uri, const vector3i& chunk, const int32_t segId);
  void recv_get_mesh(std::string& _return);
  double compare_results(const std::vector<result> & old_results, const result& new_result);
  void send_compare_results(const std::vector<result> & old_results, const result& new_result);
  double recv_compare_results();
 protected:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class serverProcessor : virtual public ::apache::thrift::TProcessor {
 protected:
  boost::shared_ptr<serverIf> iface_;
  virtual bool process_fn(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, std::string& fname, int32_t seqid, void* callContext);
 private:
  std::map<std::string, void (serverProcessor::*)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*)> processMap_;
  void process_get_chan_tile(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_get_seg_tiles(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_get_seg_id(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_get_seg_ids(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_get_mesh(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_compare_results(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  serverProcessor(boost::shared_ptr<serverIf> iface) :
    iface_(iface) {
    processMap_["get_chan_tile"] = &serverProcessor::process_get_chan_tile;
    processMap_["get_seg_tiles"] = &serverProcessor::process_get_seg_tiles;
    processMap_["get_seg_id"] = &serverProcessor::process_get_seg_id;
    processMap_["get_seg_ids"] = &serverProcessor::process_get_seg_ids;
    processMap_["get_mesh"] = &serverProcessor::process_get_mesh;
    processMap_["compare_results"] = &serverProcessor::process_compare_results;
  }

  virtual bool process(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot, void* callContext);
  virtual ~serverProcessor() {}
};

class serverMultiface : virtual public serverIf {
 public:
  serverMultiface(std::vector<boost::shared_ptr<serverIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~serverMultiface() {}
 protected:
  std::vector<boost::shared_ptr<serverIf> > ifaces_;
  serverMultiface() {}
  void add(boost::shared_ptr<serverIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  void get_chan_tile(tile& _return, const metadata& vol, const vector3d& point, const viewType::type view) {
    size_t sz = ifaces_.size();
    for (size_t i = 0; i < sz; ++i) {
      if (i == sz - 1) {
        ifaces_[i]->get_chan_tile(_return, vol, point, view);
        return;
      } else {
        ifaces_[i]->get_chan_tile(_return, vol, point, view);
      }
    }
  }

  void get_seg_tiles(std::map<std::string, tile> & _return, const metadata& vol, const int32_t segId, const bbox& segBbox, const viewType::type view) {
    size_t sz = ifaces_.size();
    for (size_t i = 0; i < sz; ++i) {
      if (i == sz - 1) {
        ifaces_[i]->get_seg_tiles(_return, vol, segId, segBbox, view);
        return;
      } else {
        ifaces_[i]->get_seg_tiles(_return, vol, segId, segBbox, view);
      }
    }
  }

  int32_t get_seg_id(const metadata& vol, const vector3d& point) {
    size_t sz = ifaces_.size();
    for (size_t i = 0; i < sz; ++i) {
      if (i == sz - 1) {
        return ifaces_[i]->get_seg_id(vol, point);
      } else {
        ifaces_[i]->get_seg_id(vol, point);
      }
    }
  }

  void get_seg_ids(std::vector<int32_t> & _return, const metadata& vol, const vector3d& point, const double radius, const viewType::type view) {
    size_t sz = ifaces_.size();
    for (size_t i = 0; i < sz; ++i) {
      if (i == sz - 1) {
        ifaces_[i]->get_seg_ids(_return, vol, point, radius, view);
        return;
      } else {
        ifaces_[i]->get_seg_ids(_return, vol, point, radius, view);
      }
    }
  }

  void get_mesh(std::string& _return, const std::string& uri, const vector3i& chunk, const int32_t segId) {
    size_t sz = ifaces_.size();
    for (size_t i = 0; i < sz; ++i) {
      if (i == sz - 1) {
        ifaces_[i]->get_mesh(_return, uri, chunk, segId);
        return;
      } else {
        ifaces_[i]->get_mesh(_return, uri, chunk, segId);
      }
    }
  }

  double compare_results(const std::vector<result> & old_results, const result& new_result) {
    size_t sz = ifaces_.size();
    for (size_t i = 0; i < sz; ++i) {
      if (i == sz - 1) {
        return ifaces_[i]->compare_results(old_results, new_result);
      } else {
        ifaces_[i]->compare_results(old_results, new_result);
      }
    }
  }

};

}} // namespace

#endif
