#pragma once

#include <boost/scoped_ptr.hpp>

#include <QString>

class OmSegmentation;

namespace om {

class usersImpl;

class users {
private:
    boost::scoped_ptr<usersImpl> impl_;

public:
    users();

    ~users();

    void SwitchToDefaultUser();
    QString GetVolSegmentsPathAbs(OmSegmentation* vol);
    QString LogFolderPath();
    void SetupFolders();
};

} // namespace om
