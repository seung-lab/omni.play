#include "project/omProject.h"
#include "volume/omVolume.h"

#include <QGroupBox>
#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>

class OmVolInspector : public QGroupBox {
 Q_OBJECT
 public:
	OmVolInspector(OmVolume * vol, QWidget * parent);
	~OmVolInspector();

 private slots:
	void apply();

 private:
        QGridLayout *mGrid;
	OmVolume * mVol;
	QLineEdit * mResX;
	QLineEdit * mResY;
	QLineEdit * mResZ;
};
