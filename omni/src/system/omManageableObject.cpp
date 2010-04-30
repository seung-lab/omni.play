#include "omManageableObject.h"

OmManageableObject::OmManageableObject() 
{ 
	mId = 0;
	mName = "";
	mNote = "";
}

OmManageableObject::OmManageableObject(OmId id)
{
	mId = id;
	mName = "";
	mNote = "";
}
		
OmId OmManageableObject::GetId() 
{ 
	return mId; 
}
	
const QString& OmManageableObject::GetName()
{ 
	return mName; 
}

void OmManageableObject::SetName(const QString &name) 
{ 
	mName = name; 
}

const QString& OmManageableObject::GetNote() 
{ 
	return mNote; 
}

void OmManageableObject::SetNote(const QString &note) 
{ 
	mNote = note; 
}
