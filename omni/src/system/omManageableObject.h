#pragma once

/*
 *
 * Brett Warne - bwarne@mit.edu - 3/3/09
 */

#include "common/omStd.h"
#include "common/omException.h"

class OmManageableObject {
public:
    OmManageableObject()
        : id_( 1 )
    {}

    explicit OmManageableObject(const OmID id)
        : id_( id )
    {}

    inline OmID GetID() const {
        return id_;
    }

    inline const QString& GetCustomName() const {
        return customName_;
    }

    inline void SetCustomName(const QString & name){
        customName_ = name;
    }

    inline const QString& GetNote() const {
        return note_;
    }

    inline void SetNote(const QString &note){
        note_ = note;
    }

protected:
    OmID id_;
    QString note_;
    QString customName_;

    friend class OmMipVolumeArchive;
    friend class OmMipVolumeArchiveOld;
};

