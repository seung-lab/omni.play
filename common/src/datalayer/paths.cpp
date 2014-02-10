#include "datalayer/paths.hpp"

namespace om {
namespace file {

Paths::Paths(const path& file) {
  auto absolute = om::file::absolute(file);

  if (!exists(absolute)) {
    throw ArgException(std::string("Invalid omni Path: ") + absolute.string());
  }

  if (absolute.string().back() == '/') {
    absolute = absolute.parent_path();
  }

  if (absolute.extension() == ".files") {
    filesFolder_ = absolute;
    omniFile_ = filesFolder_.parent_path() / filesFolder_.stem();
  } else if (absolute.extension() == ".omni") {
    omniFile_ = absolute;
    filesFolder_ = omniFile_;
    filesFolder_ += ".files";
  } else {
    throw ArgException(std::string("Invalid omni Path: ") + absolute.string());
  }

  filesFolder_ = omniFile_;
  filesFolder_ += ".files";
}

bool Paths::IsValid(const path& file) {
  auto absolute = om::file::absolute(file);
  return exists(absolute) &&
         (absolute.extension() == ".files" || absolute.extension() == ".omni");
}
}
}  // namespace om::file::