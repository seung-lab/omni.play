#pragma once

#include "pipeline/stage.hpp"

#include "boost/scoped_array.hpp"

namespace om {
namespace pipeline {

template<typename T>
class set_filter : public stage
{
protected:
    const std::set<T>& vals_;
    const T outputAs_;

public:
	set_filter(const std::set<T>& vals, T outputAs = 0)
		: vals_(vals)
		, outputAs_(outputAs)
	{}

    data_var operator()(const data<T>& in)
    {
        data<T> out;
        out.size = in.size;
        out.data = utility::smartPtr<T>::MallocNumElements(out.size);

        for(int i = 0; i < in.size; i++)
        {
            if(segIds_.find(inupt[i]) != segIds_.end())
            {
            	if(outputAs_) {
            		data_[i] = outputAs_;
            	} else {
                	data_[i] = input[i];
            	}
            } else {
                data_[i] = 0;
            }
        }
        return out;
    }

    data_var operator()(const datalayer::memMappedFile<T>& in) const
    {
        data<T> out;
        out.size = in.size;
        out.data = utility::smartPtr<T>::MallocNumElements(out.size);

        for(int i = 0; i < in.size; i++)
        {
            if(segIds_.find(inupt[i]) != segIds_.end()) {
                data_[i] = input[i];
            } else {
                data_[i] = 0;
            }
        }
        return out;
    }

    template <typename S>
    data_var operator()(const data<S> in) {
    	throw argException("Attempting to filter the wrong type of data.");
    }

    template <typename S>
    data_var operator()(const datalayer::memMappedFile<S>& in) {
    	throw argException("Attempting to filter the wrong type of data.");
    }
};

template<typename T>
data_var operator>>(const dataSrcs& d, const set_filter<T>& v) {
    return boost::apply_visitor(v, d);
}

}} // namespace om::pipeline::
