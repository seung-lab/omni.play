#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QListWidget;
class QLabel;
class QMenu;
class QFrame;

// REPLACE THIS WITH YOUR IMPLEMENTATION
class OmView2d;

class QGLWidget;

class MainWindow : public QMainWindow
	{
		Q_OBJECT
		
	public:
		MainWindow();
		
		private slots:
		void open();
		void about();
		
	private:
		void createActions();
		void createMenus();
		void createToolBars();
		void createStatusBar();
		void createDockWindows();
		
		QGLWidget *imageGLWidget;
		
		// REPLACE THIS WITH YOUR IMPLEMENTATION
		OmView2d *qtView2d_xy;
		OmView2d *qtView2d_xz;
		OmView2d *qtView2d_yz;
		
		OmView2d *qtView2d_xy_s;
		OmView2d *qtView2d_xz_s;
		OmView2d *qtView2d_yz_s;

		
		
		QFrame *loadingDock;
		
		QMenu *fileMenu;
		QMenu *editMenu;
		QMenu *viewMenu;
		QMenu *helpMenu;
		QToolBar *fileToolBar;
		QToolBar *editToolBar;
		QAction *openAct;
		QAction *aboutAct;
		QAction *aboutQtAct;
		QAction *quitAct;
	};

#endif
