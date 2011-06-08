#pragma once

#include "actions/io/omActionLoggerTask.hpp"
#include "actions/omActions.h"
#include "datalayer/fs/omFileNames.hpp"
#include "project/omProject.h"
#include "project/omProjectGlobals.h"
#include "threads/omTaskManager.hpp"
#include "threads/omTaskManagerTypes.h"
#include "utility/omFileHelpers.h"
#include "zi/omMutex.h"
#include "zi/omUtility.h"

#include <QDir>

class OmActionLogger : private om::singletonBase<OmActionLogger> {
private:
    static inline OmActionLogger& impl(){
        return OmProject::Globals().ActionLogger();
    }

public:
    ~OmActionLogger(){
        threadPool_.join();
    }

    inline static const QDir& LogFolder(){
        return impl().logFolder_;
    }

    template <typename T>
    static void save(om::shared_ptr<T> actionImpl,
                     const std::string& str)
    {
        om::shared_ptr<OmActionLoggerTask<T> >
            task(new OmActionLoggerTask<T>(actionImpl,
                                           str,
                                           impl().logFolder_));

        impl().threadPool_.push_back(task);
    }

private:
    QDir logFolder_;
    OmThreadPool threadPool_;

    OmActionLogger()
    {
        setupLogDir();
        threadPool_.start(1);
    }

    void setupLogDir()
    {
        const QString logFolderPath = OmFileNames::LogFolderPath();

        logFolder_ = QDir(logFolderPath);

        if(logFolder_.exists()){
            return;
        }

        OmFileHelpers::MkDir(logFolderPath);
    }

    friend class OmProjectGlobals;
    friend class zi::singleton<OmActionLogger>;
};

