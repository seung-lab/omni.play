#ifndef OM_VOLUME_DATA_HPP
#define OM_VOLUME_DATA_HPP

#include "datalayer/fs/omMemMappedVolume.hpp"
#include "boost/variant.hpp"
#include "volume/omMipVolume.h"

class OmVolumeData {
public:
	OmVolumeData(OmMipVolume* vol) : vol_(vol) {}

	void loadVolData();

	OmDataWrapperPtr getChunkPtr(OmMipChunkCoord & coord){
		return boost::apply_visitor(getChunkPtrVisitor(coord), volData);
	}

	template <typename C>
	C* getChunkPtrRawRaw(OmMipChunkCoord & coord){
		return boost::apply_visitor(getChunkPtrVisitorRawRaw<C>(coord), volData);
	}

	boost::variant<int8_t*, uint8_t*, int32_t*, uint32_t*, float*> getChunkPtrRaw(OmMipChunkCoord & coord){
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

	class AllocMemMapFilesVisitor : public boost::static_visitor<> {
	public:
		template <typename T> void operator()( T & d ) const {
			d.AllocMemMapFiles();
		}
	};

	template< typename C>
	class getChunkPtrVisitorRawRaw : public boost::static_visitor< C*> {
	public:
		getChunkPtrVisitorRawRaw(OmMipChunkCoord & coord) : coord(coord) {}

		template <typename T>
		C* operator()(T & d ) const {
			return d.getChunkPtr(coord);
		}
	private:
		OmMipChunkCoord coord;
	};

	class getChunkPtrVisitorRaw :
		public boost::static_visitor<boost::variant<int8_t*, uint8_t*, int32_t*, uint32_t*, float*> >{
	public:
		getChunkPtrVisitorRaw(OmMipChunkCoord & coord) : coord(coord) {}

		template <typename T>
		boost::variant<int8_t*, uint8_t*, int32_t*, uint32_t*, float*>
		operator()(T & d ) const {
			return d.getChunkPtr(coord);
		}
	private:
		OmMipChunkCoord coord;
	};


	class getChunkPtrVisitor : public boost::static_visitor<OmDataWrapperPtr>{
	public:
		getChunkPtrVisitor(OmMipChunkCoord & coord) : coord(coord) {}

		OmDataWrapperPtr operator()(OmMemMappedVolume<int8_t, OmMipVolume> & d ) const {
			return OmDataWrapper<int8_t>::producemmap(d.getChunkPtr(coord));
		}
		OmDataWrapperPtr operator()(OmMemMappedVolume<uint8_t, OmMipVolume> & d ) const {
			return OmDataWrapper<uint8_t>::producemmap(d.getChunkPtr(coord));
		}
		OmDataWrapperPtr operator()(OmMemMappedVolume<int32_t, OmMipVolume> & d ) const {
			return OmDataWrapper<int32_t>::producemmap(d.getChunkPtr(coord));
		}
		OmDataWrapperPtr operator()(OmMemMappedVolume<uint32_t, OmMipVolume> & d ) const {
			return OmDataWrapper<uint32_t>::producemmap(d.getChunkPtr(coord));
		}
		OmDataWrapperPtr operator()(OmMemMappedVolume<float, OmMipVolume> & d ) const {
			return OmDataWrapper<float>::producemmap(d.getChunkPtr(coord));
		}
	private:
		OmMipChunkCoord coord;
	};

};

#endif
