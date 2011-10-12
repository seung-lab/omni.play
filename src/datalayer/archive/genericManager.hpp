#pragma once

#include <yaml-cpp/yaml.h>
#include "system/genericManager.hpp"
#include "datalayer/archive/baseTypes.hpp"

namespace om {
namespace datalayer {
namespace archive {

class genericManager {
public:
    template <class T>
    static void Save(YAML::Emitter& out, const genericManager<T>& gm)
    {
        out << YAML::Key << "size" << YAML::Value << gm.size_;
        out << YAML::Key << "valid set" << YAML::Value << gm.validSet_;
        out << YAML::Key << "enabled set" << YAML::Value << gm.enabledSet_;
        out << YAML::Key << "next id" << YAML::Value << gm.nextId_;
        
        out << YAML::Key << "values" << YAML::Value << YAML::BeginSeq;
        FOR_EACH(iter, gm.validSet_){
            out << *gm.vec_[*iter];
        }
        out << YAML::EndSeq;
    }
    
    template <class T>
    static void Load(const YAML::Node& in, genericManager<T>& gm)
    {
        in["size"] >> gm.size_;
        in["valid set"] >> gm.validSet_;
        in["enabled set"] >> gm.enabledSet_;
        in["next id"] >> gm.nextId_;
        
        gm.vec_.resize(gm.size_, NULL);
        
        for(uint32_t i = 0; i < gm.validSet_.size(); ++i)
        {
            T* t = new T();
            in["values"][i] >> *t;
            gm.vec_[ t->GetID() ] = t;
            
            gm.vecValidPtrs_.push_back(t);
        }
    }
};

}; // namespace archive
}; // namespace datalayer
}; // namespace om
