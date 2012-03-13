#pragma once

namespace om {
namespace common {

struct stoppable {
    virtual void StoppableStop() = 0;
};

} // namespace common
} // namespace om

