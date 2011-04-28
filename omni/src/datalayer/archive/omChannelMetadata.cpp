#include "datalayer/archive/omChannelMetadata.h"

/**
 * Channel
 */

// QDataStream &operator<<(QDataStream& out, const OmChannelManager& cm)
// {
//     OmGenericManagerArchive::Save(out, cm.mChannelManager);
//     return out;
// }

// QDataStream &operator>>(QDataStream& in, OmChannelManager& cm)
// {
//     OmGenericManagerArchive::Load(in, cm.mChannelManager);
//     return in;
// }

// QDataStream& operator<<(QDataStream& out, const OmChannel& chan)
// {
//     OmMipVolumeArchive::Store(out, chan);

//     out << chan.filterManager_;

//     return out;
// }

// QDataStream& operator>>(QDataStream& in, OmChannel& chan)
// {
//     if(OmProject::GetFileVersion() < 25){
//         OmDataArchiveProjectImpl::LoadOldChannel(in, chan);

//     } else{
//         OmDataArchiveProjectImpl::LoadNewChannel(in, chan);
//     }

//     return in;
// }

// void OmChannelMetadata::LoadOldChannel(QDataStream& in, OmChannel& chan)
// {
//     OmMipVolumeArchiveOld::Load(in, chan, OmProject::GetFileVersion());

//     in >> chan.filterManager_;

//     if(OmProject::GetFileVersion() > 13)
//     {
//         bool dead;
//         in >> dead;

//         if(OmProject::GetFileVersion() < 24)
//         {
//             float dead;
//             in >> dead;
//             in >> dead;
//         }
//     }

//     if(OmProject::GetFileVersion() > 13){
//         chan.loadVolDataIfFoldersExist();
//     }
// }

// void OmChannelMetadata::LoadNewChannel(QDataStream& in, OmChannel& chan)
// {
//     OmMipVolumeArchive::Load(in, chan);

//     in >> chan.filterManager_;
//     chan.loadVolDataIfFoldersExist();
// }

// /**
//  * Filter
//  */

// QDataStream &operator<<(QDataStream& out, const OmFilter2dManager& fm)
// {
//     OmGenericManagerArchive::Save(out, fm.filters_);
//     return out;
// }

// QDataStream &operator>>(QDataStream& in, OmFilter2dManager& fm)
// {
//     OmGenericManagerArchive::Load(in, fm.filters_);
//     return in;
// }

// QDataStream &operator<<(QDataStream& out, const OmFilter2d& f)
// {
//     OmMipVolumeArchiveOld::StoreOmManageableObject(out, f);
//     out << f.alpha_;
//     out << f.chanID_;
//     out << f.segID_;

//     return out;
// }

// QDataStream &operator>>(QDataStream& in, OmFilter2d& f)
// {
//     OmMipVolumeArchiveOld::LoadOmManageableObject(in, f);
//     in >> f.alpha_;
//     in >> f.chanID_;
//     in >> f.segID_;

//     return in;
// }
