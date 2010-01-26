
#include "omKeyManager.h"


#include <QKeyEvent>
#include "system/omDebug.h"


#define DEBUG 0



//init instance pointer
OmKeyManager* OmKeyManager::mspInstance = 0;




#pragma mark -
#pragma mark OmKeyManager
/////////////////////////////////
///////
///////		 OmKeyManager
///////

OmKeyManager::OmKeyManager() { 
	
}

OmKeyManager::~OmKeyManager() { 
	
}



OmKeyManager* 
OmKeyManager::Instance() {
	if(NULL == mspInstance) {
		mspInstance = new OmKeyManager;
	}
	return mspInstance;
}


void 
OmKeyManager::Delete() {
	if(mspInstance) delete mspInstance;
	mspInstance = NULL;
}







#pragma mark -
#pragma mark Map Key Sequence
/////////////////////////////////
///////		 Map Key Sequence


void 
OmKeyManager::MapKeySequence( OmKeySequence omKeySeq, QKeyEvent *keyEvent) {
	
	//create QKeySequence from event
	QKeySequence q_key_sequence = QKeySequence(keyEvent->modifiers() + keyEvent->key());

	//create string from sequence
	string key_sequence_str = QString(q_key_sequence).toStdString();
	
	//map enum to string
	Instance()->mDefsToKeySeqBiMap.map(omKeySeq, key_sequence_str);
}



void 
OmKeyManager::MapKeySequence( OmKeySequence omKeySeq, QKeySequence qKeySeq) {
	//create string from sequence
	string key_sequence_str = QString(qKeySeq).toStdString();
	
	//map enum to string
	Instance()->mDefsToKeySeqBiMap.map(omKeySeq, key_sequence_str);
}


void 
OmKeyManager::MapKeySequence( OmKeySequence omKeySeq, int value) {
	//create seq
	QKeySequence qKeySeq = QKeySequence(value);
	
	//create string from sequence
	string key_sequence_str = QString(qKeySeq).toStdString();
	
	//map enum to string
	Instance()->mDefsToKeySeqBiMap.map(omKeySeq, key_sequence_str);
}


void 
OmKeyManager::MapKeySequence( OmKeySequence omKeySeq, string keySequenceStr ) {
	//map enum to string
	Instance()->mDefsToKeySeqBiMap.map(omKeySeq, keySequenceStr);
}









#pragma mark -
#pragma mark Lookup Key Sequence
/////////////////////////////////
///////		 Lookup Key Sequence


/*
 *	Returns the OmKeySequence that is mapped to the given QKeyEvent.
 *	Returns OmKeySeq_None if no matching sequence is found.
 */	
OmKeySequence 
OmKeyManager::LookupKeySequence( QKeyEvent *keyEvent) {
	
	//create QKeySequence from event
	QKeySequence q_key_sequence = QKeySequence(keyEvent->modifiers() + keyEvent->key());
	
	//create string from sequence
	string key_sequence_str = QString(q_key_sequence).toStdString();

	//lookup in bimap
	OmKeySequence *result_om_key_seq = Instance()->mDefsToKeySeqBiMap[key_sequence_str];
	
	//check result of lookup
	if(NULL == result_om_key_seq) {
		//if null, then seq not mapped so unknown
		return OmKeySeq_Unknown;
	} else {
		//else return result
		return *result_om_key_seq;
	}
	
}



/*
 *	Convert OmKeySequence to QKeySequence.
 *	Returns empty QKeySequence() if given OmKeySequence is not mapped.
 */
QKeySequence 
OmKeyManager::OmToQKeySequence( OmKeySequence omKeySeq ) {
	
	//check if mapped
	string *sequence_str = Instance()->mDefsToKeySeqBiMap[omKeySeq];
	if(NULL == sequence_str) {
		return QKeySequence();
	} else {
		return QKeySequence::fromString(QString::fromStdString(*sequence_str));
	}
	
}







#pragma mark -
#pragma mark Default Mapping
/////////////////////////////////
///////		 Default Mapping

void 
OmKeyManager::SetDefaults() {
	
	//not user definable
	MapKeySequence(OmKeySeq_Save,					QKeySequence(QKeySequence::Save));
	MapKeySequence(OmKeySeq_Open,					QKeySequence(QKeySequence::Open));
	MapKeySequence(OmKeySeq_Undo,					QKeySequence(QKeySequence::Undo));
	MapKeySequence(OmKeySeq_Redo,					QKeySequence(QKeySequence::Redo));
	
	MapKeySequence(OmKeySeq_Navigation_Mode,		Qt::CTRL + Qt::Key_N);
	MapKeySequence(OmKeySeq_Edit_Mode,				Qt::CTRL + Qt::Key_E);
	
	
	
	//view3d
	MapKeySequence(OmKeySeq_Focus_Select,			Qt::Key_Space);
	MapKeySequence(OmKeySeq_Focus_Select_Append,	Qt::SHIFT + Qt::Key_Space);
}
