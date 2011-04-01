#ifndef OM_PROJECT_SAVE_IMPL_HPP
#define OM_PROJECT_SAVE_IMPL_HPP

#include "common/omCommon.h"
#include "project/omProject.h"

class OmProjectSaveActionImpl {
public:
    OmProjectSaveActionImpl()
    {}

    void Execute(){
        OmProject::Save();
    }

    void Undo()
    {}

    std::string Description() const {
        return "Saved";
    }

    QString classNameForLogFile() const {
        return "OmProjectSaveAction";
    }

private:
    template <typename T> friend class OmActionLoggerThread;
};

#endif
