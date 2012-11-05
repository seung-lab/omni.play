#pragma once

#include "common/common.h"
#include "common/exception.h"

#include <boost/iterator/iterator_facade.hpp>

class OmChunkVoxelWalker
	: public boost::iterator_facade<OmChunkVoxelWalker,
					Vector3i const,
					boost::forward_traversal_tag > {
public:
	explicit OmChunkVoxelWalker(const int chunkDim)
		: x(0), y(0), z(0)
		, chunkDim_(chunkDim)
		, curPos_(Vector3i(0,0,0))
	{}

	OmChunkVoxelWalker(const int x, const int y, const int z,
			   const int chunkDim)
		: x(x), y(y), z(z)
		, chunkDim_(chunkDim)
		, curPos_(Vector3i(x,y,z))
	{}

	OmChunkVoxelWalker begin(){
		return OmChunkVoxelWalker(0,0,0, chunkDim_);
	}

	OmChunkVoxelWalker end(){ // one beyond the end
		return OmChunkVoxelWalker(chunkDim_,
					  chunkDim_,
					  chunkDim_,
					  chunkDim_);
	}

	bool operator <(const OmChunkVoxelWalker& rhs) const{
		if(chunkDim_ != rhs.chunkDim_){
			throw ArgException("different dimensions not allowed");
		}
		return (x < rhs.x &&
			y < rhs.y &&
			z < rhs.z);
	}

private:
	int x, y, z;
	const int chunkDim_;
	Vector3i curPos_;

	OmChunkVoxelWalker()
		: x(0), y(0), z(0)
		, chunkDim_(1)
		, curPos_(Vector3i(0,0,0))
	{} // user must specify chunkDim!

	void increment() {
		++x;
		if(x == chunkDim_){
			x = 0;
			++y;
			if(y == chunkDim_){
				y = 0;
				z++;
			}
		}

		curPos_ = Vector3i(x,y,z);
	}

	bool equal(OmChunkVoxelWalker const& rhs) const {
		if(chunkDim_ != rhs.chunkDim_){
			throw ArgException("different dimensions not allowed");
		}
		return this->curPos_ == rhs.curPos_;
	}

	const Vector3i& dereference() const {
		return curPos_;
	}

	bool operator !=(const OmChunkVoxelWalker&) const{
		throw ArgException("not implemented");
	}

	friend class boost::iterator_core_access;
};

