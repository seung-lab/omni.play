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
class TracingTask;
class ComparisonTask;

int confirmSaveAndClose();

class TaskManager : private om::SingletonBase<TaskManager>,
                    public om::event::ConnectionEventListener {
 public:
  static const std::shared_ptr<Task>& currentTask() {
    return instance().currentTask_;
  }
  typedef std::shared_ptr<network::http::TypedGetRequest<TracingTask>>
      TracingTaskRequest;
  typedef std::shared_ptr<network::http::TypedGetRequest<ComparisonTask>>
      ComparisonTaskRequest;
  typedef std::shared_ptr<network::http::TypedGetRequest<std::vector<TaskInfo>>>
      TaskInfosRequest;
  typedef std::shared_ptr<network::http::TypedGetRequest<std::vector<Dataset>>>
      DatasetsRequest;
  typedef std::shared_ptr<network::http::TypedGetRequest<std::vector<Cell>>>
      CellsRequest;

  // static std::vector<Task*> ListTasks();
  // static std::vector<Task*> ListTasks(int cellID);
  static TracingTaskRequest GetTask(int cellID = 0);
  static TaskInfosRequest GetTasks(int datasetID = 0, int cellID = 0,
                                   int maxWeight = 0);
  static TracingTaskRequest GetTaskByID(int taskID);

  static ComparisonTaskRequest GetComparisonTask(int cellID = 0);
  static ComparisonTaskRequest GetComparisonTaskByID(int taskID);

  static DatasetsRequest GetDatasets();
  static CellsRequest GetCells(int datasetID);
  static std::shared_ptr<network::http::PutRequest> UpdateNotes(
      int taskID, std::string notes);

  static bool LoadTask(std::shared_ptr<Task> task);
  static bool AttemptFinishTask();
  static bool SubmitTask();
  static std::shared_ptr<Task> FindInterruptedTask();
  static void Refresh();

  void ConnectionChangeEvent();

 private:
  TaskManager() : currentTask_(nullptr) {}
  ~TaskManager();

  std::shared_ptr<Task> currentTask_;

  friend class zi::singleton<TaskManager>;
};

}  // namespace om::task::
}  // namespace om::
