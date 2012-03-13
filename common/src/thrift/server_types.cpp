/**
 * Autogenerated by Thrift Compiler (0.8.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "server_types.h"

namespace om { namespace server {

int _kviewTypeValues[] = {
  viewType::XY_VIEW,
  viewType::XZ_VIEW,
  viewType::ZY_VIEW
};
const char* _kviewTypeNames[] = {
  "XY_VIEW",
  "XZ_VIEW",
  "ZY_VIEW"
};
const std::map<int, const char*> _viewType_VALUES_TO_NAMES(::apache::thrift::TEnumIterator(3, _kviewTypeValues, _kviewTypeNames), ::apache::thrift::TEnumIterator(-1, NULL, NULL));

int _kdataTypeValues[] = {
  dataType::INT8,
  dataType::UINT8,
  dataType::INT32,
  dataType::UINT32,
  dataType::FLOAT
};
const char* _kdataTypeNames[] = {
  "INT8",
  "UINT8",
  "INT32",
  "UINT32",
  "FLOAT"
};
const std::map<int, const char*> _dataType_VALUES_TO_NAMES(::apache::thrift::TEnumIterator(5, _kdataTypeValues, _kdataTypeNames), ::apache::thrift::TEnumIterator(-1, NULL, NULL));

int _kvolTypeValues[] = {
  volType::CHANNEL,
  volType::SEGMENTATION
};
const char* _kvolTypeNames[] = {
  "CHANNEL",
  "SEGMENTATION"
};
const std::map<int, const char*> _volType_VALUES_TO_NAMES(::apache::thrift::TEnumIterator(2, _kvolTypeValues, _kvolTypeNames), ::apache::thrift::TEnumIterator(-1, NULL, NULL));

const char* vector3d::ascii_fingerprint = "EFFAD640FBA2CA56C50155B2A4545897";
const uint8_t vector3d::binary_fingerprint[16] = {0xEF,0xFA,0xD6,0x40,0xFB,0xA2,0xCA,0x56,0xC5,0x01,0x55,0xB2,0xA4,0x54,0x58,0x97};

uint32_t vector3d::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_DOUBLE) {
          xfer += iprot->readDouble(this->x);
          this->__isset.x = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_DOUBLE) {
          xfer += iprot->readDouble(this->y);
          this->__isset.y = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_DOUBLE) {
          xfer += iprot->readDouble(this->z);
          this->__isset.z = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t vector3d::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("vector3d");
  xfer += oprot->writeFieldBegin("x", ::apache::thrift::protocol::T_DOUBLE, 1);
  xfer += oprot->writeDouble(this->x);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("y", ::apache::thrift::protocol::T_DOUBLE, 2);
  xfer += oprot->writeDouble(this->y);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("z", ::apache::thrift::protocol::T_DOUBLE, 3);
  xfer += oprot->writeDouble(this->z);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

const char* vector3i::ascii_fingerprint = "6435B39C87AB0E30F30BEDEFD7328C0D";
const uint8_t vector3i::binary_fingerprint[16] = {0x64,0x35,0xB3,0x9C,0x87,0xAB,0x0E,0x30,0xF3,0x0B,0xED,0xEF,0xD7,0x32,0x8C,0x0D};

uint32_t vector3i::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->x);
          this->__isset.x = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->y);
          this->__isset.y = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->z);
          this->__isset.z = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t vector3i::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("vector3i");
  xfer += oprot->writeFieldBegin("x", ::apache::thrift::protocol::T_I32, 1);
  xfer += oprot->writeI32(this->x);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("y", ::apache::thrift::protocol::T_I32, 2);
  xfer += oprot->writeI32(this->y);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("z", ::apache::thrift::protocol::T_I32, 3);
  xfer += oprot->writeI32(this->z);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

const char* edge::ascii_fingerprint = "AA65D1E80A832794F68DFF3C92410597";
const uint8_t edge::binary_fingerprint[16] = {0xAA,0x65,0xD1,0xE8,0x0A,0x83,0x27,0x94,0xF6,0x8D,0xFF,0x3C,0x92,0x41,0x05,0x97};

uint32_t edge::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->a);
          this->__isset.a = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->b);
          this->__isset.b = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_DOUBLE) {
          xfer += iprot->readDouble(this->value);
          this->__isset.value = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t edge::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("edge");
  xfer += oprot->writeFieldBegin("a", ::apache::thrift::protocol::T_I32, 1);
  xfer += oprot->writeI32(this->a);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("b", ::apache::thrift::protocol::T_I32, 2);
  xfer += oprot->writeI32(this->b);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("value", ::apache::thrift::protocol::T_DOUBLE, 3);
  xfer += oprot->writeDouble(this->value);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

const char* bbox::ascii_fingerprint = "EFD57E86E580906F433F480C60E1C408";
const uint8_t bbox::binary_fingerprint[16] = {0xEF,0xD5,0x7E,0x86,0xE5,0x80,0x90,0x6F,0x43,0x3F,0x48,0x0C,0x60,0xE1,0xC4,0x08};

uint32_t bbox::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRUCT) {
          xfer += this->min.read(iprot);
          this->__isset.min = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_STRUCT) {
          xfer += this->max.read(iprot);
          this->__isset.max = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t bbox::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("bbox");
  xfer += oprot->writeFieldBegin("min", ::apache::thrift::protocol::T_STRUCT, 1);
  xfer += this->min.write(oprot);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("max", ::apache::thrift::protocol::T_STRUCT, 2);
  xfer += this->max.write(oprot);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

const char* segData::ascii_fingerprint = "B3DA054E3D0E1F6B4B2B99C776187A63";
const uint8_t segData::binary_fingerprint[16] = {0xB3,0xDA,0x05,0x4E,0x3D,0x0E,0x1F,0x6B,0x4B,0x2B,0x99,0xC7,0x76,0x18,0x7A,0x63};

uint32_t segData::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRUCT) {
          xfer += this->bounds.read(iprot);
          this->__isset.bounds = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->size);
          this->__isset.size = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t segData::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("segData");
  xfer += oprot->writeFieldBegin("bounds", ::apache::thrift::protocol::T_STRUCT, 1);
  xfer += this->bounds.write(oprot);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("size", ::apache::thrift::protocol::T_I32, 2);
  xfer += oprot->writeI32(this->size);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

const char* tile::ascii_fingerprint = "89F61E823E1BFA34748CDA01327FE40C";
const uint8_t tile::binary_fingerprint[16] = {0x89,0xF6,0x1E,0x82,0x3E,0x1B,0xFA,0x34,0x74,0x8C,0xDA,0x01,0x32,0x7F,0xE4,0x0C};

uint32_t tile::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          int32_t ecast0;
          xfer += iprot->readI32(ecast0);
          this->view = (viewType::type)ecast0;
          this->__isset.view = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_STRUCT) {
          xfer += this->bounds.read(iprot);
          this->__isset.bounds = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readBinary(this->data);
          this->__isset.data = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t tile::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("tile");
  xfer += oprot->writeFieldBegin("view", ::apache::thrift::protocol::T_I32, 1);
  xfer += oprot->writeI32((int32_t)this->view);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("bounds", ::apache::thrift::protocol::T_STRUCT, 2);
  xfer += this->bounds.write(oprot);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("data", ::apache::thrift::protocol::T_STRING, 3);
  xfer += oprot->writeBinary(this->data);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

const char* result::ascii_fingerprint = "A803C54EAD95E24D90C5E66FB98EA72B";
const uint8_t result::binary_fingerprint[16] = {0xA8,0x03,0xC5,0x4E,0xAD,0x95,0xE2,0x4D,0x90,0xC5,0xE6,0x6F,0xB9,0x8E,0xA7,0x2B};

uint32_t result::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_LIST) {
          {
            this->selected.clear();
            uint32_t _size1;
            ::apache::thrift::protocol::TType _etype4;
            iprot->readListBegin(_etype4, _size1);
            this->selected.resize(_size1);
            uint32_t _i5;
            for (_i5 = 0; _i5 < _size1; ++_i5)
            {
              xfer += iprot->readI32(this->selected[_i5]);
            }
            iprot->readListEnd();
          }
          this->__isset.selected = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t result::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("result");
  xfer += oprot->writeFieldBegin("selected", ::apache::thrift::protocol::T_LIST, 1);
  {
    xfer += oprot->writeListBegin(::apache::thrift::protocol::T_I32, static_cast<uint32_t>(this->selected.size()));
    std::vector<int32_t> ::const_iterator _iter6;
    for (_iter6 = this->selected.begin(); _iter6 != this->selected.end(); ++_iter6)
    {
      xfer += oprot->writeI32((*_iter6));
    }
    xfer += oprot->writeListEnd();
  }
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

const char* metadata::ascii_fingerprint = "B9F2183D0FD0EF0DCA59C160A61E9191";
const uint8_t metadata::binary_fingerprint[16] = {0xB9,0xF2,0x18,0x3D,0x0F,0xD0,0xEF,0x0D,0xCA,0x59,0xC1,0x60,0xA6,0x1E,0x91,0x91};

uint32_t metadata::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->uri);
          this->__isset.uri = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_STRUCT) {
          xfer += this->bounds.read(iprot);
          this->__isset.bounds = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_STRUCT) {
          xfer += this->resolution.read(iprot);
          this->__isset.resolution = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 4:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          int32_t ecast7;
          xfer += iprot->readI32(ecast7);
          this->type = (dataType::type)ecast7;
          this->__isset.type = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 5:
        if (ftype == ::apache::thrift::protocol::T_STRUCT) {
          xfer += this->chunkDims.read(iprot);
          this->__isset.chunkDims = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 6:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->mipLevel);
          this->__isset.mipLevel = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 7:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          int32_t ecast8;
          xfer += iprot->readI32(ecast8);
          this->vol_type = (volType::type)ecast8;
          this->__isset.vol_type = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t metadata::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("metadata");
  xfer += oprot->writeFieldBegin("uri", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeString(this->uri);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("bounds", ::apache::thrift::protocol::T_STRUCT, 2);
  xfer += this->bounds.write(oprot);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("resolution", ::apache::thrift::protocol::T_STRUCT, 3);
  xfer += this->resolution.write(oprot);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("type", ::apache::thrift::protocol::T_I32, 4);
  xfer += oprot->writeI32((int32_t)this->type);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("chunkDims", ::apache::thrift::protocol::T_STRUCT, 5);
  xfer += this->chunkDims.write(oprot);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("mipLevel", ::apache::thrift::protocol::T_I32, 6);
  xfer += oprot->writeI32(this->mipLevel);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("vol_type", ::apache::thrift::protocol::T_I32, 7);
  xfer += oprot->writeI32((int32_t)this->vol_type);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

}} // namespace
