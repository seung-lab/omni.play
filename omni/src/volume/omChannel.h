#ifndef OM_CHANNEL_H
#define OM_CHANNEL_H

/*
 * OmChannel is the MIP data structure for a raw data volume
 *
 * Brett Warne - bwarne@mit.edu - 2/6/09
 */

#include "volume/omChannelImpl.h"

class OmChannel : public OmChannelImpl {

public:
    OmChannel();
    OmChannel(OmID id);
    virtual ~OmChannel();

    virtual QString GetDefaultHDF5DatasetName(){
        return "chanSingle";
    }

private:

    friend class OmBuildChannel;
    template <class T> friend class OmVolumeBuilder;
    template <class T> friend class OmVolumeBuilderHdf5;
    template <class T> friend class OmVolumeBuilderImages;
    template <class T> friend class OmVolumeImporter;

    friend QDataStream& operator<<(QDataStream& out, const OmChannel&);
    friend QDataStream& operator>>(QDataStream& in, OmChannel&);
};

#endif
