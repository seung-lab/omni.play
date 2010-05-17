#include "project/omProject.h"
#include "volume/omVolume.h"

#include <QGroupBox>
#include <QLabel>
#include <QGridLayout>

class OmVolInspector : public QGroupBox {
	Q_OBJECT
public:
	OmVolInspector(OmVolume * vol, QWidget * parent=0);
	~OmVolInspector();
private:
        QGridLayout *mGrid;
	OmVolume * mVol;
};
