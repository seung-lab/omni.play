#ifndef OM_PROJECT_H
#define OM_PROJECT_H

/*
 *	Manages data structures that are shared between various parts of the system.  Making centralized
 *	changes in the StateManager will send events that cause the other interested systems to be
 *	notified and synchronized.
 *
 *	Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include "common/omCommon.h"
#include "system/omGenericManager.h"

class OmChannel;
class OmSegmentation;
class OmVolumeCuller;
class OmViewGroupState;

typedef int (*GGOCTFPointer) (char *, int, int, int mousex, int mousey);

class OmProject : boost::noncopyable {
 public:
	
	static OmProject* Instance();
	static void Delete();
	
	//project properties
	static QString GetFileName() {return Instance()->mFileName;}
	
	//project IO
	static QString New( QString fileNameAndPath );
	static void Save();
	static void Commit();
	static void Load( QString fileNameAndPath );
	static void Close(bool doSave = true);

        //volume management
        static OmChannel& GetChannel(const OmId id);
        static OmChannel& AddChannel();
        static void RemoveChannel(const OmId id);
        static bool IsChannelValid(const OmId id);
        static const OmIDsSet & GetValidChannelIds();
        static bool IsChannelEnabled(const OmId id);
        static void SetChannelEnabled(const OmId id, const bool enable);
        
        static OmSegmentation& GetSegmentation(const OmId id);
        static OmSegmentation& AddSegmentation();
        static void RemoveSegmentation(const OmId id);
        static bool IsSegmentationValid(const OmId id);
        static const OmIDsSet & GetValidSegmentationIds();
        static bool IsSegmentationEnabled(const OmId id);
        static void SetSegmentationEnabled(const OmId id, const bool enable);
	static void SetCanFlush(bool canflush);
	static bool GetCanFlush();

 private:
	//singleton
	OmProject();
	~OmProject();
	static OmProject* mspInstance;
	
	//project
	QString mFileName;
	QString mDirectoryPath;
	bool mCanFlush;

        //data managers
        OmGenericManager<OmChannel> mChannelManager;
        OmGenericManager<OmSegmentation> mSegmentationManager;

        friend QDataStream &operator<<(QDataStream & out, const OmProject & p );
        friend QDataStream &operator>>(QDataStream & in, OmProject & p );
};

#endif
