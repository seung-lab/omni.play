#ifndef OM_HDF5_UTILS_HPP
#define OM_HDF5_UTILS_HPP

#include <string>
#include "hdf5.h"

class OmHdf5FileUtils {
public:
	static void file_create(std::string fpath);
	static hid_t file_open(std::string fpath, const bool readOnly);
	static void file_close(hid_t fileId);
	static void flush(const hid_t fileId);

private:
	static void printfDatasetCacheSize( const hid_t dataset_id );
	static void printfFileCacheSize( const hid_t fileId );
};

#endif
