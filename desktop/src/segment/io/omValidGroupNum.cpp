#include "volume/omSegmentation.h"
#include "segment/io/omValidGroupNum.hpp"
#include <QString>
#include "project/omProject.h"
#include "project/omProjectGlobals.h"
#include "users/omUsers.h"

QString OmValidGroupNum::filePathV1() const {
  return QString(OmProject::Paths().UserValidGroupNum(
      OmProject::Globals().Users().CurrentUser(), vol_.id()).c_str());
}
