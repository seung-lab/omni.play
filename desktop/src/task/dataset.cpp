#include "task/dataset.h"
#include "task/taskManager.h"

namespace om {
namespace task {
std::vector<Cell>* Dataset::cells() const { return cells_.get(); }
void Dataset::LoadCells() { cells_ = TaskManager::GetCells(id_); }
}
}  // namespace om::task::
