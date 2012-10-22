#pragma once

#include "pipeline/stage.hpp"

#include "boost/scoped_array.hpp"

namespace om {
namespace pipeline {

template<typename T>
class set_filter //: public stage
{
protected:
    const std::set<T>& vals_;
    const T outputAs_;

public:
	set_filter(const std::set<T>& vals, T outputAs = 0)
		: vals_(vals)
		, outputAs_(outputAs)
	{}

    data<T> operator()(const data<T>& in) const
    {
    	std::cout << "Filtering." << std::endl;
    	data<T> out;
        out.size = in.size;
        out.data = utility::smartPtr<T>::MallocNumElements(out.size);

        doFilter(in.data.get(), out.data.get(), out.size);
        std::cout << "Done Filtering." << std::endl;
        return out;
    }

    // data_var operator()(const datalayer::memMappedFile<T>& in) const
    // {
    //     data<T> out;
    //     out.size = in.Size();
    //     out.data = utility::smartPtr<T>::MallocNumElements(out.size);

    //     doFilter(in.GetPtr(), out.data.get(), out.size);
    //     return out;
    // }


    // template <typename S>
    // data_var operator()(const data<S> in) const {
    // 	throw argException("Attempting to filter the wrong type of data.");
    // }

    // template <typename S>
    // data_var operator()(const datalayer::memMappedFile<S>& in) const {
    // 	throw argException("Attempting to filter the wrong type of data.");
    // }

private:
    void doFilter(const T* inData, T* outData, size_t size) const
    {
    	for(int i = 0; i < size; i++)
        {
            if(vals_.find(inData[i]) != vals_.end())
            {
            	if(outputAs_) {
            		outData[i] = outputAs_;
            	} else {
                	outData[i] = inData[i];
            	}
            } else {
                outData[i] = 0;
            }
        }
    }
};

// template<typename T>
// data_var operator>>(const dataSrcs& d, const set_filter<T>& v) {
//     return boost::apply_visitor(v, d);
// }

}} // namespace om::pipeline::
