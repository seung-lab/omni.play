#include "headless/headless.h"
#include "system/omStateManager.h"
#include "gui/mainwindow.h"

#include <zi/zargs/zargs.hpp>

#include <zi/logging.hpp>
USE_ZiLOGGING(DEFAULT);

#include <QApplication>
#include <QFileInfo>

ZiARG_bool(headless, false, "run Omni without GUI");
ZiARG_string(cmdFile, "", "run automated script file");
ZiARG_int64(psn, 0, "mac OSX proces ID");

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

		setOmniExecutablePath();

		if(shouldRunHeadless()){
			return runHeadless();
		}

		return runGUI();
	}

private:
	void setOmniExecutablePath()
	{
		const QString arg = QString(argv_[0]);
		QFileInfo fInfo(arg);
		const QString fnpn = fInfo.absoluteFilePath();
		OmStateManager::setOmniExecutableAbsolutePath(fnpn);
	}

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

	int runHeadless()
	{
		const QString headlessCMD = QString::fromStdString(ZiARG_cmdFile);

		Headless h;
		h.runHeadless(headlessCMD, fileToOpen_);
		return 0;
	}

	int runGUI()
	{
		boost::shared_ptr<QApplication> app(new QApplication(argc_, argv_));
		Q_INIT_RESOURCE(resources);
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
};

int main(int argc, char *argv[])
{
	zi::parse_arguments(argc, argv, true);

	return Omni(argc, argv).Run();
}
