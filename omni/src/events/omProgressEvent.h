#ifndef OM_PROGRESS_EVENT_H
#define OM_PROGRESS_EVENT_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 3/14/09
 */


#include <common/omStd.h>

#include "events/omEvent.h"

class OmProgressEvent : public OmEvent {

public:
	OmProgressEvent(QEvent::Type type);
	OmProgressEvent(QEvent::Type type, int val);
	OmProgressEvent(QEvent::Type type, std::string title, int min = 0, int max = 1);
	OmProgressEvent(QEvent::Type type, int min, int max, int val = 0);
	void Dispatch(OmEventListener *);

	//class
	static const OmEventClass CLASS  = OM_PROGRESS_EVENT_CLASS;
	//events
	static const QEvent::Type PROGRESS_SHOW = (QEvent::Type) (CLASS);
	static const QEvent::Type PROGRESS_HIDE = (QEvent::Type) (CLASS+1);
	static const QEvent::Type PROGRESS_RANGE = (QEvent::Type) (CLASS+2);
	static const QEvent::Type PROGRESS_VALUE = (QEvent::Type) (CLASS+3);
	static const QEvent::Type PROGRESS_INCREMENT = (QEvent::Type) (CLASS+4);

	std::string GetText() { return mText; }
	int GetMinimum() { return mMinimum; }
	int GetMaximum() { return mMaximum; }
	int GetValue() { return mValue; }

	static void SetWasCanceled(bool state) { mWasCanceled = state; }
	static bool GetWasCanceled() { return mWasCanceled; }

private:
	std::string mText;
	int mMinimum, mMaximum;
	int mValue;
	static bool mWasCanceled;
};



class OmProgressEventListener : public OmEventListener {

public:
	OmProgressEventListener() : OmEventListener(OmProgressEvent::CLASS) { };

	virtual void ProgressShow() = 0;
	virtual void ProgressHide() = 0;

	virtual void ProgressRangeEvent() = 0;
	virtual void ProgressValueEvent() = 0;
	virtual void ProgressIncrementEvent() = 0;

};



#endif
