#pragma once

class channelImpl;



namespace om {
namespace channel {

class folder {
private:
    channelImpl *const vol_;

public:
    folder(channelImpl* chan);

    std::string GetVolPath() const;
    std::string RelativeVolPath() const;
    std::string MakeVolFolder() const;
};

} // namespace channel
} // namespace om
