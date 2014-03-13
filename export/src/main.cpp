#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "objExporter.hpp"
#include "rawExporter.hpp"
#include "mesh/vertexIndex/dataSource.h"
#include "chunk/uniqueValuesFileDataSource.hpp"
#include "volume/segmentation.h"
#include "common/logging.h"

#include "version.hpp"

using namespace om;
namespace po = boost::program_options;

class Options {
 public:
  Options() {
    MainOD.add_options()("mode", "Mode")("path", "Path");
    MainPD.add("mode", 1);
    MainPD.add("path", -1);

    HelpOD.add_options()("mode", "Mode")("helpMode", "Help for Mode")("none",
                                                                      "");
    HelpPD.add("mode", 1);
    HelpPD.add("helpMode", 2);
    HelpPD.add("none", -1);

    MeshOD.add_options()("resolution,r",
                         po::value<std::string>()->default_value("1,1,1"),
                         "A multiplicative resolution modifier")(
        "scale,s", po::value<float>()->default_value(1.0),
        "A scale factor for the resulting meshes")(
        "mip,m", po::value<uint8_t>()->default_value(0),
        "Mip level at which to export")("id", po::value<uint32_t>(),
                                        "Segment ID to export")(
        "obj,o", po::value<bool>()->default_value(true), "Export as obj");
  }

  po::options_description MainOD;
  po::options_description HelpOD;
  po::options_description MeshOD;
  po::options_description TileOD;
  po::options_description MstOD;

  po::positional_options_description MainPD;
  po::positional_options_description HelpPD;
};

int getHelp(const Options& opt, int argc, char* argv[]) {
  po::variables_map helpVM;
  po::store(po::command_line_parser(argc, argv)
                .options(opt.HelpOD)
                .positional(opt.HelpPD)
                .allow_unregistered()
                .run(),
            helpVM);
  // po::notify(helpVM);

  std::cerr << "Usage: " << argv[0] << " mode [options] path" << std::endl
            << "  Modes: help mesh tile mst" << std::endl;

  std::string mode;

  if (helpVM.count("helpMode")) {
    mode = helpVM["helpMode"].as<std::string>();
  } else if (helpVM.count("mode")) {
    mode = helpVM["mode"].as<std::string>();
  }

  if (mode == "mesh") {
    std::cerr << opt.MeshOD << std::endl;
  }

  if (mode == "tile") {
    std::cerr << opt.TileOD << std::endl;
  }

  if (mode == "mst") {
    std::cerr << opt.TileOD << std::endl;
  }
  return 1;
}

