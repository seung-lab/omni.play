
#include "annotation/annotation.h"
#include "annotation/annotationYaml.hpp"
#include "volume/omSegmentation.h"
#include "volume/omSegmentationFolder.h"
#include "utility/yaml/omYaml.hpp"
#include "utility/yaml/genericManager.hpp"

#include <fstream>
#include <sstream>


std::string om::annotation::manager::filePathV1() const {
    return vol_->Folder()->AnnotationFile().toStdString();
}

void om::annotation::manager::Save() const {
    std::string fnp = filePathV1();
    
    YAML::Emitter e;
    
    e << YAML::BeginDoc << YAML::BeginMap;
    YAML::genericManager::Save(e, *this);
    e << YAML::EndMap << YAML::EndDoc;
    
    om::yaml::yamlUtil::Write(fnp, e);
}

void om::annotation::manager::Load() {
    std::string fnp = filePathV1();
    if(!om::file::exists(fnp)) {
        std::cout << "Unable to find Annotations file.  Creating new one." << std::endl;
        return;
    }
    
    YAML::Node n;
    try
    {
        om::yaml::yamlUtil::Read(fnp, n);
        YAML::genericManager::Load(n, *this);
    }
    catch(YAML::Exception e)
    {
        std::stringstream ss;
        ss << "Error Loading Annotations: " << e.what() << ".\n";
        throw OmIoException(ss.str());
    }
    
    return;
}
