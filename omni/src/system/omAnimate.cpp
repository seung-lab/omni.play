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

	} catch (...) {
		//debug ("animate", "Animation failed to run fully.\n");
	}
}

