#pragma once

#include <memory>
#include "zi/utility.h"
#include "task/cell.h"
#include "task/dataset.h"
#include "events/listeners.h"
#include "network/http/httpScope.hpp"
#include "system/account.h"

namespace om {
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
  static std::shared_ptr<Task> GetTaskByID(int taskID);

  static std::shared_ptr<Task> GetComparisonTask(int cellID = 0);
  static std::shared_ptr<Task> GetComparisonTaskByID(int taskID);

  static std::shared_ptr<Task> GetReapTask(int taskID);
  static std::shared_ptr<Datasets> GetDatasets();
  static std::shared_ptr<Cells> GetCells(int datasetID);
  static bool LoadTask(const std::shared_ptr<Task>& task);
  static bool FinishTask();
  static std::shared_ptr<Task> FindInterruptedTask();

  void ConnectionChangeEvent();

 private:
  TaskManager() : currentTask_(nullptr), scope_(&system::Account::Scope()) {}
  ~TaskManager();

  std::shared_ptr<Task> currentTask_;
  network::HTTPScope scope_;

  friend class zi::singleton<TaskManager>;
};

}  // namespace om::task::
}  // namespace om::
