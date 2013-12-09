#include "datalayer/file.h"
#include "datalayer/compressedFile.h"

#include <fstream>
#include <iostream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

namespace om {
namespace file {

void uncompressFileToFile(const std::string& in_fnp,
                          const std::string& out_fnp) {

  std::ifstream in_file(in_fnp.c_str(), std::ios::in | std::ios::binary);

  boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
  in.push(boost::iostreams::gzip_decompressor());
  in.push(in_file);

  if (!exists(out_fnp)) {
    MkDir(path(out_fnp).parent_path());
  }
  std::ofstream out_file(out_fnp.c_str(),
                         std::ios::out | std::ios::trunc | std::ios::binary);

  boost::iostreams::copy(in, out_file);
}

void compressToFileNumBytes(char const* const data, const int64_t numBytes,
                            const std::string& out_fnp) {
  if (!exists(out_fnp)) {
    MkDir(path(out_fnp).parent_path());
  }
  std::ofstream out_file(out_fnp.c_str(),
                         std::ios::out | std::ios::trunc | std::ios::binary);

  boost::iostreams::filtering_streambuf<boost::iostreams::output> out;
  out.push(boost::iostreams::gzip_compressor());
  out.push(out_file);

  typedef boost::iostreams::basic_array_source<char> Device;
  boost::iostreams::stream_buffer<Device> input(data, numBytes);

  boost::iostreams::copy(input, out);
}
}
}  // namespace om::
