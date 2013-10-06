#pragma once

#include <yaml-cpp-old/yaml.h>
#include "system/omGenericManager.hpp"
#include "datalayer/archive/baseTypes.hpp"

namespace om {
namespace data {
namespace archive {

class genericManager {
public:
    template <class T>
    static void Save(YAMLold::Emitter& out, const OmGenericManager<T>& gm)
    {
        out << YAMLold::Key << "size" << YAMLold::Value << gm.size_;
        out << YAMLold::Key << "valid set" << YAMLold::Value << gm.validSet_;
        out << YAMLold::Key << "enabled set" << YAMLold::Value << gm.enabledSet_;
        out << YAMLold::Key << "next id" << YAMLold::Value << gm.nextId_;

        out << YAMLold::Key << "values" << YAMLold::Value << YAMLold::BeginSeq;
        FOR_EACH(iter, gm.validSet_){
            out << *gm.vec_[*iter];
        }
        out << YAMLold::EndSeq;
    }

    template <class T>
    static void Load(const YAMLold::Node& in, OmGenericManager<T>& gm, void(*parse)(const YAMLold::Node&, T*) = NULL)
    {
        in["size"] >> gm.size_;
        in["valid set"] >> gm.validSet_;
        in["enabled set"] >> gm.enabledSet_;
        in["next id"] >> gm.nextId_;

        gm.vec_.resize(gm.size_, NULL);

        for(uint32_t i = 0; i < gm.validSet_.size(); ++i)
        {
            T* t = new T();
            if (!parse) {
            	in["values"][i] >> *t;
            } else {
            	parse(in["values"][i], t);
            }

            gm.vec_[ t->GetID() ] = t;

            gm.vecValidPtrs_.push_back(t);
        }
    }
};

}; // namespace archive
}; // namespace data
}; // namespace om