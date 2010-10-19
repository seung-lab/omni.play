#ifndef OM_MST_H
#define OM_MST_H

#include "datalayer/omDataWrapper.h"
#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/fs/omIOnDiskFile.h"
#include "datalayer/fs/omFileQT.hpp"

class OmSegmentation;

struct OmMSTEdge {
	uint32_t number;
	uint32_t node1ID;
	uint32_t node2ID;
	double threshold;
	uint8_t userJoin;
	uint8_t userSplit;
	uint8_t wasJoined;	// transient state
};

class OmMST {
public:
	OmMST(OmSegmentation* segmentation);
	~OmMST(){}

	void Read();
	void Flush();
	void import(const std::string& fname);

	bool isValid(){
		return numEdges_ > 0;
	}

	uint32_t NumEdges() const {
		return numEdges_;
	}

	double UserThreshold() const {
		return userThreshold_;
	}
	void SetUserThreshold(const float t) {
		userThreshold_ = t;
	}

	OmMSTEdge* Edges(){
		return edges_;
	}

private:
	OmSegmentation* segmentation_;
	uint32_t numEdges_;
	double userThreshold_;

	boost::shared_ptr<OmIOnDiskFile<OmMSTEdge> > edgesPtr_;
	OmMSTEdge* edges_;

	void create();
	void convert();

	typedef OmFileReadQT<OmMSTEdge> reader_t;
	typedef OmFileWriteQT<OmMSTEdge> writer_t;

	QString memMapPathQStr();
	std::string memMapPath();

	friend class SegmentTests;

	friend QDataStream &operator<<(QDataStream& out, const OmSegmentation &);
	friend QDataStream &operator>>(QDataStream& in, OmSegmentation  &);
};

#endif
