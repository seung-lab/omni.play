/**
 * Autogenerated by Thrift Compiler (0.7.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 */
#ifndef server_TYPES_H
#define server_TYPES_H

#include <Thrift.h>
#include <TApplicationException.h>
#include <protocol/TProtocol.h>
#include <transport/TTransport.h>



namespace om { namespace server {

struct viewType {
  enum type {
    XY_VIEW = 1,
    XZ_VIEW = 2,
    ZY_VIEW = 3
  };
};

extern const std::map<int, const char*> _viewType_VALUES_TO_NAMES;

struct dataType {
  enum type {
    INT8 = 1,
    UINT8 = 2,
    INT32 = 3,
    UINT32 = 4,
    FLOAT = 5
  };
};

extern const std::map<int, const char*> _dataType_VALUES_TO_NAMES;

typedef struct _vector3d__isset {
  _vector3d__isset() : x(false), y(false), z(false) {}
  bool x;
  bool y;
  bool z;
} _vector3d__isset;

class vector3d {
 public:

  static const char* ascii_fingerprint; // = "EFFAD640FBA2CA56C50155B2A4545897";
  static const uint8_t binary_fingerprint[16]; // = {0xEF,0xFA,0xD6,0x40,0xFB,0xA2,0xCA,0x56,0xC5,0x01,0x55,0xB2,0xA4,0x54,0x58,0x97};

  vector3d() : x(0), y(0), z(0) {
  }

  virtual ~vector3d() throw() {}

  double x;
  double y;
  double z;

  _vector3d__isset __isset;

  void __set_x(const double val) {
    x = val;
  }

  void __set_y(const double val) {
    y = val;
  }

  void __set_z(const double val) {
    z = val;
  }

