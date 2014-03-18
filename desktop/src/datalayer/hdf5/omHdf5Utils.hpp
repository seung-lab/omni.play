#pragma once
#include "precomp.h"

#include "volume/omVolumeTypes.hpp"
#include "datalayer/omDataWrapper.h"

class OmDataPath;

class OmHdf5Utils {
 public:
  static bool group_exists(const int, const char*);
  static void group_delete(const int, const char*);
  static void group_create(const int, const char*);

  static bool dataset_exists(const int, const char*);
  static void dataset_delete(const int, const char*);
  static void dataset_delete_if_exists(const int, const char*);

  static OmDataWrapperPtr getNullDataWrapper(const int);
  static void printTypeInfo(const int);
  static int getSizeofType(const int);
  static OmDataWrapperPtr getDataWrapper(void*, const int,
                                         const om::OmDataAllocType);
};
