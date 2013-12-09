#pragma once

#include "coordinates/dataCoord.h"
#include "coordinates/globalCoord.h"
#include "common/common.h"
#include "common/colors.h"
#include "datalayer/fs/omFile.hpp"
#include "system/manager.hpp"
#include "events/events.h"

#include <QDataStream>
#include <QFile>
#include <QVector>

class OmSegmentation;

namespace om {
namespace annotation {

struct data {
  dataCoord coord;
  std::string comment;
  om::common::Color color;
  double size;

  data(dataCoord coord, std::string comment, om::common::Color color,
       double size)
      : coord(coord), comment(comment), color(color), size(size) {}

  inline void setCoord(const om::globalCoord& c) {
    coord = c.toDataCoord(coord.volume(), 0);
  }
  inline om::globalCoord getCoord() { return coord.toGlobalCoord(); }
};

class manager : public system::Manager<data> {
 private:
  const OmSegmentation* vol_;
  typedef system::Manager<data> base_t;

 public:
  manager(OmSegmentation* vol) : vol_(vol) {}
  virtual ~manager() {}

  void Add(globalCoord coord, const std::string& comment,
           const om::common::Color& color, double size);

  void Load();
  void Save() const;

 protected:
  data* parse(const YAMLold::Node& n);
  std::string getFileName() { return filePathV1(); }

 private:
  std::string filePathV1() const;
};

}  // namespace annotation
}  // namespace om
