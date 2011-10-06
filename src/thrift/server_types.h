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



namespace om { namespace common {

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

typedef struct _tile__isset {
  _tile__isset() : data(false) {}
  bool data;
} _tile__isset;

class tile {
 public:

  static const char* ascii_fingerprint; // = "EFB929595D312AC8F305D5A794CFEDA1";
  static const uint8_t binary_fingerprint[16]; // = {0xEF,0xB9,0x29,0x59,0x5D,0x31,0x2A,0xC8,0xF3,0x05,0xD5,0xA7,0x94,0xCF,0xED,0xA1};

  tile() : data("") {
  }

  virtual ~tile() throw() {}

  std::string data;

  _tile__isset __isset;

  void __set_data(const std::string& val) {
    data = val;
  }

  bool operator == (const tile & rhs) const
  {
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

typedef struct _click_info__isset {
  _click_info__isset() : segId(false), tile_data(false) {}
  bool segId;
  bool tile_data;
} _click_info__isset;

class click_info {
 public:

  static const char* ascii_fingerprint; // = "023FCF385EF954E87275FD4CD13D624E";
  static const uint8_t binary_fingerprint[16]; // = {0x02,0x3F,0xCF,0x38,0x5E,0xF9,0x54,0xE8,0x72,0x75,0xFD,0x4C,0xD1,0x3D,0x62,0x4E};

  click_info() : segId(0) {
  }

  virtual ~click_info() throw() {}

  int32_t segId;
  tile tile_data;

  _click_info__isset __isset;

  void __set_segId(const int32_t val) {
    segId = val;
  }

  void __set_tile_data(const tile& val) {
    tile_data = val;
  }

  bool operator == (const click_info & rhs) const
  {
    if (!(segId == rhs.segId))
      return false;
    if (!(tile_data == rhs.tile_data))
      return false;
    return true;
  }
  bool operator != (const click_info &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const click_info & ) const;

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

}} // namespace

#endif