int exportMesh(const Options& opt, int argc, char* argv[]) {
  po::variables_map meshVM;
  try {
    po::options_description od;
    od.add(opt.MainOD).add(opt.MeshOD);
    po::store(po::command_line_parser(argc, argv)
                  .options(od)
                  .positional(opt.MainPD)
                  .run(),
              meshVM);
    po::notify(meshVM);
  }
  catch (std::exception e) {
    return getHelp(opt, argc, argv);
  }

  std::string path;
  if (meshVM.count("path")) {
    path = meshVM["path"].as<std::string>();
  } else {
    log_debugs << "Failed path";
    return getHelp(opt, argc, argv);
  }

  std::string resolutionStr;
  Vector3i resolution;
  if (meshVM.count("resolution")) {
    resolutionStr = meshVM["resolution"].as<std::string>();

    std::vector<std::string> strs;
    boost::split(strs, resolutionStr, boost::is_any_of(","));

    if (strs.size() != 3) {
      log_debugs << "Failed res size " << strs.size() << " : " << resolutionStr;
      return getHelp(opt, argc, argv);
    }

    int x = std::stoi(strs[0]);
    if (!x) {
      log_debugs << "Failed resolution x";
      return getHelp(opt, argc, argv);
    }
    int y = std::stoi(strs[1]);
    if (!y) {
      log_debugs << "Failed resolution y";
      return getHelp(opt, argc, argv);
    }
    int z = std::stoi(strs[2]);
    if (!z) {
      log_debugs << "Failed resolution z";
      return getHelp(opt, argc, argv);
    }
    resolution = Vector3i(x, y, z);
  } else {
    log_debugs << "Failed resolution";
    return getHelp(opt, argc, argv);
  }

  float scale;
  if (meshVM.count("scale")) {
    scale = meshVM["scale"].as<float>();
  } else {
    log_debugs << "Failed scale";
    return getHelp(opt, argc, argv);
  }

  uint8_t mip;
  if (meshVM.count("mip")) {
    mip = meshVM["mip"].as<uint8_t>();
  } else {
    log_debugs << "Failed mip";
    return getHelp(opt, argc, argv);
  }

  uint32_t id;
  if (meshVM.count("id")) {
    id = meshVM["id"].as<uint32_t>();
  } else {
    log_debugs << "Failed id";
    return getHelp(opt, argc, argv);
  }

  bool obj;
  if (meshVM.count("obj")) {
    obj = meshVM["obj"].as<bool>();
  } else {
    log_debugs << "Failed obj";
    return getHelp(opt, argc, argv);
  }

  file::Paths p(path);
  volume::Segmentation v(p, 1);

  auto coords = v.Coords();
  coords.SetResolution(coords.Resolution() * resolution);

  auto& meshDS = v.MeshDS();

  std::unique_ptr<exporter::IMeshExporter> exporter;
  if (obj) {
    exporter.reset(new exporter::Obj(coords, scale));
  } else {
    exporter.reset(new exporter::Raw());
  }

  auto chunks = coords.MipChunkCoords(mip);
  for (auto& chunk : *chunks) {
    auto mesh = meshDS.Get(coords::Mesh(chunk, id));
    if (mesh && mesh->HasData()) {
      exporter->Accumulate(
          mesh->Data().VertexData(), mesh->Data().VertexDataCount() * 6,
          mesh->Data().VertexIndex(), mesh->Data().VertexIndexCount(),
          mesh->Data().StripData(), mesh->Data().StripDataCount() * 2);
    }
  }

  log_infos << exporter->Write();

  return 0;
}

int exportTile(const Options& opt, int argc, char* argv[]) { return 0; }

int exportMST(const Options& opt, int argc, char* argv[]) {
  po::variables_map mstVM;
  try {
    po::options_description od;
    od.add(opt.MainOD).add(opt.MstOD);
    po::store(po::command_line_parser(argc, argv)
                  .options(od)
                  .positional(opt.MainPD)
                  .run(),
              mstVM);
    po::notify(mstVM);
  }
  catch (std::exception e) {
    return getHelp(opt, argc, argv);
  }

  std::string path;
  if (mstVM.count("path")) {
    path = mstVM["path"].as<std::string>();
  } else {
    log_debugs << "Failed path";
    return getHelp(opt, argc, argv);
  }

  file::Paths p(path);
  volume::Segmentation seg(p, 1);

  for (auto& e : seg.Edges()) {
    log_infos << e.number << "," << e.node1ID << "," << e.node2ID << ","
              << e.threshold;
  }

  return 0;
}

int main(int argc, char* argv[]) {
  logging::initLogging("", false);

  Options opt;

  po::variables_map mainVM;
  try {
    po::options_description od;
    po::store(po::command_line_parser(argc, argv)
                  .options(opt.MainOD)
                  .positional(opt.MainPD)
                  .allow_unregistered()
                  .run(),
              mainVM);
    po::notify(mainVM);
  }
  catch (std::exception e) {
    return getHelp(opt, argc, argv);
  }

  auto mode = mainVM["mode"].as<std::string>();

  log_variable(mode);
  if (mode == "help") {
    return getHelp(opt, argc, argv);
  }

  if (mode == "version") {
    std::cout << "omni.export version " << OMNI_EXPORT_VERSION << std::endl;
    return 0;
  }

  if (mode == "mesh") {
    return exportMesh(opt, argc, argv);
  }

  if (mode == "tile") {
    return exportTile(opt, argc, argv);
  }

  if (mode == "mst") {
    return exportMST(opt, argc, argv);
  }

  return getHelp(opt, argc, argv);
}