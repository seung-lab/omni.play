#pragma once

#include <string>





namespace om {
namespace volume { class channelImpl; }
namespace channel {

class folder {
private:
    volume::channelImpl *const vol_;

public:
    folder(volume::channelImpl* chan);

    std::string GetVolPath() const;
    std::string RelativeVolPath() const;
    std::string MakeVolFolder() const;
};

} // namespace channel
} // namespace om
