#pragma once

/*
 * OmChannel is the MIP data structure for a raw data volume
 *
 * Brett Warne - bwarne@mit.edu - 2/6/09
 */

#include "volume/omChannelImpl.h"
#include "datalayer/archive/channel.h"

class OmChannel : public OmChannelImpl {

 public:
  OmChannel();
  OmChannel(om::common::ID id);
  virtual ~OmChannel();

  virtual QString GetDefaultHDF5DatasetName() { return "chanSingle"; }

 private:

  friend class OmBuildChannel;
  template <class T> friend class OmVolumeBuilder;
  template <class T> friend class OmVolumeBuilderHdf5;
  template <class T> friend class OmVolumeBuilderImages;
  template <class T> friend class OmVolumeImporter;

  friend QDataStream& operator<<(QDataStream& out, const OmChannel&);
  friend QDataStream& operator>>(QDataStream& in, OmChannel&);
  friend YAMLold::Emitter& YAMLold::operator<<(YAMLold::Emitter& out,
                                               const OmChannel& chan);
  friend void YAMLold::operator>>(const YAMLold::Node& in, OmChannel& chan);
};
