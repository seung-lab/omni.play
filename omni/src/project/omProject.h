#ifndef OM_PROJECT_H
#define OM_PROJECT_H

/*
 *	Manages data structures that are shared between various parts of the system.  Making centralized
 *	changes in the StateManager will send events that cause the other interested systems to be
 *	notified and synchronized.
 *
 *	Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include "common/omStd.h"
#include "system/omPreferences.h"
#include "system/omStateManager.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"


typedef int (*GGOCTFPointer) (char *, int, int, int mousex, int mousey);

class OmViewGroupState;

class OmProject {

public:
	
	static OmProject* Instance();
	static void Delete();
	
	//project properties
	static QString GetFileName() {return Instance()->mFileName;}
	static const string& GetDirectoryPath();
	static string GetTempDirectoryPath();
	
	//project IO
	static QString New( QString fileNameAndPath );
	static void Save();
	static void Commit();
	static void Load( QString fileNameAndPath, const bool autoOpenAndClose = false );
	static void Close();
	

        //volume management
        static OmChannel& GetChannel(OmId id);
        static OmChannel& AddChannel();
        static void RemoveChannel(OmId id);
        static bool IsChannelValid(OmId id);
        static const OmIds & GetValidChannelIds();
        static bool IsChannelEnabled(OmId id);
        static void SetChannelEnabled(OmId id, bool enable);
        
        static OmSegmentation& GetSegmentation(OmId id);
        static OmSegmentation& AddSegmentation();
        static void RemoveSegmentation(OmId id);
        static bool IsSegmentationValid(OmId id);
        static const OmIds & GetValidSegmentationIds();
        static bool IsSegmentationEnabled(OmId id);
        static void SetSegmentationEnabled(OmId id, bool enable);
	static void Draw(OmVolumeCuller & rCuller, OmViewGroupState * vgs);

	
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
	QString mFileName;
	QString mDirectoryPath;

        //data managers
        OmGenericManager<OmChannel> mChannelManager;
        OmGenericManager<OmSegmentation> mSegmentationManager;

        friend QDataStream &operator<<(QDataStream & out, const OmProject & p );
        friend QDataStream &operator>>(QDataStream & in, OmProject & p );
};

#endif
