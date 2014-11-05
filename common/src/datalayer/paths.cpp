#include "datalayer/paths.hpp"

namespace om {
namespace file {

path normalize(const path& file) {
  auto absolute = om::file::absolute(file);
  if (absolute.string().back() == '/') {
    absolute = absolute.parent_path();
  }
  return absolute;
}

Paths::Paths(const path& file, bool newFile) {
  auto normal = normalize(file);
  if (!exists(normal) && !newFile) {
    throw ArgException(std::string("Invalid omni Path: ") + normal.string());
  }

  if (normal.extension() == ".files") {
    filesFolder_ = normal;
    omniFile_ = filesFolder_.parent_path() / filesFolder_.stem();
  } else if (normal.extension() == ".omni") {
    omniFile_ = normal;
    filesFolder_ = omniFile_;
    filesFolder_ += ".files";
  } else {
    throw ArgException(std::string("Invalid omni extension: ") + normal.string());
  }

  filesFolder_ = omniFile_;
  filesFolder_ += ".files";
}

bool Paths::IsValid(const path& file) {
  auto normal = normalize(file);
  return exists(normal) &&
         (normal.extension() == ".files" || normal.extension() == ".omni");
}
}
}  // namespace om::file::
