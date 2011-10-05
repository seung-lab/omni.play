#pragma once

class channelImpl;

#include <QString>

namespace om {
namespace channel {

class folder {
private:
    channelImpl *const vol_;

public:
    folder(channelImpl* chan);

    QString GetVolPath() const;
    QString RelativeVolPath() const;
    QString MakeVolFolder() const;
};

} // namespace channel
} // namespace om
