#pragma once

#include <boost/iterator/filter_iterator.hpp>
#include <boost/unordered_map.hpp>
#include <zi/mutex.hpp>

#include "common/omException.h"
#include "utility/omLockedPODs.hpp"
#include "utility/yaml/manager.hpp"

namespace om {
namespace system {

template <typename T>
struct ManagedObject
{
	OmID ID;
	bool Enabled;
	zi::spinlock Lock;
	T* Object;
};

}}

namespace YAML {

template <typename T>
Emitter& operator<<(Emitter& out, const om::system::ManagedObject<T>& data)
{
    out << BeginMap;
    out << Key << "id" << Value << data.ID;
    out << Key << "enabled" << Value << data.Enabled;
    out << Key << "value" << Value << *data.Object;
    out << EndMap;

    return out;
}

}

namespace om {
namespace system {

template <typename T>
class Manager
{
public:
	typedef ManagedObject<T> obj;
	typedef boost::unordered_map<OmID, obj> coll;
	typedef typename coll::iterator iterator;
	typedef typename coll::const_iterator const_iterator;

private:
	coll objs_;
	size_t next_;
	zi::spinlock lock_;
	LockedBool disposed_;

public:
	Manager()
		: objs_()
		, next_(1)
	{
		disposed_.set(false);
	}

	~Manager()
	{
		zi::guard g(lock_);
		if(!disposed_.get())
		{
			FOR_EACH(iter, objs_) {
				delete iter->second.Object;
			}
			disposed_.set(true);
		}
	}

	obj& Add(T* toAdd, size_t id = 0, bool enabled = true)
	{
		zi::guard g(lock_);
		if(id == 0 || isValid(id)) {
			id = next_;
		}
		objs_[id].ID = id;
		objs_[id].Enabled = enabled;
		objs_[id].Object = toAdd;
		findNext();
	}

	bool IsValid(OmID id)
	{
		zi::guard g(lock_);
		return isValid(id);
	}

	obj& Get(OmID id)
	{
		zi::guard g(lock_);
		iterator iter = objs_.find(id);
		if (iter != end()) {
			throw OmArgException("Bad id");
		}
		return iter->second;
	}

	void Remove(OmID id)
	{
		zi::guard g(lock_);
		if(isValid(id)) {
			zi::guard g2(objs_[id].Lock);
			delete objs_[id].Object;
			objs_.erase(id);
			if(id < next_) {
				next_ = id;
			}
		}
	}

	void Save(YAML::Emitter& out) const
    {
    	out << YAML::BeginSeq;
        FOR_EACH(iter, objs_){
            out << iter->second;
        }
        out << YAML::EndSeq;
    }

    void Load(const YAML::Node& in)
    {
    	if(in.FindValue("size")) // Old Manager Format
    	{
    		OmIDsSet valid, enabled;
	        in["valid set"] >> valid;
	        in["enabled set"] >> enabled;

	        int idx = 0;
	        FOR_EACH(i, valid)
	        {
	        	T* obj = parse(in["values"][idx]);
	            Add(obj, *i, enabled.count(*i) > 0);
	            idx++;
	        }
	    } else {
	    	for(int i = 0; i < in.size(); ++i)
	    	{
	    		size_t id;
	    		in[i]["id"] >> id;
	    		bool enabled;
	    		in[i]["enabled"] >> enabled;
	    		T* obj = parse(in[i]["value"]);
    			Add(obj, id, enabled);
	    	}
	    }
    }

	inline iterator begin() {
		return objs_.begin();
	}

	inline iterator end() {
		return objs_.end();
	}

	inline const_iterator begin() const {
		return objs_.begin();
	}

	inline const_iterator end() const {
		return objs_.end();
	}

protected:
	virtual T* parse(const YAML::Node& in) = 0;

	inline bool isValid(OmID id) {
		return objs_.count(id) > 0;
	}

	void findNext() {
		for (int i = next_;; ++i) {
			if (!isValid(i)) {
				next_ = i;
				return;
			}
		}
	}
};


}} // namespace om::system::