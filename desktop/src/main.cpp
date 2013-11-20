#include <zi/zargs/zargs.hpp>
ZiARG_bool(headless, false, "run Omni without GUI");
ZiARG_string(cmdfile, "", "run automated script file");
ZiARG_int64(psn, 0, "mac OSX proces ID");
ZiARG_bool(noTilePrefetch, false, "disable tile prefetcher");
ZiARG_bool(noView3dThrottle, false, "disable View3d throttling");

#include "common/logging.h"

#include "common/common.h"
#include "gui/tools.hpp"
#include "gui/mainWindow/mainWindow.h"
#include "utility/channelDataWrapper.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "headless/headless.h"
#include "system/omQTApp.hpp"

#include <QFileInfo>
#include <QApplication>

class Omni {
 private:
  int argc_;
  char **argv_;
  QString fileToOpen_;

 public:
  Omni(int argc, char **argv) : argc_(argc), argv_(argv) {
    om::logging::initLogging("~/.omni/omni.log");
  }

  int Run() {
    checkRemainingArgs();

    fileToOpen_ = getFileToOpen();

    if (shouldRunHeadless()) {
      return runHeadless();
    }

    return runGUI();
  }

 private:
  bool shouldRunHeadless() {
// from QT docs
#ifdef Q_WS_X11
    bool useGUI = getenv("DISPLAY") != 0;
#else
    bool useGUI = true;
#endif
    const bool headless = ZiARG_headless;

    if (!useGUI && !headless) {
      log_infos(unknown) << "No GUI detected; Running headless....";
      return true;

    } else if (headless) {
      return true;
    }

    return false;
  }

  void registerTypes() {
    qRegisterMetaType<om::tool::mode>("om::tool::mode");
    qRegisterMetaType<om::common::ID>("om::common::ID");
    qRegisterMetaType<om::common::ViewType>("om::common::ViewType");
    qRegisterMetaType<uint32_t>("uint32_t");
    qRegisterMetaType<ChannelDataWrapper>("ChannelDataWrapper");
    qRegisterMetaType<SegmentationDataWrapper>("SegmentationDataWrapper");
    qRegisterMetaType<std::string>("std::string");
  }

  int runHeadless() {
    Headless h;
    h.RunHeadless(QString::fromStdString(ZiARG_cmdfile), fileToOpen_);
    return 0;
  }

  int runGUI() {
    // leak QApplication to avoid "~QX11PixmapData(): QPixmap objects" error
    auto *app = new QApplication(argc_, argv_, this);
    Q_INIT_RESOURCE(resources);
    registerTypes();

    OmQTApp::SetAppFontSize();

    MainWindow mainWin;
    mainWin.show();

    if (fileToOpen_ != "") {
      mainWin.openProject(fileToOpen_.toStdString(), "");
    }

    return app->exec();
  }

  void checkRemainingArgs() {
    if (argc_ > 2) {
      log_errors(unknown) << "too many arguments given:";
      for (auto i = 1; i < argc_; ++i) {
        log_debug(unknown, "\t(%d) %s", i, argv_[i]);
      }
      exit(EXIT_FAILURE);
    }
  }

  QString getFileToOpen() {
    if (2 == argc_) {
      return QString(argv_[1]);
    }
    return "";
  }
};

int main(int argc, char *argv[]) {
  zi::parse_arguments(argc, argv, true);
  return Omni(argc, argv).Run();
}
