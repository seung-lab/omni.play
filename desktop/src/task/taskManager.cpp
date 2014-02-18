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

    return instance().scope_.GET<TracingTask>(taskURI);
  }
  catch (om::Exception e) {
    log_debugs << "Failed loading task: " << e.what();
  }
  return std::shared_ptr<Task>();
}

std::shared_ptr<Task> TaskManager::GetTaskByID(int taskID) {
  try {
    std::string taskURI = system::Account::endpoint() +
                          "/api/v1/task/cell/0/task/" + std::to_string(taskID);

    return network::ApplicationScope::GET<TracingTask>(taskURI);
  }
  catch (om::Exception e) {
    log_debugs << "Failed loading task: " << e.what();
  }
  return std::shared_ptr<Task>();
}

std::shared_ptr<Task> TaskManager::GetComparisonTask(int cellID) {
  try {
    std::string taskURI = system::Account::endpoint() + "/1.0/comparison_task";
    if (cellID) {
      taskURI += "?cell=" + std::to_string(cellID);
    }

    return instance().scope_.GET<ComparisonTask>(taskURI);
  }
  catch (om::Exception e) {
    log_debugs << "Failed loading task: " << e.what();
  }
  return std::shared_ptr<Task>();
}

std::shared_ptr<Task> TaskManager::GetComparisonTaskByID(int taskID) {
  if (!taskID) {
    return std::shared_ptr<Task>();
  }

  try {
    std::string taskURI = system::Account::endpoint() + "/1.0/comparison_task";
    taskURI += "?task=" + std::to_string(taskID);

    return network::ApplicationScope::GET<ComparisonTask>(taskURI);
  }
  catch (om::Exception e) {
    log_debugs << "Failed loading task: " << e.what();
  }
  return std::shared_ptr<Task>();
}

std::shared_ptr<Task> TaskManager::GetReapTask(int taskID) {
  try {
    std::string taskURI = system::Account::endpoint() +
                          "/api/v1/task/cell/0/task/" + std::to_string(taskID);

    auto task = network::HTTP::GET<ReapingTask>(taskURI);
    if (!task) {
      log_debugs << "No task... Bailing.";
      return task;
    }

    auto agg = network::HTTP::GET_JSON<Aggregate>(taskURI + "/aggregate");
    if (agg) {
      task->set_aggregate(std::move(*agg));
      log_debugs << "Setting Aggregate.";
    }
    return task;
  }
  catch (om::Exception e) {
    log_debugs << "Failed loading task: " << e.what();
  }
  return std::shared_ptr<Task>();
}

std::shared_ptr<Datasets> TaskManager::GetDatasets() {
  try {
    std::string datasetURI = system::Account::endpoint() + "/1.0/dataset";
    return network::ApplicationScope::GET<Datasets>(datasetURI);
  }
  catch (om::Exception e) {
    log_debugs << "Failed loading datasets: " << e.what();
  }
  return std::shared_ptr<Datasets>();
}

std::shared_ptr<Cells> TaskManager::GetCells(int datasetID) {
  try {
    std::string cellURI = system::Account::endpoint() + "/1.0/cell?dataset=" +
                          std::to_string(datasetID);
    return instance().scope_.GET<Cells>(cellURI);
  }
  catch (om::Exception e) {
    log_debugs << "Failed loading cells: " << e.what();
  }
  return std::shared_ptr<Cells>();
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
  instance().scope_.Refresh();
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
