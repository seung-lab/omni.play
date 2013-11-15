#pragma once

namespace om {
namespace task {

class Task {
 public:
  virtual ~Task() {}
  virtual int Id() = 0;
  virtual int CellId() = 0;
  virtual bool Reaping() = 0;
  virtual bool Start() = 0;
  virtual bool Submit() = 0;
};

}  // namespace om::task::
}  // namespace om::