  bool operator == (const vector3d & rhs) const
  {
    if (!(x == rhs.x))
      return false;
    if (!(y == rhs.y))
      return false;
    if (!(z == rhs.z))
      return false;
    return true;
  }
  bool operator != (const vector3d &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const vector3d & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _vector3i__isset {
  _vector3i__isset() : x(false), y(false), z(false) {}
  bool x;
  bool y;
  bool z;
} _vector3i__isset;

class vector3i {
 public:

  static const char* ascii_fingerprint; // = "6435B39C87AB0E30F30BEDEFD7328C0D";
  static const uint8_t binary_fingerprint[16]; // = {0x64,0x35,0xB3,0x9C,0x87,0xAB,0x0E,0x30,0xF3,0x0B,0xED,0xEF,0xD7,0x32,0x8C,0x0D};

  vector3i() : x(0), y(0), z(0) {
  }

  virtual ~vector3i() throw() {}

  int32_t x;
  int32_t y;
  int32_t z;

  _vector3i__isset __isset;

  void __set_x(const int32_t val) {
    x = val;
  }

  void __set_y(const int32_t val) {
    y = val;
  }

  void __set_z(const int32_t val) {
    z = val;
  }

  bool operator == (const vector3i & rhs) const
  {
    if (!(x == rhs.x))
      return false;
    if (!(y == rhs.y))
      return false;
    if (!(z == rhs.z))
      return false;
    return true;
  }
  bool operator != (const vector3i &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const vector3i & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _bbox__isset {
  _bbox__isset() : min(false), max(false) {}
  bool min;
  bool max;
} _bbox__isset;

class bbox {
 public:

  static const char* ascii_fingerprint; // = "EFD57E86E580906F433F480C60E1C408";
  static const uint8_t binary_fingerprint[16]; // = {0xEF,0xD5,0x7E,0x86,0xE5,0x80,0x90,0x6F,0x43,0x3F,0x48,0x0C,0x60,0xE1,0xC4,0x08};

  bbox() {
  }

  virtual ~bbox() throw() {}

  vector3d min;
  vector3d max;

  _bbox__isset __isset;

  void __set_min(const vector3d& val) {
    min = val;
  }

  void __set_max(const vector3d& val) {
    max = val;
  }

  bool operator == (const bbox & rhs) const
  {
    if (!(min == rhs.min))
      return false;
    if (!(max == rhs.max))
      return false;
    return true;
  }
  bool operator != (const bbox &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const bbox & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _tile__isset {
  _tile__isset() : view(false), bounds(false), data(false) {}
  bool view;
  bool bounds;
  bool data;
} _tile__isset;

class tile {
 public:

  static const char* ascii_fingerprint; // = "89F61E823E1BFA34748CDA01327FE40C";
  static const uint8_t binary_fingerprint[16]; // = {0x89,0xF6,0x1E,0x82,0x3E,0x1B,0xFA,0x34,0x74,0x8C,0xDA,0x01,0x32,0x7F,0xE4,0x0C};

  tile() : data("") {
  }

  virtual ~tile() throw() {}

  viewType::type view;
  bbox bounds;
  std::string data;

  _tile__isset __isset;

  void __set_view(const viewType::type val) {
    view = val;
  }

  void __set_bounds(const bbox& val) {
    bounds = val;
  }

  void __set_data(const std::string& val) {
    data = val;
  }

  bool operator == (const tile & rhs) const
  {
    if (!(view == rhs.view))
      return false;
    if (!(bounds == rhs.bounds))
      return false;
    if (!(data == rhs.data))
      return false;
    return true;
  }
  bool operator != (const tile &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const tile & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _result__isset {
  _result__isset() : selected(false) {}
  bool selected;
} _result__isset;

class result {
 public:

  static const char* ascii_fingerprint; // = "A803C54EAD95E24D90C5E66FB98EA72B";
  static const uint8_t binary_fingerprint[16]; // = {0xA8,0x03,0xC5,0x4E,0xAD,0x95,0xE2,0x4D,0x90,0xC5,0xE6,0x6F,0xB9,0x8E,0xA7,0x2B};

  result() {
  }

  virtual ~result() throw() {}

  std::vector<int32_t>  selected;

  _result__isset __isset;

  void __set_selected(const std::vector<int32_t> & val) {
    selected = val;
  }

  bool operator == (const result & rhs) const
  {
    if (!(selected == rhs.selected))
      return false;
    return true;
  }
  bool operator != (const result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _value__isset {
  _value__isset() : int8(false), int16(false), int32(false), int64(false), double_float(false) {}
  bool int8;
  bool int16;
  bool int32;
  bool int64;
  bool double_float;
} _value__isset;

class value {
 public:

  static const char* ascii_fingerprint; // = "1A5F7E9D274F7B4947E93F6F966E68DC";
  static const uint8_t binary_fingerprint[16]; // = {0x1A,0x5F,0x7E,0x9D,0x27,0x4F,0x7B,0x49,0x47,0xE9,0x3F,0x6F,0x96,0x6E,0x68,0xDC};

  value() : int8(0), int16(0), int32(0), int64(0), double_float(0) {
  }

  virtual ~value() throw() {}

  dataType::type type;
  int8_t int8;
  int16_t int16;
  int32_t int32;
  int64_t int64;
  double double_float;

  _value__isset __isset;

  void __set_type(const dataType::type val) {
    type = val;
  }

  void __set_int8(const int8_t val) {
    int8 = val;
    __isset.int8 = true;
  }

  void __set_int16(const int16_t val) {
    int16 = val;
    __isset.int16 = true;
  }

  void __set_int32(const int32_t val) {
    int32 = val;
    __isset.int32 = true;
  }

  void __set_int64(const int64_t val) {
    int64 = val;
    __isset.int64 = true;
  }

  void __set_double_float(const double val) {
    double_float = val;
    __isset.double_float = true;
  }

  bool operator == (const value & rhs) const
  {
    if (!(type == rhs.type))
      return false;
    if (__isset.int8 != rhs.__isset.int8)
      return false;
    else if (__isset.int8 && !(int8 == rhs.int8))
      return false;
    if (__isset.int16 != rhs.__isset.int16)
      return false;
    else if (__isset.int16 && !(int16 == rhs.int16))
      return false;
    if (__isset.int32 != rhs.__isset.int32)
      return false;
    else if (__isset.int32 && !(int32 == rhs.int32))
      return false;
    if (__isset.int64 != rhs.__isset.int64)
      return false;
    else if (__isset.int64 && !(int64 == rhs.int64))
      return false;
    if (__isset.double_float != rhs.__isset.double_float)
      return false;
    else if (__isset.double_float && !(double_float == rhs.double_float))
      return false;
    return true;
  }
  bool operator != (const value &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const value & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _metadata__isset {
  _metadata__isset() : volId(false), bounds(false), resolution(false), type(false), chunkDims(false) {}
  bool volId;
  bool bounds;
  bool resolution;
  bool type;
  bool chunkDims;
} _metadata__isset;

class metadata {
 public:

  static const char* ascii_fingerprint; // = "66D3FD5740968CE2ACC5A5F0EE9F325F";
  static const uint8_t binary_fingerprint[16]; // = {0x66,0xD3,0xFD,0x57,0x40,0x96,0x8C,0xE2,0xAC,0xC5,0xA5,0xF0,0xEE,0x9F,0x32,0x5F};

  metadata() : volId(0) {
  }

  virtual ~metadata() throw() {}

  int32_t volId;
  bbox bounds;
  vector3i resolution;
  dataType::type type;
  vector3i chunkDims;

  _metadata__isset __isset;

  void __set_volId(const int32_t val) {
    volId = val;
  }

  void __set_bounds(const bbox& val) {
    bounds = val;
  }

  void __set_resolution(const vector3i& val) {
    resolution = val;
  }

  void __set_type(const dataType::type val) {
    type = val;
  }

  void __set_chunkDims(const vector3i& val) {
    chunkDims = val;
  }

  bool operator == (const metadata & rhs) const
  {
    if (!(volId == rhs.volId))
      return false;
    if (!(bounds == rhs.bounds))
      return false;
    if (!(resolution == rhs.resolution))
      return false;
    if (!(type == rhs.type))
      return false;
    if (!(chunkDims == rhs.chunkDims))
      return false;
    return true;
  }
  bool operator != (const metadata &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const metadata & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

}} // namespace

#endif
