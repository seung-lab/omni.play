#ifndef OM_METADATA_SEGMENTATION_HPP
#define OM_METADATA_SEGMENTATION_HPP

#include <QDataStream>

class OmSegmentation;

class OmMetadataSegmentation {
private:
	const int fileVersion_;

public:
	OmMetadataSegmentation(const int fileVersion)
		: fileVersion_(fileVersion)
	{}

//	void Read(QDataStream& in, OmSegmentation& seg);
//	void Write(QDataStream& out, const OmSegmentation& seg);
};

#endif
