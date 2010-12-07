#ifndef OM_VALID_GROUP_NUM_HPP
#define OM_VALID_GROUP_NUM_HPP

#include "datalayer/archive/omDataArchiveStd.hpp"
#include "datalayer/fs/omFileNames.hpp"
#include "utility/omLockedPODs.hpp"
#include "segment/omSegmentCache.h"
#include "utility/dataWrappers.h"

#include <QVector> //TODO: switch to mem-mapped file...

class OmValidGroupNum {
private:
	OmSegmentation* segmentation_;
	int version_;
	LockedUint32 maxGroupNum_;
	std::vector<uint32_t> segToGroupNum_;

public:
	OmValidGroupNum(OmSegmentation* segmentation)
		: segmentation_(segmentation)
		, version_(1)
	{
		maxGroupNum_.set(1);
	}

	void Load() {
		load();
	}

	void Save() const {
		save();
	}

	void Set(const SegmentationDataWrapper& sdw,
			 boost::shared_ptr<std::set<OmSegment*> > segs,
			 const bool isValid)
	{
		const uint32_t size = sdw.GetSegmentCache()->getMaxValue();
		if(segToGroupNum_.size() <= size){
			segToGroupNum_.resize(size*1.5, 0);
		}

		if(isValid){
			const uint32_t groupNum = maxGroupNum_.inc();
			FOR_EACH(iter, *segs){
				OmSegment* seg = *iter;
				const OmSegID segID = seg->value();
				segToGroupNum_[segID] = groupNum;
			}
		} else {
			FOR_EACH(iter, *segs){
				OmSegment* seg = *iter;
				const OmSegID segID = seg->value();
				segToGroupNum_[segID] = 0;
			}
		}
	}

	inline uint32_t Get(const OmSegID segID) const
	{
		//TODO: memmap, so no worry about OmSegIDs > int_max
		const uint32_t size = segID;
		if(segToGroupNum_.size() <= size){
			return 0;
		}
		return segToGroupNum_[segID];
	}

	inline uint32_t Get(OmSegment* seg) const {
		return Get(seg->value());
	}

private:
	QString filePathV1() const
	{
		const QString volPath =	OmFileNames::MakeVolSegmentsPath(segmentation_);
		const QString fullPath = QString("%1valid_group_num.data.ver1")
			.arg(volPath);

		return fullPath;
	}

	void load()
	{
		const QString filePath = filePathV1();

		QFile file(filePath);

		if(!file.exists()){
			return;
		}

		if(!file.open(QIODevice::ReadOnly)){
			throw OmIoException("error reading file", filePath);
		}

		QDataStream in(&file);
		in.setByteOrder( QDataStream::LittleEndian );
		in.setVersion(QDataStream::Qt_4_6);

		in >> version_;
		in >> segToGroupNum_;

		quint64 maxGroupNum;
		in >> maxGroupNum;
		maxGroupNum_.set(maxGroupNum);

		if(!in.atEnd()){
			throw OmIoException("corrupt file?", filePath);
		}
	}

	void save() const
	{
		const QString filePath = filePathV1();

		QFile file(filePath);

		if (!file.open(QIODevice::WriteOnly)) {
			throw OmIoException("could not write file", filePath);
		}

		QDataStream out(&file);
		out.setByteOrder( QDataStream::LittleEndian );
		out.setVersion(QDataStream::Qt_4_6);

		out << version_;
		out <<segToGroupNum_;

		const quint64 maxGroupNum = maxGroupNum_.get();
		out << maxGroupNum;
	}

};

#endif
