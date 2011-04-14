#ifndef OM_PROJECT_GLOBALS_HPP
#define OM_PROJECT_GLOBALS_HPP

#include <zi/concurrency/semaphore.hpp>

#include <boost/scoped_ptr.hpp>

class OmActionLogger;
class OmActionsImpl;
class OmEventManagerImpl;
class OmRandColorFile;
class OmStateManagerImpl;
class OmTilePools;
class OmView2dManagerImpl;

class OmProjectGlobals {
private:
    boost::scoped_ptr<OmRandColorFile> randColorFile_;
    boost::scoped_ptr<OmView2dManagerImpl> v2dManagerImpl_;
    boost::scoped_ptr<OmStateManagerImpl> stateMan_;
    boost::scoped_ptr<OmEventManagerImpl> eventMan_;
    boost::scoped_ptr<OmActionsImpl> actions_;
    boost::scoped_ptr<OmActionLogger> actionLogger_;

    boost::scoped_ptr<OmTilePools> tilePools_;

    zi::semaphore fileReadThrottle_;

public:
    OmProjectGlobals();
    ~OmProjectGlobals();

    inline OmRandColorFile& RandColorFile(){
        return *randColorFile_;
    }

    inline OmView2dManagerImpl& View2dManagerImpl(){
        return *v2dManagerImpl_;
    }

    inline OmStateManagerImpl& StateManagerImpl(){
        return *stateMan_;
    }

    inline OmEventManagerImpl& EventManImpl(){
        return *eventMan_;
    }

    inline OmActionsImpl& Actions(){
        return *actions_;
    }

    inline OmActionLogger& ActionLogger(){
        return *actionLogger_;
    }

    inline OmTilePools& TilePools(){
        return *tilePools_;
    }

    inline zi::semaphore& FileReadSemaphore(){
        return fileReadThrottle_;
    }
};

#endif
