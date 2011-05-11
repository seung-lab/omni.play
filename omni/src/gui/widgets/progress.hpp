#pragma once

#include "utility/omLockedPODs.hpp"

#include <QObject>

namespace om {
namespace gui {

class progress : public QObject {
Q_OBJECT

private:
    LockedUint32 numTotal_;
    LockedUint32 numDone_;

public:
    progress()
    {
        numTotal_.set(0);
        numDone_.set(0);
    }

    uint32_t GetTotal() const {
        return numTotal_.get();
    }

    void SetTotal(const uint32_t total){
        numTotal_.set(total);
    }

    uint32_t GetDone(){
        return numDone_.get();
    }

    void SetDone(const uint32_t numDid)
    {
        numDone_.add(numDid);
        progressChanged(numDone_.get(), numTotal_.get());
    }

Q_SIGNALS:
    void progressChanged(const uint32_t numDone, const uint32_t numTotal);
};

} // namespace gui
} // namespace om
