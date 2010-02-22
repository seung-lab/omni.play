#ifndef OM_PROJECT_H
#define OM_PROJECT_H

/*
 *	Manages data structures that are shared between various parts of the system.  Making centralized
 *	changes in the StateManager will send events that cause the other interested systems to be
 *	notified and synchronized.
 *
 *	Brett Warne - bwarne@mit.edu - 3/14/09
 */



#include "system/omPreferences.h"
#include "system/omStateManager.h"
#include "system/omKeyManager.h"
#include "system/omTagManager.h"
#include "volume/omVolume.h"

#include "common/omStd.h"
#include "common/omSerialization.h"

#include <vmmlib/vmmlib.h>
#include <vmmlib/serialization.h>
using namespace vmml;



typedef int (*GGOCTFPointer) (char *, int, int, int mousex, int mousey);


class OmProject {

public:
	
	static OmProject* Instance();
	static void Delete();
	
	//project properties
	static const string& GetFileName();
	static const string& GetDirectoryPath();
	static string GetTempDirectoryPath();
	
	//project IO
	static void New( QString fileNameAndPath );
	static void Save();
	static void Commit();
	static void Load( QString fileNameAndPath );
	static void Close();
	
	
protected:
	// singleton constructor, copy constructor, assignment operator protected
	OmProject();
	~OmProject();
	OmProject(const OmProject&);
	OmProject& operator= (const OmProject&);

	
private:
	//singleton
	static OmProject* mspInstance;
	
	//project
	string mFileName;
	string mDirectoryPath;

	
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int file_version);
};







#pragma mark 
#pragma mark Serialization
/////////////////////////////////
///////		 Serialization


BOOST_CLASS_VERSION(OmProject, 1)

template<class Archive>
void 
OmProject::serialize(Archive & ar, const unsigned int file_version) {

	ar & *OmPreferences::Instance();
	ar & *OmKeyManager::Instance();
	ar & *OmTagManager::Instance();
	ar & *OmVolume::Instance();

/*
	if (file_version > 0 ) {
		ar & *OmViewList::Instance();
	}
		//ar & *OmColormaps::Instance();
	ar & *OmGroups::Instance();
*/
}



#endif
