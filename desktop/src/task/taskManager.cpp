#include <QtGui>

#include "taskManager.h"
#include "task/task.h"
#include "task/tracingTask.h"
#include "task/comparisonTask.h"
#include "task/reapingTask.h"
#include "task/aggregate.hpp"
#include "system/account.h"
#include "network/http/http.hpp"
#include "yaml-cpp/yaml.h"
#include "events/events.h"

namespace om {
namespace task {

TaskManager::~TaskManager() {}

template <typename T>
std::shared_ptr<Task> TaskManager::CachedGet(const std::string& uri) {
  auto& cache = instance().cache_;
  auto iter = cache.find(uri);
  if (iter != cache.end()) {
    return iter->second;
  }
  try {
    cache[uri] = network::HTTP::GET_JSON<T>(uri);
    return cache[uri];
  }
  catch (om::Exception e) {
    log_debugs << "Failed loading task: " << e.what();
  }
  return std::shared_ptr<Task>();
}

std::shared_ptr<Task> TaskManager::GetTask(int cellID) {
  std::string uri = system::Account::endpoint() + "/api/v1/task";
  if (cellID) {
    uri += "/cell/" + std::to_string(cellID);
  }
  return CachedGet<TracingTask>(uri);
}

std::shared_ptr<Task> TaskManager::GetTaskByID(int taskID) {
  std::string uri = system::Account::endpoint() + "/api/v1/task/cell/0/task/" +
                    std::to_string(taskID);
  if (taskID) {
    uri += "/cell/" + std::to_string(taskID);
  }
  return CachedGet<TracingTask>(uri);
}

std::shared_ptr<Task> TaskManager::GetComparisonTask(int cellID) {
  std::string uri = system::Account::endpoint() + "/1.0/comparison_task";
  if (cellID) {
    uri += "?cell=" + std::to_string(cellID);
  }
  return CachedGet<ComparisonTask>(uri);
}

std::shared_ptr<Task> TaskManager::GetComparisonTaskByID(int taskID) {
  if (!taskID) {
    return std::shared_ptr<Task>();
  }

  std::string uri = system::Account::endpoint() + "/1.0/comparison_task";
  uri += "?task=" + std::to_string(taskID);

  return CachedGet<ComparisonTask>(uri);
}

std::shared_ptr<Task> TaskManager::GetReapTask(int taskID) {
  std::string uri = system::Account::endpoint() + "/api/v1/task/cell/0/task/" +
                    std::to_string(taskID);

  auto& cache = instance().cache_;
  auto iter = cache.find(uri);
  if (iter != cache.end()) {
    return iter->second;
  }

  try {

    auto task = network::HTTP::GET_JSON<ReapingTask>(uri);
    if (!task) {
      log_debugs << "No task... Bailing.";
      return std::shared_ptr<Task>();
    }

    auto agg = network::HTTP::GET_JSON<Aggregate>(uri + "/aggregate");
    if (agg) {
      task->set_aggregate(std::move(*agg));
      log_debugs << "Setting Aggregate.";
    }

    cache[uri] = task;
    return cache[uri];
  }
  catch (om::Exception e) {
    log_debugs << "Failed loading task: " << e.what();
  }
  return std::shared_ptr<Task>();
}

std::shared_ptr<std::vector<Dataset>> TaskManager::GetDatasets() {
  try {
    std::string datasetURI = system::Account::endpoint() + "/1.0/dataset";
    return network::HTTP::GET_JSON<std::vector<Dataset>>(datasetURI);
  }
  catch (om::Exception e) {
    log_debugs << "Failed loading datasets: " << e.what();
  }
  return std::shared_ptr<std::vector<Dataset>>();
}

std::shared_ptr<std::vector<Cell>> TaskManager::GetCells(int datasetID) {
  try {
    std::string cellURI = system::Account::endpoint() + "/1.0/cell?dataset=" +
                          std::to_string(datasetID);
    return network::HTTP::GET_JSON<std::vector<Cell>>(cellURI);
  }
  catch (om::Exception e) {
    log_debugs << "Failed loading cells: " << e.what();
  }
  return std::shared_ptr<std::vector<Cell>>();
}

void TaskManager::ConnectionChangeEvent() {}

bool TaskManager::LoadTask(const std::shared_ptr<Task>& task) {
  auto& current = instance().currentTask_;
  if (task == current) {
    return true;
  } else if (current) {
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
  current = task;
  if (current) {
    log_debugs << "Changed current task " << current->Id();
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
    current = nullptr;
    return false;
  }
  om::event::TaskStarted();
  return true;
}

bool TaskManager::FinishTask() { return LoadTask(std::shared_ptr<Task>()); }

std::shared_ptr<Task> TaskManager::FindInterruptedTask() {
  return std::shared_ptr<Task>();
}

}  // namespace om::task::
}  // namespace om::
