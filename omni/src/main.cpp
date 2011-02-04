#include <zi/zargs/zargs.hpp>
ZiARG_bool(headless, false, "run Omni without GUI");
ZiARG_bool(tests, false, "run tests");
ZiARG_bool(segments, false, "run segments tests");
ZiARG_string(cmdfile, "", "run automated script file");
ZiARG_int64(psn, 0, "mac OSX proces ID");
ZiARG_bool(perf, false, "enable performance tests");
ZiARG_bool(noTilePrefetch, false, "disable tile prefetcher");
ZiARG_string(importHDF5seg, "", "create new Omni project file from data");
ZiARG_bool(noView3dThrottle, false, "disable View3d throttling");

#include <zi/logging.hpp>
USE_ZiLOGGING(STDOUT);
DEFINE_ZiLOG(memmap, false );
DEFINE_ZiLOG(mesh, false );
DEFINE_ZiLOG(hdf5verbose, false );
DEFINE_ZiLOG(io, false);
DEFINE_ZiLOG(segmentlist, false);
DEFINE_ZiLOG(tiles, false);
DEFINE_ZiLOG(tilesVerbose, false);
DEFINE_ZiLOG(threadpool, false);
DEFINE_ZiLOG(segmentSelector, false);

#include "gui/mainwindow.h"
#include "headless/headless.h"
#include "headless/headlessImpl.hpp"
#include "system/omStateManager.h"
#include "system/omQTApp.hpp"
#include "tests/tests.hpp"

#include <QApplication>
#include <QFileInfo>

class Omni{
private:
    int argc_;
    char **argv_;
    QString fileToOpen_;

public:
    Omni(int argc, char **argv)
        : argc_(argc), argv_(argv) {}

    int Run()
    {
        checkRemainingArgs();
        fileToOpen_ = getFileToOpen();

        if(ZiARG_importHDF5seg.size() > 0){
            return importHDF5seg();
        }

        if(ZiARG_tests){
            return runTests();
        }

        if(shouldRunHeadless()){
            return runHeadless();
        }

        return runGUI();
    }

private:
    bool shouldRunHeadless()
    {
#ifdef Q_WS_X11
        bool useGUI = getenv("DISPLAY") != 0;
#else
        bool useGUI = true;
#endif
        const bool headless = ZiARG_headless;

        if(!useGUI && !headless){
            printf("No GUI detected; Running headless....\n");
            return true;
        } else if(headless){
            return true;
        }

        return false;
    }

    int runTests()
    {
        Tests().Run();
        return 0;
    }

    int runHeadless()
    {
        const QString headlessCMD = QString::fromStdString(ZiARG_cmdfile);

        Headless h;
        h.runHeadless(headlessCMD, fileToOpen_);
        return 0;
    }

    int runGUI()
    {
        // leak QApplication to avoid "~QX11PixmapData(): QPixmap objects" error
        QApplication* app = new QApplication(argc_, argv_);
        Q_INIT_RESOURCE(resources);

        OmQTApp::SetAppFontSize();

        MainWindow mainWin;
        mainWin.show();

        if(fileToOpen_ != ""){
            mainWin.openProject(fileToOpen_);
        }

        return app->exec();
    }

    void checkRemainingArgs()
    {
        if( argc_ > 2){
            printf("too many arguments given:\n");
            for(int i = 1; i < argc_; ++i){
                printf("\t(%d) %s\n", i, argv_[i]);
            }
            exit(EXIT_FAILURE);
        }
    }

    QString getFileToOpen()
    {
        if(2 == argc_){
            return QString(argv_[1]);
        }

        return "";
    }

    int importHDF5seg()
    {
        HeadlessImpl::importHDF5seg(ZiARG_importHDF5seg);
        OmProject::Close();
        return 0;
    }
};

int main(int argc, char *argv[])
{
    zi::parse_arguments(argc, argv, true);

    return Omni(argc, argv).Run();
}
