#include <zi/zargs/zargs.hpp>
ZiARG_int32(level, 0, "Detail level. 0: Cursory, 1: Detailed, 2: Thorough");
ZiARG_bool(chan, true, "Check Channels");
ZiARG_bool(seg, true, "Check Segmentations");
ZiARG_bool(mesh, true, "Check Meshes");
ZiARG_bool(cuvm, true, "Check Chunk Unique Values");
ZiARG_bool(segment, true, "Check Segment Data");
ZiARG_bool(mst, true, "Check mst");
ZiARG_string_list(only, "Check only these");
ZiARG_bool(verbose, false, "Verbose");
ZiARG_bool(fail, false, "Fail immediately on an error");
ZiARG_bool(sample, true, "Don't check everything, just sample.");

#include "common/common.h"
#include "datalayer/file.h"
#include "datalayer/paths.hpp"
#include "volume/volume.h"
#include "volume/segmentation.h"

#include "controller.hpp"
#include "volume.h"
#include "mesh.h"
#include "uniqueValues.h"

using namespace om;
using namespace om::valid;

int TestVolume(std::string path) {
  Controller::Reset();

  om::file::Paths p(path);
  if (!file::exists(p.FilesFolder())) {
    std::cout << "Bad Path." << std::endl;
    return 2;
  }

  if (ZiARG_only.size() > 0) {
    ZiARG_chan = std::find(ZiARG_only.begin(), ZiARG_only.end(), "chan") !=
                 ZiARG_only.end();
    ZiARG_seg = std::find(ZiARG_only.begin(), ZiARG_only.end(), "seg") !=
                ZiARG_only.end();
    ZiARG_mesh = std::find(ZiARG_only.begin(), ZiARG_only.end(), "mesh") !=
                 ZiARG_only.end();
    ZiARG_cuvm = std::find(ZiARG_only.begin(), ZiARG_only.end(), "cuvm") !=
                 ZiARG_only.end();
    ZiARG_segment = std::find(ZiARG_only.begin(), ZiARG_only.end(),
                              "segment") != ZiARG_only.end();
    ZiARG_mst = std::find(ZiARG_only.begin(), ZiARG_only.end(), "mst") !=
                ZiARG_only.end();
  }

  // Todo, extend to all Channels and Segmentations.
  if (ZiARG_chan) {
    Controller::Context c("Channel");
    volume::Volume chan(p.Channel(1));

    VolumeValid cv(chan, ZiARG_level);
    if (!cv.Check()) {
      std::cout << path << " failed." << std::endl;
      return 1;
    }
  }

  volume::Segmentation seg(p.Segmentation(1));

  if (ZiARG_seg) {
    Controller::Context c("Segmentation");

    VolumeValid cv(seg, ZiARG_level);
    if (!cv.Check()) {
      std::cout << path << " failed." << std::endl;
      return 1;
    }
  }

  if (ZiARG_cuvm) {
    Controller::Context c("UniqueValues");

    UniqueValuesValid uvv(seg, ZiARG_level);
    if (!uvv.Check()) {
      std::cout << path << " failed." << std::endl;
      return 1;
    }
  }

  if (ZiARG_mesh) {
    Controller::Context c("Meshes");

    MeshValid mv(seg.Endpoint(), seg.Coords(), ZiARG_level);
    if (!mv.Check()) {
      std::cout << path << " failed." << std::endl;
      return 1;
    }
  }

  if (Controller::HasFailed()) {
    std::cout << path << " failed." << std::endl;
  }

  return Controller::HasFailed();
}

int main(int argc, char *argv[]) {
  zi::parse_arguments(argc, argv, true);
  if (argc < 2) {
    std::cout << "Usage: omni.valid [Options] path..." << std::endl;
    return 1;
  }

  om::logging::initLogging("", false);

  if (ZiARG_only.size() > 0) {
    ZiARG_chan = std::find(ZiARG_only.begin(), ZiARG_only.end(), "chan") !=
                 ZiARG_only.end();
    ZiARG_seg = std::find(ZiARG_only.begin(), ZiARG_only.end(), "seg") !=
                ZiARG_only.end();
    ZiARG_mesh = std::find(ZiARG_only.begin(), ZiARG_only.end(), "mesh") !=
                 ZiARG_only.end();
    ZiARG_cuvm = std::find(ZiARG_only.begin(), ZiARG_only.end(), "cuvm") !=
                 ZiARG_only.end();
    ZiARG_segment = std::find(ZiARG_only.begin(), ZiARG_only.end(),
                              "segment") != ZiARG_only.end();
    ZiARG_mst = std::find(ZiARG_only.begin(), ZiARG_only.end(), "mst") !=
                ZiARG_only.end();
  }

  Controller::SetFail(ZiARG_fail);
  Controller::SetQuiet(!ZiARG_verbose);
  Controller::SetSample(ZiARG_sample);

  int retVal = 0;
  for (int i = 1; i < argc; i++) {
    auto err = TestVolume(argv[i]);
    if (err > 0) {
      retVal = err;
    }
  }

  return retVal;
}