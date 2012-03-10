#pragma once

#include "common/omCommon.h"

class OmProjectCloseActionImpl {
public:
    OmProjectCloseActionImpl()
    {}

    void Execute()
    {}

    void Undo()
    {}

    std::string Description() const {
        return "Closed";
    }

    QString classNameForLogFile() const {
        return "OmProjectCloseAction";
    }

private:
    template <typename T> friend class OmActionLoggerThread;
};

