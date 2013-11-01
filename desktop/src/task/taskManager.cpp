#include <QtGui>

#include "taskManager.h"
#include "task/task.h"
#include "task/tracingTask.h"
#include "system/account.h"
#include "network/http/http.hpp"
#include "yaml-cpp/yaml.h"

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
  if (!taskID) {
    return std::shared_ptr<Task>();
  }
  return std::shared_ptr<Task>();
}

std::shared_ptr<Task> TaskManager::GetComparisonTask(int cellID) {
  return std::shared_ptr<Task>();
}

std::shared_ptr<std::vector<Cell>> TaskManager::GetCells() {
  try {
    std::string cellURI = system::Account::endpoint() + "/api/v1/task/cell";
    return network::HTTP::GET_JSON<std::vector<Cell>>(cellURI);
  }
  catch (om::Exception e) {
    log_debugs(Task) << "Failed loading cells: " << e.what();
  }
  return std::shared_ptr<std::vector<Cell>>();
}

bool TaskManager::LoadTask(const std::shared_ptr<Task>& task) {
  auto& current = instance().currentTask_;
  if (task == current) {
    return true;
  } else if (current) {
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
  if (!task) {
    return true;
  }

  if (!task->Start()) {
    current = nullptr;
    return false;
  }
  return true;
}

}  // namespace om::task::
}  // namespace om::
