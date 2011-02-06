#include <assert.h>
#include "omProgressEvent.h"

bool OmProgressEvent::mWasCanceled = false;

OmProgressEvent::OmProgressEvent(QEvent::Type type)
	: OmEvent(type, CLASS)
{

}

OmProgressEvent::OmProgressEvent(QEvent::Type type, int value)
	: OmEvent(type, CLASS)
{
	mValue = value;
}

//PROGRESS_RANGE
OmProgressEvent::OmProgressEvent(QEvent::Type type, int min, int max, int value)
	: OmEvent(type, CLASS)
{
	mMinimum = min;
	mMaximum = max;
	mValue = value;
}

//PROGRESS_SHOW
OmProgressEvent::OmProgressEvent(QEvent::Type type, std::string text,
								 int min, int max)
	: OmEvent(type, CLASS)
{
	mText = text;
	mMinimum = min;
	mMaximum = max;
	mValue = 0;
}

/*
 *	Dispatch event based on event type.
 */

void OmProgressEvent::Dispatch(OmEventListener * pListener)
{

	//cast to proper listener
	OmProgressEventListener *p_cast_listener = dynamic_cast < OmProgressEventListener * >(pListener);
	assert(p_cast_listener);

	switch (type()) {

	case OmProgressEvent::PROGRESS_SHOW:
		p_cast_listener->ProgressShow();
		return;

	case OmProgressEvent::PROGRESS_HIDE:
		p_cast_listener->ProgressHide();
		return;

	case OmProgressEvent::PROGRESS_RANGE:
		p_cast_listener->ProgressRangeEvent();
		return;

	case OmProgressEvent::PROGRESS_VALUE:
		p_cast_listener->ProgressValueEvent();
		return;

	case OmProgressEvent::PROGRESS_INCREMENT:
		p_cast_listener->ProgressIncrementEvent();
		return;

	default:
		assert(false);
	}

}
