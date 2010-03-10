#ifndef OM_KEY_MANAGER_H
#define OM_KEY_MANAGER_H

/*
 *
 */

#include "common/omStd.h"
#include "common/omSerialization.h"

#include "utility/biMap.h"

#include <QKeySequence>

class QKeyEvent;



enum OmKeySequence {
	OmKeySeq_Unknown = 0,
	
	//not user definable
	OmKeySeq_Open,
	OmKeySeq_Save,
	OmKeySeq_Undo,
	OmKeySeq_Redo,
	
	
	//user definable
	OmKeySeq_Navigation_Mode,
	OmKeySeq_Edit_Mode,
	
	
	//view3d
	OmKeySeq_Focus_Select,
	OmKeySeq_Focus_Select_Append,
	
	OmKeySeq_Focus_X_Inc,
	OmKeySeq_Focus_X_Dec,
	OmKeySeq_Focus_Y_Inc,
	OmKeySeq_Focus_Y_Dec,
	OmKeySeq_Focus_Z_Inc,
	OmKeySeq_Focus_Z_Dec
};







class OmKeyManager {

public:
	
	static OmKeyManager* Instance();
	static void Delete();
	
	//map
	static void MapKeySequence( OmKeySequence, QKeyEvent *);
	static void MapKeySequence( OmKeySequence, QKeySequence );
	static void MapKeySequence( OmKeySequence, int );
	static void MapKeySequence( OmKeySequence, string keySequenceStr );
	
	//lookup
	static OmKeySequence LookupKeySequence( QKeyEvent * );
	
	//convert
	static QKeySequence OmToQKeySequence( OmKeySequence );
	
	//set defaults
	static void SetDefaults();

protected:
	// singleton constructor, copy constructor, assignment operator protected
	OmKeyManager();
	~OmKeyManager();
	OmKeyManager(const OmKeyManager&);
	OmKeyManager& operator= (const OmKeyManager&);

	
private:
	//singleton
	static OmKeyManager* mspInstance;
		
	//bimap key defs to key sequence string
	BiMap< OmKeySequence, string > mDefsToKeySeqBiMap; 
	
	
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int file_version);
};







/////////////////////////////////
///////		 Serialization


BOOST_CLASS_VERSION(OmKeyManager, 0)

template<class Archive>
void 
OmKeyManager::serialize(Archive & ar, const unsigned int file_version) {
	ar & mDefsToKeySeqBiMap;
}



#endif
