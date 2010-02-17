/*
 * Author(s): Matt Wimer
 */
#include <qthread.h>
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"
#include "common/omDebug.h"

class OmAnimate: public QThread
{
public:
	OmAnimate (OmId seg);
	~OmAnimate ();

	void safeTerminate ();
        virtual void run();

private:
	OmId mSegmentation;
	bool mTerminate;
};
