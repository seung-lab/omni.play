#ifndef OM_VOLUME_DATA_HPP
#define OM_VOLUME_DATA_HPP

#include "datalayer/fs/omMemMappedVolume.hpp"
#include "volume/omMipVolume.h"
#include "volume/omVolumeTypes.hpp"

class OmVolumeData {
public:
	OmVolumeData(OmMipVolume* vol) : vol_(vol) {}

	void loadVolData();

	int GetBytesPerSample(){
		return boost::apply_visitor(GetBytesPerSampleVisitor(), volData_);
	}

	OmRawDataPtrs getChunkPtrRaw(const OmMipChunkCoord & coord){
		return boost::apply_visitor(getChunkPtrVisitorRaw(coord), volData_);
	}

	void AllocMemMapFiles(const std::map<int, Vector3i> & levDims){
		boost::apply_visitor(AllocMemMapFilesVisitor(levDims), volData_);
	}


private:
	OmMipVolume *const vol_;
	boost::variant<OmMemMappedVolume<int8_t, OmMipVolume>,
		       OmMemMappedVolume<uint8_t, OmMipVolume>,
		       OmMemMappedVolume<int32_t, OmMipVolume>,
		       OmMemMappedVolume<uint32_t, OmMipVolume>,
		       OmMemMappedVolume<float, OmMipVolume> > volData_;

	void determineOldVolType();

	class GetBytesPerSampleVisitor : public boost::static_visitor<int> {
	public:
		template <typename T> int operator()( T & d ) const {
			return d.GetBytesPerSample();
		}
	};

	class AllocMemMapFilesVisitor : public boost::static_visitor<> {
	public:
		AllocMemMapFilesVisitor(const std::map<int, Vector3i> & levDims)
			: levelsAndDims(levDims) {}
		template <typename T> void operator()( T & d ) const {
			d.AllocMemMapFiles(levelsAndDims);
		}
	private:
		const std::map<int, Vector3i> levelsAndDims;
	};

	class getChunkPtrVisitorRaw : public boost::static_visitor<OmRawDataPtrs>{
	public:
		getChunkPtrVisitorRaw(const OmMipChunkCoord & coord)
			: coord(coord) {}

		template <typename T>
		OmRawDataPtrs operator()(T & d ) const {
			return d.getChunkPtr(coord);
		}
	private:
		OmMipChunkCoord coord;
	};
};

#endif
