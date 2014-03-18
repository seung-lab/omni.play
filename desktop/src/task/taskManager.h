#pragma once
#include "precomp.h"

#include "zi/utility.h"
#include "task/cell.h"
#include "task/taskInfo.hpp"
#include "task/dataset.h"
#include "events/listeners.h"
#include "network/http/httpCache.hpp"

namespace om {
namespace network {
class Uri;
}
namespace task {

class Task;

int confirmSaveAndClose();

class TaskManager : private om::SingletonBase<TaskManager>,
                    public om::event::ConnectionEventListener {
 public:
  static const std::shared_ptr<Task>& currentTask() {
    return instance().currentTask_;
  }
  // static std::vector<Task*> ListTasks();
  // static std::vector<Task*> ListTasks(int cellID);
  static std::shared_ptr<Task> GetTask(int cellID = 0);
  static std::shared_ptr<std::vector<TaskInfo>> GetTasks(int datasetID = 0,
                                                         int cellID = 0,
                                                         int maxWeight = 0);
  static std::shared_ptr<Task> GetTaskByID(int taskID);

  static std::shared_ptr<Task> GetComparisonTask(int cellID = 0);
  static std::shared_ptr<Task> GetComparisonTaskByID(int taskID);

  static std::shared_ptr<std::vector<Dataset>> GetDatasets();
  static std::shared_ptr<std::vector<Cell>> GetCells(int datasetID);
  static bool LoadTask(const std::shared_ptr<Task>& task);
  static bool AttemptFinishTask();
  static bool SubmitTask();
  static std::shared_ptr<Task> FindInterruptedTask();
  static void Refresh();

  void ConnectionChangeEvent();

 private:
  TaskManager() : currentTask_(nullptr) {}
  ~TaskManager();

  std::shared_ptr<Task> currentTask_;

  network::HTTPCache<Task> taskCache_;
  network::HTTPCache<std::vector<TaskInfo>> taskInfoCache_;
  network::HTTPCache<std::vector<Dataset>> datasetCache_;
  network::HTTPCache<std::vector<Cell>> cellsCache_;

  friend class zi::singleton<TaskManager>;
};

}  // namespace om::task::
}  // namespace om::
