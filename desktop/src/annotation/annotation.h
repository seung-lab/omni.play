#pragma once

#include "common/common.h"
#include "coordinates/data.h"
#include "coordinates/global.h"
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
  coords::Data coord;
  std::string comment;
  common::Color color;
  double size;

  data(coords::Data coord, std::string comment, common::Color color,
       double size)
      : coord(coord), comment(comment), color(color), size(size) {}

  inline void setCoord(const coords::Global& c) {
    coord = c.ToData(coord.volume(), 0);
  }
  inline coords::Global getCoord() { return coord.ToGlobal(); }
};

class manager : public system::Manager<data> {
 private:
  const OmSegmentation* vol_;
  typedef system::Manager<data> base_t;

 public:
  manager(OmSegmentation* vol) : vol_(vol) {}
  virtual ~manager() {}

  void Add(coords::Global coord, const std::string& comment,
           const common::Color& color, double size);

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
