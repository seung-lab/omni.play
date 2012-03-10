#include "common/common.h"
#include "pipeline/stage.hpp"
#include "datalayer/memMappedFile.hpp"
#include "utility/smartPtr.hpp"

namespace om {
namespace pipeline {

// TODO: Switch over to writing directly to string to avoid extra copy.
class getChunk : public stage
{
private:
    coords::volumeSystem vs_;
    coords::chunk cc_;

public:
    getChunk(coords::volumeSystem vs, coords::chunk cc)
        : vs_(vs)
        , cc_(cc)
    { }

    template <typename T>
    data_var operator()(const datalayer::memMappedFile<T>& in) const
    {
        data<T> out;
        uint64_t offset = cc_.chunkPtrOffset(&vs_, sizeof(T));
        T* chunkPtr = in.GetPtrWithOffset(offset);
        
        out.size = 128 * 128 * 128;
        out.data = utility::smartPtr<T>::MallocNumElements(out.size);
        std::copy(chunkPtr, &chunkPtr[out.size], out.data.get());
        
        return out;
    }
};

data_var operator>>(const dataSrcs& d, const getChunk& v);

class writeChunk : boost::static_visitor<>
{
private:
	dataSrcs file_;
	coords::data dc_;

public:
	writeChunk(dataSrcs file, coords::data dc)
		: file_(file)
		, dc_(dc)
	{}

	template<typename T>
	void operator()(const data<T>& toWrite)
	{
		datalayer::memMappedFile<T>* file = boost::get<datalayer::memMappedFile<T> >(file_);
		if(!file) {
			throw argException("Trying to write the wrong type of data.");
		}

		if(toWrite.size != 128 * 128 * 128) {
			throw argException("Chunk Data is the wrong size.");
		}

		coords::chunk cc = dc_.toChunk();
        uint64_t offset = cc.chunkPtrOffset(dc_.volume(), sizeof(T));
        T* chunkPtr = file->GetPtrWithOffset(offset);

		std::copy(toWrite->data.get(), &toWrite->data.get()[toWrite->size], chunkPtr);
	}
};

}}
