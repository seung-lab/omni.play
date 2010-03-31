#include "preferences.h"
#include "volumeInspector.h"
#include "preferences3d.h"
#include "preferences2d.h"
#include "preferencesMesh.h"
#include "localPreferencesSystem.h"
#include "localPreferencesMeshing.h"
#include "localPreferences2d.h"
#include "localPreferences3d.h"

Preferences::Preferences(QWidget *parent) : QDialog(parent)
{
}

void Preferences::showLocalPreferences()
{
     tabs = new QTabWidget;
     tabs->addTab(new LocalPreferencesSystem(this), "System" );
     tabs->addTab(new LocalPreferences2d(this), "2D View" );
     tabs->addTab(new LocalPreferences3d(this), "3D View" );
     tabs->addTab(new LocalPreferencesMeshing(this), "Mesh" );

     closeButton = new QPushButton("&Close");
     connect(closeButton, SIGNAL(clicked()), this, SLOT(closeDialog()));
     
     QVBoxLayout *mainLayout = new QVBoxLayout();
     mainLayout->addWidget(closeButton);
     mainLayout->addWidget(tabs);
     setLayout(mainLayout);
     setWindowTitle(tr("Local Preferences"));
}

void Preferences::showProjectPreferences()
{
     tabs = new QTabWidget;
     tabs->addTab(new VolumeInspector(this), "Volume" );
     tabs->addTab(new Preferences2d(this), "2D View" );
     tabs->addTab(new Preferences3d(this), "3D View");
     tabs->addTab(new PreferencesMesh(this), "Mesh");

     closeButton = new QPushButton("&Close");
     connect(closeButton, SIGNAL(clicked()), this, SLOT(closeDialog()));
     
     QVBoxLayout *mainLayout = new QVBoxLayout();
     mainLayout->addWidget(closeButton);
     mainLayout->addWidget(tabs);
     setLayout(mainLayout);
     setWindowTitle(tr("Project Preferences"));
}

void Preferences::closeDialog()
{
	QDialog::done(0);
}
