/* The animation skeleton.
 *
 * Author(s): Matt Wimer
 */
#include "omAnimate.h"
#include "system/events/omView3dEvent.h"

OmAnimate::OmAnimate (OmId seg): mSegmentation (seg)
{
	mTerminate = false;
}

OmAnimate::~OmAnimate()
{
}

void OmAnimate::safeTerminate ()
{
	mTerminate = true;
}

void OmAnimate::run ()
{
	try {
		OmSegmentation & seg = OmVolume::GetSegmentation (mSegmentation);

		foreach(OmId segID, seg.GetValidSegmentIds()) {

			if (mTerminate) {
				return;
			}

			seg.SetSegmentSelected (segID, true);

			sleep (1);
			seg.SetAllSegmentsSelected (false);

			OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));

		}

	} catch (...) {
		debug ("animate", "Animation failed to run fully.\n");
	}
}

