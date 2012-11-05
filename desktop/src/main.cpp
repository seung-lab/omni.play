#include <zi/zargs/zargs.hpp>
ZiARG_bool(headless, false, "run Omni without GUI");
ZiARG_string(cmdfile, "", "run automated script file");
ZiARG_int64(psn, 0, "mac OSX proces ID");
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

#include "gui/mainWindow/mainWindow.h"
#include "headless/headless.h"
#include "headless/headlessImpl.hpp"
#include "system/omQTApp.hpp"
#include "system/omStateManager.h"
#ifndef ZI_OS_MACOS
#  include "client/linux/handler/exception_handler.h"
#  include "common/linux/google_crashdump_uploader.h"
using namespace google_breakpad;
#endif
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

class Omni{
private:
    int argc_;
    char **argv_;
    QString fileToOpen_;
#ifndef ZI_OS_MACOS
    ExceptionHandler eh_;
#endif

public:
    Omni(int argc, char **argv)
        : argc_(argc)
        , argv_(argv)
#ifndef ZI_OS_MACOS
        , eh_(MinidumpDescriptor("/tmp"), NULL, dumpCallback, NULL, true, -1)
#endif
    {}

    int Run()
    {
        checkRemainingArgs();

        fileToOpen_ = getFileToOpen();

        if(ZiARG_importHDF5seg.size() > 0){
            return importHDF5seg();
        }

        if(shouldRunHeadless()){
            return runHeadless();
        }

        return runGUI();
    }

private:
    bool shouldRunHeadless()
    {
// from QT docs
#ifdef Q_WS_X11
        bool useGUI = getenv("DISPLAY") != 0;
#else
        bool useGUI = true;
#endif
        const bool headless = ZiARG_headless;

        if(!useGUI && !headless)
        {
            printf("No GUI detected; Running headless....\n");
            return true;

        } else if(headless){
            return true;
        }

        return false;
    }

    void registerTypes()
    {
        qRegisterMetaType<om::tool::mode>("om::tool::mode");
        qRegisterMetaType<om::common::ID>("om::common::ID");
        qRegisterMetaType<ViewType>("ViewType");
        qRegisterMetaType<uint32_t>("uint32_t");
        qRegisterMetaType<ChannelDataWrapper>("ChannelDataWrapper");
        qRegisterMetaType<SegmentationDataWrapper>("SegmentationDataWrapper");
        qRegisterMetaType<std::string>("std::string");
    }

    int runHeadless()
    {
        const QString headlessCMD = QString::fromStdString(ZiARG_cmdfile);

        Headless h;
        h.RunHeadless(headlessCMD, fileToOpen_);
        return 0;
    }

    int runGUI()
    {
        // leak QApplication to avoid "~QX11PixmapData(): QPixmap objects" error
        QApplication* app = new QApplication(argc_, argv_, this);
        Q_INIT_RESOURCE(resources);
        registerTypes();

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
        if( argc_ > 2)
        {
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

#ifndef ZI_OS_MACOS
	static bool dumpCallback(const MinidumpDescriptor& descriptor,
                             void* context,
                             bool succeeded)
	{
		std::cout << "Omni has crashed.  Uploading error report " << descriptor.path() << std::endl;

		std::string cmd = std::string("curl -sF \"file=@") +
			descriptor.path() +
			"\" http://seungweb.mit.edu/omni/crashes/upload.php";

		if(!system(cmd.c_str())) {
			std::cout << std::endl << "Error report uploaded sucessfully!  This bug will be fixed!  Eventually....." << std::endl;
		} else {
			std::cout << std::endl << "Error report not uploaded!  Bug Matt...." << std::endl;
		}
		return succeeded;
	}
#endif
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

int main(int argc, char *argv[])
{
    zi::parse_arguments(argc, argv, true);

    return Omni(argc, argv).Run();
}
