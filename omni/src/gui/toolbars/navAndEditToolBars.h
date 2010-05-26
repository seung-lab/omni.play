#ifndef NAV_AND_EDIT_TOOLBARS_H
#define NAV_AND_EDIT_TOOLBARS_H

#include <QtGui>
#include "system/omStateManager.h"

class MainWindow;
class DendToolBar;
class OmViewGroupState;

class NavAndEditToolBars : public QWidget
{
	Q_OBJECT
		
 public:
	NavAndEditToolBars( MainWindow * mw );
	void setupToolbarInitially();
	void updateReadOnlyRelatedWidgets();
	void updateGuiFromProjectLoadOrOpen(OmViewGroupState * );
	void SystemModeChangeEvent();

 private slots:
	void ChangeModeModify(bool checked);
	void toolbarSelect(bool checked);
	void toolbarCrosshair(bool checked);
	void toolbarPan(bool checked);
	void toolbarZoom(bool checked);
	void toolbarBrush(bool checked);
	void toolbarEraser(bool checked);
	void toolbarFill(bool checked);
	void toolbarVoxelize(bool checked);
	void setFilAlpha(int alpha);
	void filterSegmentationChanged(int segId);
	void filterChannelChanged(int chanId);

 private:
	MainWindow * mMainWindow;

	void updateSilder();

	QToolBar *fileToolBar;
	QToolBar *systemToolBar;
	QToolBar *toolToolBar;
	QToolBar *navigateToolBar;
	QToolBar *editToolBar;
	QToolBar *viewToolBar;
	QToolBar *filterToolBar;

	// new toolbar
	void createToolbar();
	void createToolbarActions();
	void addToolbars();
	void setupSegmentationBoxes();

	QAction *toolbarView2D3DopenAct;
	QAction *saveToolbarAct;
	QAction *modifyAct;
	QAction *toolbarSelectAct;
	QAction *toolbarCrosshairAct;
	QAction *toolbarStickyCrosshairAct;
	QAction *toolbarPanAct;
	QAction *toolbarZoomAct;
	QAction *toolbarBrushAct;
	QAction *toolbarEraserAct;
	QAction *toolbarFillAct;
	QAction *toolbarVoxelizeAct;

	void setToolbarDisabled();
	void resetTool( QAction* tool, const bool enabled );
	void resetTools( const OmSystemMode sys_mode );
	void resetViewTools();
	void resetModifyTools( const bool enabled );
	void toolbarToolChange(const bool checked, QAction * tool, const OmToolMode mode );

	QSlider *alphaSlider;
	QComboBox *mSegmentationCombo;
	QComboBox *mChannelCombo;
	void setupFilterToolbar();
	void setFilterToolbarEnabled( bool setEnabled );

	DendToolBar * dendToolBar;
};

#endif
