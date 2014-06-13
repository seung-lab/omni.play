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

TaskManager::TracingTaskRequest TaskManager::GetTask(int cellID) {
  auto uri = system::Account::endpoint();
  uri.set_path("/1.0/task/assign");
  if (cellID) {
    uri.AddQueryParameter("cell", std::to_string(cellID));
  }
  return network::http::GET<TracingTask>(uri);
}

TaskManager::TaskInfosRequest TaskManager::GetTasks(int datasetID, int cellID,
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
  return network::http::GET<std::vector<TaskInfo>>(uri);
}

TaskManager::TracingTaskRequest TaskManager::GetTaskByID(int taskID) {
  auto uri = system::Account::endpoint(std::string("/1.0/task/") +
                                       std::to_string(taskID));
  return network::http::GET<TracingTask>(uri);
}

TaskManager::ComparisonTaskRequest TaskManager::GetComparisonTask(int cellID) {
  auto uri = system::Account::endpoint("/1.0/comparison_task");
  if (cellID) {
    uri.AddQueryParameter("cell", std::to_string(cellID));
  }
  return network::http::GET<ComparisonTask>(uri);
}

TaskManager::ComparisonTaskRequest TaskManager::GetComparisonTaskByID(
    int taskID) {
  if (!taskID) {
    return TaskManager::ComparisonTaskRequest();
  }

  auto uri = system::Account::endpoint("/1.0/comparison_task");
  uri.AddQueryParameter("task", std::to_string(taskID));

  return network::http::GET<ComparisonTask>(uri);
}

TaskManager::DatasetsRequest TaskManager::GetDatasets() {
  try {
    auto datasetURI = system::Account::endpoint("/1.0/dataset");
    return network::http::GET<std::vector<Dataset>>(datasetURI);
  }
  catch (om::Exception e) {
    log_debugs << "Failed loading datasets: " << e.what();
  }
  return TaskManager::DatasetsRequest();
}

TaskManager::CellsRequest TaskManager::GetCells(int datasetID) {
  try {
    auto cellURI = system::Account::endpoint("/1.0/cell");
    cellURI.AddQueryParameter("dataset", std::to_string(datasetID));
    return network::http::GET<std::vector<Cell>>(cellURI);
  }
  catch (om::Exception e) {
    log_debugs << "Failed loading cells: " << e.what();
  }
  return TaskManager::CellsRequest();
}

std::shared_ptr<om::network::http::PutRequest> TaskManager::UpdateNotes(
    int taskID, std::string notes) {
  auto uri = system::Account::endpoint(std::string("/1.0/task/") +
                                       std::to_string(taskID) + "/notes");
  uri.set_port(88);
  return network::http::PUT(uri, notes);
}

void TaskManager::ConnectionChangeEvent() {}

bool TaskManager::LoadTask(std::shared_ptr<Task> task) {
  instance().currentTask_ = task;
  if (task) {
    log_infos << "Changed current task " << task->Id();
  } else {
    log_infos << "Changed current task nullptr";
  }
  om::event::TaskChange();
  if (!task) {
    return true;
  }

  if (!task->Start()) {
    log_debugs << "Failed starting task " << task->Id();
    instance().currentTask_ = nullptr;
    om::event::TaskChange();
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

void TaskManager::Refresh() {}

std::shared_ptr<Task> TaskManager::FindInterruptedTask() {
  return std::shared_ptr<Task>();
}

}  // namespace om::task::
}  // namespace om::
