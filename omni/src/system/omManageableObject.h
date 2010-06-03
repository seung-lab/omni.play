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
	OmManageableObject();
	
	OmManageableObject(const OmId id);
		
	OmId GetId();
	
	const QString& GetName();
	void SetName(const QString &name);

	const QString& GetNote();
	void SetNote(const QString &note);

protected:
	OmId mId;
	QString mName;
	QString mNote;

	friend class OmDataArchiveProject;
};

#endif
