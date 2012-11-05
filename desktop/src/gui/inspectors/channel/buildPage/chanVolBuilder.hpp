#pragma once

#include "volume/build/omBuildChannel.hpp"
#include "utility/channelDataWrapper.hpp"

namespace om {
namespace channelInspector {

class chanVolBuilder {
private:
    const ChannelDataWrapper cdw_;
    const QString whatOrHowToBuild_;
    const QFileInfoList fileNamesAndPaths_;

public:
    chanVolBuilder(const ChannelDataWrapper& cdw,
                  const QString& whatOrHowToBuild,
                  const QFileInfoList& fileNamesAndPaths)
        : cdw_(cdw)
        , whatOrHowToBuild_(whatOrHowToBuild)
        , fileNamesAndPaths_(fileNamesAndPaths)
    {}

    void Build()
    {
        OmBuildChannel* bc = new OmBuildChannel(cdw_);
        bc->setFileNamesAndPaths(fileNamesAndPaths_);
        bc->BuildNonBlocking();
    }

    void Build(boost::shared_ptr<om::gui::progress>)
    {
        OmBuildChannel* bc = new OmBuildChannel(cdw_);
        bc->setFileNamesAndPaths(fileNamesAndPaths_);
        bc->BuildNonBlocking();
    }
};

} // namespace segmentationInspector
} // namespace om
