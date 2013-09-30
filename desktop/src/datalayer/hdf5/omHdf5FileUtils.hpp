#pragma once

#include <string>
#include "hdf5.h"

class OmHdf5FileUtils {
 public:
  static void file_create(const std::string&);
  static hid_t file_open(std::string fpath, const bool readOnly);
  static void file_close(hid_t fileId);
  static void flush(const hid_t fileId);
};
