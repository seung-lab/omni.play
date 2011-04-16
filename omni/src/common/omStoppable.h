#ifndef OM_STOPPABLE_H
#define OM_STOPPABLE_H

namespace om {

struct stoppable {
    virtual void StoppableStop() = 0;
};

} // namespace om

#endif
