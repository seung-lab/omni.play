#include <QtGui>

#include "taskManager.h"
#include "task/task.h"
#include "task/taskInfo.hpp"
#include "task/tracingTask.h"
#include "task/comparisonTask.h"
#include "task/aggregate.hpp"
#include "system/account.h"
#include "network/http/http.hpp"
#include "network/uri.hpp"
#include "yaml-cpp/yaml.h"
#include "events/events.h"

namespace om {
namespace task {

TaskManager::~TaskManager() {}

template <typename T>
std::shared_ptr<Task> TaskManager::CachedGet(const network::Uri& uri) {
  auto& cache = instance().cache_;
  auto uriString = uri.string();
  auto iter = cache.find(uriString);
  if (iter != cache.end()) {
    return iter->second;
  }
  try {
    cache[uriString] = network::HTTP::GET_JSON<T>(uriString);
    return cache[uriString];
  }
  catch (om::Exception e) {
    log_debugs << "Failed loading task: " << e.what();
  }
  return std::shared_ptr<Task>();
}

std::shared_ptr<Task> TaskManager::GetTask(int cellID) {
  auto uri = system::Account::endpoint();
  uri.set_path("/api/v1/task");
  if (cellID) {
    uri.AppendPath(std::string("/cell/") + std::to_string(cellID));
  }
  return CachedGet<TracingTask>(uri);
}
std::shared_ptr<std::vector<TaskInfo>> TaskManager::GetTasks(int datasetID,
                                                             int cellID,
                                                             int maxWeight) {
  auto uri = system::Account::endpoint();
  uri.set_path("/1.0/task");
  if (cellID) {
    uri.AddQueryParameter("cell", std::to_string(cellID));
  } else if (datasetID) {
    uri.AddQueryParameter("dataset", std::to_string(datasetID));
  }
  if (maxWeight) {
    uri.AddQueryParameter("max_weight", std::to_string(maxWeight));
  }
  return network::HTTP::GET_JSON<std::vector<TaskInfo>>(uri);
}

std::shared_ptr<Task> TaskManager::GetTaskByID(int taskID) {
  auto uri = system::Account::endpoint(
      std::string("/api/v1/task/cell/0/task/") + std::to_string(taskID));
  if (taskID) {
    uri.AppendPath(std::string("cell/") + std::to_string(taskID));
  }
  return CachedGet<TracingTask>(uri);
}

std::shared_ptr<Task> TaskManager::GetComparisonTask(int cellID) {
  auto uri = system::Account::endpoint("/1.0/comparison_task");
  if (cellID) {
    uri.AddQueryParameter("cell", std::to_string(cellID));
  }
  return CachedGet<ComparisonTask>(uri);
}

std::shared_ptr<Task> TaskManager::GetComparisonTaskByID(int taskID) {
  if (!taskID) {
    return std::shared_ptr<Task>();
  }

  auto uri = system::Account::endpoint("/1.0/comparison_task");
  uri.AddQueryParameter("task", std::to_string(taskID));

  return CachedGet<ComparisonTask>(uri);
}

std::shared_ptr<std::vector<Dataset>> TaskManager::GetDatasets() {
  try {
    auto datasetURI = system::Account::endpoint("/1.0/dataset");
    return network::HTTP::GET_JSON<std::vector<Dataset>>(datasetURI);
  }
  catch (om::Exception e) {
    log_debugs << "Failed loading datasets: " << e.what();
  }
  return std::shared_ptr<std::vector<Dataset>>();
}

std::shared_ptr<std::vector<Cell>> TaskManager::GetCells(int datasetID) {
  try {
    auto cellURI = system::Account::endpoint("/1.0/cell");
    cellURI.AddQueryParameter("dataset", std::to_string(datasetID));
    return network::HTTP::GET_JSON<std::vector<Cell>>(cellURI);
  }
  catch (om::Exception e) {
    log_debugs << "Failed loading cells: " << e.what();
  }
  return std::shared_ptr<std::vector<Cell>>();
}

void TaskManager::ConnectionChangeEvent() {}

bool TaskManager::LoadTask(const std::shared_ptr<Task>& task) {
  instance().currentTask_ = task;
  if (task) {
    log_debugs << "Changed current task " << task->Id();
  } else {
    log_debugs << "Changed current task nullptr";
  }
  om::event::TaskChange();
  instance().cache_.clear();
  if (!task) {
    return true;
  }

  if (!task->Start()) {
    log_debugs << "Failed starting task " << task->Id();
    instance().currentTask_ = nullptr;
    return false;
  }
  om::event::TaskStarted();
  return true;
}

bool TaskManager::AttemptFinishTask() {
  auto& current = instance().currentTask_;
  if (current) {
    log_debugs << "Finishing current task " << current->Id();
    // TODO: headless
    QMessageBox box(
        QMessageBox::Question, "Submit current task?",
        "Would you like to submit your accomplishments on the current task?",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    int result = box.exec();
    if (result == QMessageBox::Cancel) {
      return false;
    }

    if (result == QMessageBox::Yes) {
      current->Submit();
    }
  }
  return LoadTask(nullptr);
}

bool TaskManager::SubmitTask() {
  auto& current = instance().currentTask_;
  if (!current) {
    return true;
  }
  log_debugs << "Finishing current task " << current->Id();
  current->Submit();
  return LoadTask(nullptr);
}

std::shared_ptr<Task> TaskManager::FindInterruptedTask() {
  return std::shared_ptr<Task>();
}

}  // namespace om::task::
}  // namespace om::
