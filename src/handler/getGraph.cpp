#include "handler/handler.h"
#include "pipeline/utility.hpp"
#include "volume/volume.h"
#include "segments/mstTypes.hpp"

namespace om {
namespace handler{

void get_mst(std::vector<server::edge>& _return, const volume::volume& vol)
{
	std::string uri = vol.Uri() + "segmentations/segmentation1/segments/mst.data";

	datalayer::memMappedFile<segments::mstEdge> mstData(fname);

    _return.resize(mstData.Length());
    for(size_t i = 0; i < _return.size(); i++)
    {
    	_return[i].a = mstData.GetPtr()[i].node1ID;
    	_return[i].b = mstData.GetPtr()[i].node2ID;
    	_return[i].value = mstData.GetPtr()[i].threshold;
    }
}


void get_graph(std::vector<server::edge>& _return, const volume::volume& vol)
{
	get_mst(_return, vol);
}

}}		