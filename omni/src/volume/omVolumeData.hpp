#ifndef OM_VOLUME_DATA_HPP
#define OM_VOLUME_DATA_HPP

#include "datalayer/fs/omMemMappedVolume.hpp"
#include "volume/omMipVolume.h"
#include "volume/omVolumeTypes.hpp"

class OmVolumeData {
public:
	OmVolumeData(OmMipVolume* vol) : vol_(vol) {}

	void loadVolData();

	OmRawDataPtrs getChunkPtrRaw(const OmMipChunkCoord & coord){
		return boost::apply_visitor(getChunkPtrVisitorRaw(coord), volData);
	}

	void AllocMemMapFiles(){
		boost::apply_visitor(AllocMemMapFilesVisitor(), volData);
	}

	boost::variant<OmMemMappedVolume<int8_t, OmMipVolume>,
		       OmMemMappedVolume<uint8_t, OmMipVolume>,
		       OmMemMappedVolume<int32_t, OmMipVolume>,
		       OmMemMappedVolume<uint32_t, OmMipVolume>,
		       OmMemMappedVolume<float, OmMipVolume> > volData;
private:
	OmMipVolume *const vol_;

	void determineOldVolType();

	class AllocMemMapFilesVisitor : public boost::static_visitor<> {
	public:
		template <typename T> void operator()( T & d ) const {
			d.AllocMemMapFiles();
		}
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
