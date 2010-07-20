#include "omManageableObject.h"

OmManageableObject::OmManageableObject() 
	: mId( 1 )
	, mName("")
	, mNote("")
{
}

OmManageableObject::OmManageableObject(const OmId id)
	: mId( id )
	, mName("")
	, mNote("")
{
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
