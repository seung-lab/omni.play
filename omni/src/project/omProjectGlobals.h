#ifndef OM_PROJECT_GLOBALS_HPP
#define OM_PROJECT_GLOBALS_HPP

#include <boost/scoped_ptr.hpp>

class OmRandColorFile;
class OmView2dManagerImpl;
class OmStateManagerImpl;
class OmEventManagerImpl;
class OmActionsImpl;
class OmActionLogger;

class OmProjectGlobals {
private:
    boost::scoped_ptr<OmRandColorFile> randColorFile_;
    boost::scoped_ptr<OmView2dManagerImpl> v2dManagerImpl_;
    boost::scoped_ptr<OmStateManagerImpl> stateMan_;
    boost::scoped_ptr<OmEventManagerImpl> eventMan_;
    boost::scoped_ptr<OmActionsImpl> actions_;
    boost::scoped_ptr<OmActionLogger> actionLogger_;

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
};

#endif
