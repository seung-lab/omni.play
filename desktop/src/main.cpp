#include <zi/zargs/zargs.hpp>
ZiARG_bool(headless, false, "run Omni without GUI");
ZiARG_string(cmdfile, "", "run automated script file");
ZiARG_int64(psn, 0, "mac OSX proces ID");
ZiARG_bool(noTilePrefetch, false, "disable tile prefetcher");
ZiARG_string(importHDF5seg, "", "create new Omni project file from data");
ZiARG_bool(noView3dThrottle, false, "disable View3d throttling");

#include <zi/logging.hpp>
USE_ZiLOGGING(STDOUT);
DEFINE_ZiLOG(memmap, false);
DEFINE_ZiLOG(mesh, false);
DEFINE_ZiLOG(hdf5verbose, false);
DEFINE_ZiLOG(io, false);
DEFINE_ZiLOG(segmentlist, false);
DEFINE_ZiLOG(tiles, false);
DEFINE_ZiLOG(tilesVerbose, false);
DEFINE_ZiLOG(threadpool, false);
DEFINE_ZiLOG(segmentSelector, false);

#include "gui/mainWindow/mainWindow.h"
#include "headless/headless.h"
#include "headless/headlessImpl.hpp"
#include "system/omQTApp.hpp"
#include "system/omStateManager.h"
#include <QFileInfo>
#include <QApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <boost/filesystem.hpp>

class Omni;

// class MyApplication : public QApplication
// {
// private:
// 	Omni* o_;
// public:
// 	MyApplication(int argc, char **argv, Omni* o)
// 		: QApplication(argc, argv)
// 		, o_(o)
// 	{}

// 	bool notify ( QObject * receiver, QEvent * e );
// };

class Omni {
 private:
  int argc_;
  char **argv_;
  QString fileToOpen_;

 public:
  Omni(int argc, char **argv) : argc_(argc), argv_(argv) {}

  int Run() {
    checkRemainingArgs();

    fileToOpen_ = getFileToOpen();

    if (ZiARG_importHDF5seg.size() > 0) {
      return importHDF5seg();
    }

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
      printf("No GUI detected; Running headless....\n");
      return true;

    } else if (headless) {
      return true;
    }

    return false;
  }

  void registerTypes() {
    qRegisterMetaType<om::tool::mode>("om::tool::mode");
    qRegisterMetaType<OmID>("OmID");
    qRegisterMetaType<ViewType>("ViewType");
    qRegisterMetaType<uint32_t>("uint32_t");
    qRegisterMetaType<ChannelDataWrapper>("ChannelDataWrapper");
    qRegisterMetaType<SegmentationDataWrapper>("SegmentationDataWrapper");
    qRegisterMetaType<std::string>("std::string");
  }

  int runHeadless() {
    const QString headlessCMD = QString::fromStdString(ZiARG_cmdfile);

    Headless h;
    h.RunHeadless(headlessCMD, fileToOpen_);
    return 0;
  }

  int runGUI() {
    // leak QApplication to avoid "~QX11PixmapData(): QPixmap objects" error
    QApplication *app = new QApplication(argc_, argv_, this);
    Q_INIT_RESOURCE(resources);
    registerTypes();

    OmQTApp::SetAppFontSize();

    MainWindow mainWin;
    mainWin.show();

    if (fileToOpen_ != "") {
      mainWin.openProject(fileToOpen_);
    }

    return app->exec();
  }

  void checkRemainingArgs() {
    if (argc_ > 2) {
      printf("too many arguments given:\n");
      for (int i = 1; i < argc_; ++i) {
        printf("\t(%d) %s\n", i, argv_[i]);
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

  int importHDF5seg() {
    HeadlessImpl::importHDF5seg(ZiARG_importHDF5seg);
    OmProject::Close();
    return 0;
  }
};

// bool MyApplication::notify ( QObject * receiver, QEvent * e )
// {
// 	try {
// 		QApplication::notify(receiver, e);
// 	} catch (OmException e) {
// 		o_->eh().WriteMinidump();
// 		throw;
// 	}
// }

int main(int argc, char *argv[]) {
  zi::parse_arguments(argc, argv, true);

  return Omni(argc, argv).Run();
}
