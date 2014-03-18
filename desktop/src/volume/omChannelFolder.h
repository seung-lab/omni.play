#pragma once
#include "precomp.h"

class OmChannelImpl;

namespace om {
namespace channel {

class folder {
 private:
  OmChannelImpl* const vol_;

 public:
  folder(OmChannelImpl* chan);

  QString GetVolPath() const;
  QString RelativeVolPath() const;
  QString MakeVolFolder() const;
};

}  // namespace channel
}  // namespace om
