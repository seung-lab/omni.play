#pragma once

#include "common/std.h"
#include "common/exception.h"
#include "datalayer/archive/segmentation.h"
#include "datalayer/archive/filter.h"

namespace YAML { template<class T> class mipVolume; }

namespace om {
namespace common {

class manageableObject {
public:
    manageableObject()
        : id_( 1 )
    {}

    explicit manageableObject(const id id)
        : id_( id )
    {}

    inline id GetID() const {
        return id_;
    }

    inline const std::string& GetCustomName() const {
        return customName_;
    }

    inline void SetCustomName(const std::string & name){
        customName_ = name;
    }

    inline const std::string& GetNote() const {
        return note_;
    }

    inline void SetNote(const std::string &note){
        note_ = note;
    }

protected:
    id id_;
    std::string note_;
    std::string customName_;

    template <class T> friend class YAML::mipVolume;
};

} // namespace common
} // namespace om
