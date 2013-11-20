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

std::shared_ptr<Task> TaskManager::GetTask(int cellID) {
  try {
    std::string taskURI = system::Account::endpoint() + "/api/v1/task";
    if (cellID) {
      taskURI += "/cell/" + std::to_string(cellID);
    }

    return network::HTTP::GET_JSON<TracingTask>(taskURI);
  }
  catch (om::Exception e) {
    log_debugs(Task) << "Failed loading task: " << e.what();
  }
  return std::shared_ptr<Task>();
}

std::shared_ptr<Task> TaskManager::GetTaskByID(int taskID) {
  try {
    std::string taskURI = system::Account::endpoint() +
                          "/api/v1/task/cell/0/task/" + std::to_string(taskID);

    return network::HTTP::GET_JSON<TracingTask>(taskURI);
  }
  catch (om::Exception e) {
    log_debugs(Task) << "Failed loading task: " << e.what();
  }
  return std::shared_ptr<Task>();
}

std::shared_ptr<Task> TaskManager::GetComparisonTask(int cellID) {
  try {
    std::string taskURI = system::Account::endpoint() + "/1.0/comparison_task";
    if (cellID) {
      taskURI += "?cell=" + std::to_string(cellID);
    }

    return network::HTTP::GET_JSON<ComparisonTask>(taskURI);
  }
  catch (om::Exception e) {
    log_debugs(Task) << "Failed loading task: " << e.what();
  }
  return std::shared_ptr<Task>();
}

std::shared_ptr<Task> TaskManager::GetComparisonTaskByID(int taskID) {
  return std::shared_ptr<Task>();
}

std::shared_ptr<Task> TaskManager::GetReapTask(int taskID) {
  try {
    std::string taskURI = system::Account::endpoint() +
                          "/api/v1/task/cell/0/task/" + std::to_string(taskID);

    auto task = network::HTTP::GET_JSON<ReapingTask>(taskURI);
    if (!task) {
      log_debugs(Task) << "No task... Bailing.";
      return task;
    }

    auto agg = network::HTTP::GET_JSON<Aggregate>(taskURI + "/aggregate");
    if (agg) {
      task->set_aggregate(std::move(*agg));
      log_debugs(Task) << "Setting Aggregate.";
    }
    return task;
  }
  catch (om::Exception e) {
    log_debugs(Task) << "Failed loading task: " << e.what();
  }
  return std::shared_ptr<Task>();
}

std::shared_ptr<std::vector<Dataset>> TaskManager::GetDatasets() {
  try {
    std::string datasetURI = system::Account::endpoint() + "/1.0/dataset";
    return network::HTTP::GET_JSON<std::vector<Dataset>>(datasetURI);
  }
  catch (om::Exception e) {
    log_debugs(Task) << "Failed loading datasets: " << e.what();
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
    log_debugs(Task) << "Failed loading cells: " << e.what();
  }
  return std::shared_ptr<std::vector<Cell>>();
}

void TaskManager::ConnectionChangeEvent() {}

bool TaskManager::LoadTask(const std::shared_ptr<Task>& task) {
  auto& current = instance().currentTask_;
  if (task == current) {
    return true;
  } else if (current) {
    log_debugs(Task) << "Finishing current task " << current->Id();
    // TODO: headless
    QMessageBox box(
        QMessageBox::Question, "Save current task?",
        "Would you like to save your accomplishments on the current task?",
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
    log_debugs(Task) << "Changed current task " << current->Id();
  } else {
    log_debugs(Task) << "Changed current task nullptr";
  }
  om::event::TaskChange();
  if (!task) {
    return true;
  }

  if (!task->Start()) {
    log_debugs(Task) << "Failed starting task " << task->Id();
    current = nullptr;
    return false;
  }
  return true;
}

bool TaskManager::FinishTask() { return LoadTask(std::shared_ptr<Task>()); }

}  // namespace om::task::
}  // namespace om::
