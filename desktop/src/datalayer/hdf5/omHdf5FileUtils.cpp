#include "datalayer/hdf5/omHdf5FileUtils.hpp"
#include "common/omDebug.h"
#include "common/omException.h"
#include <QFile>

void OmHdf5FileUtils::file_create(const std::string& fpath) {
  //debug(hdf5verbose, "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

  QFile file(QString::fromStdString(fpath));
  if (!file.exists()) {
    hid_t fileId =
        H5Fcreate(fpath.c_str(), H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);
    if (fileId < 0) {
      const std::string errMsg = "Could not create HDF5 file: " + fpath + "\n";
      fprintf(stderr, "%s", errMsg.c_str());
      throw OmIoException(errMsg);
    }

    file_close(fileId);
  }
}

hid_t OmHdf5FileUtils::file_open(std::string fpath, const bool readOnly) {
  //debug(hdf5, "%s: opened HDF file\n", __FUNCTION__ );
  //debug(hdf5verbose, "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

  const unsigned int totalCacheSizeMB = 256;

  // number of elements (objects) in the raw data chunk cache (default 521)
  //  should be a prime number (due to simplistic hashing algorithm)
  size_t rdcc_nelmts = 2011;

  size_t rdcc_nbytes =
      totalCacheSizeMB * 1024 *
      1024;  // total size of the raw data chunk cache (default 1MB)
  double rdcc_w0 = 0.75;  // preemption policy (default 0.75)
  int mdc_nelmts = 0;     // no longer used

  hid_t fapl = H5Pcreate(H5P_FILE_ACCESS);  // defaults
  herr_t err =
      H5Pset_cache(fapl, mdc_nelmts, rdcc_nelmts, rdcc_nbytes, rdcc_w0);
  if (err < 0) {
    throw OmIoException("Could not setup HDF5 file cache.");
  }

  hid_t fileId;
  if (readOnly) {
    fileId = H5Fopen(fpath.c_str(), H5F_ACC_RDONLY, fapl);
  } else {
    fileId = H5Fopen(fpath.c_str(), H5F_ACC_RDWR, fapl);
  }

  if (fileId < 0) {
    const std::string errMsg = "Could not open HDF5 file: " + fpath + "\n";
    throw OmIoException(errMsg);
  }

  return fileId;
}

void OmHdf5FileUtils::flush(const hid_t fileId) {
  H5Fflush(fileId, H5F_SCOPE_GLOBAL);
}

void OmHdf5FileUtils::file_close(hid_t fileId) {
  flush(fileId);
  herr_t ret = H5Fclose(fileId);
  if (ret < 0) {
    throw OmIoException("Could not close HDF5 file.");
  }
}
