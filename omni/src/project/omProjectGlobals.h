#ifndef OM_PROJECT_GLOBALS_HPP
#define OM_PROJECT_GLOBALS_HPP

#include <boost/scoped_ptr.hpp>

class OmRandColorFile;
class OmView2dManagerImpl;
class OmStateManagerImpl;
class OmEventManagerImpl;
class OmActionsImpl;
class OmActionLogger;
template <class> class OmTilePool;

class OmProjectGlobals {
private:
    boost::scoped_ptr<OmRandColorFile> randColorFile_;
    boost::scoped_ptr<OmView2dManagerImpl> v2dManagerImpl_;
    boost::scoped_ptr<OmStateManagerImpl> stateMan_;
    boost::scoped_ptr<OmEventManagerImpl> eventMan_;
    boost::scoped_ptr<OmActionsImpl> actions_;
    boost::scoped_ptr<OmActionLogger> actionLogger_;

    boost::scoped_ptr<OmTilePool<uint8_t> > tilePoolUint8_;
    boost::scoped_ptr<OmTilePool<OmColorARGB> > tilePoolARGB_;

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

    template <typename T> OmTilePool<T>& TilePool(){
        return getTilePool(static_cast<T*>(0));
    }

private:
    inline OmTilePool<uint8_t>& getTilePool(uint8_t*){
        return *tilePoolUint8_;
    }

    inline OmTilePool<OmColorARGB>& getTilePool(OmColorARGB*){
        return *tilePoolARGB_;
    }
};

#endif
