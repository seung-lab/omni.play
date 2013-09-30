#pragma once

#include "actions/details/omUndoCommand.hpp"
#include "actions/io/omActionLogger.hpp"
#include "common/common.h"

template <typename IMPL> class OmActionBase : public OmUndoCommand {
 protected:
  std::shared_ptr<IMPL> impl_;

 public:
  OmActionBase() : impl_(std::make_shared<IMPL>()) {}

  OmActionBase(std::shared_ptr<IMPL> impl) : impl_(impl) {}

  virtual ~OmActionBase() {}

 protected:
  virtual void Action() { impl_->Execute(); }

  virtual void UndoAction() { impl_->Undo(); }

  virtual std::string Description() { return impl_->Description(); }

  virtual void save(const std::string& comment) {
    OmActionLogger::save(impl_, comment);
  }
};
