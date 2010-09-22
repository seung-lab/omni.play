#ifndef OM_MANAGEABLE_OBJECT_H
#define OM_MANAGEABLE_OBJECT_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 3/3/09
 */

#include "common/omStd.h"
#include "common/omException.h"

class OmManageableObject {
public:
	OmManageableObject()
		: mId( 1 )
		, mNote("") {}

	explicit OmManageableObject(const OmId id)
		: mId( id )
		, mNote("") {}

	OmId GetId(){
		return mId;
	}

	QString GetCustomName(){
		return mCustomName;
	}
	void SetCustomName(const QString & name){
		mCustomName = name;
	}

	const QString& GetNote(){
		return mNote;
	}
	void SetNote(const QString &note){
		mNote = note;
	}

protected:
	OmId mId;
	QString mNote;
	QString mCustomName;

	friend class OmDataArchiveProject;
};

#endif
