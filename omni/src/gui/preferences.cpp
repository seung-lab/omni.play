#include "preferences.h"
#include "volumeInspector.h"
#include "ui_preferences3d.h"
#include "preferences3d.h"
#include "preferences2d.h"
#include "preferencesMesh.h"

Preferences::Preferences(QWidget *parent) : QDialog(parent)
{
     tabs = new QTabWidget;
     tabs->addTab(new VolumeInspector(this), "Volume" );
     tabs->addTab(new Preferences2d(this), "2D View" );
     tabs->addTab(new Preferences3d(this), "3D View");
     tabs->addTab(new PreferencesMesh(this), "Mesh");

     
     QVBoxLayout *mainLayout = new QVBoxLayout;
     //     mainLayout->addWidget(buttonBox);
     mainLayout->addWidget(tabs);
     setLayout(mainLayout);
}
