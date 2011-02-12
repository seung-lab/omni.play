#ifndef OM_RING_BUFFER_HPP
#define OM_RING_BUFFER_HPP

#include "common/omCommon.h"
#include "utility/omTimer.hpp"
#include "zi/omUtility.h"
#include "zi/omMutex.h"

template <typename T>
class OmRingBuffer {
private:
	const uint32_t maxSize_;
	uint32_t curPos_;

	zi::rwmutex lock_;
	std::vector<T*> buffer_;

public:
	OmRingBuffer(const uint32_t maxSize = 50)
		: maxSize_(maxSize)
		, curPos_(0)
	{
		zi::rwmutex::write_guard g(lock_);
		buffer_.resize(maxSize_, NULL);
	}

	void Put(T* item)
	{
		zi::rwmutex::write_guard g(lock_);

		OmTimer timer;

		if(item == buffer_[curPos_]){
			return;
		}

		if(buffer_[curPos_]){
			printf("ring buffer is unmapping file...");
			buffer_[curPos_]->Unmap();
			timer.PrintDone();
		}

		buffer_[curPos_] = item;

		++curPos_;

		if(maxSize_ == curPos_){
			curPos_ = 0;
		}
	}
};

#endif
