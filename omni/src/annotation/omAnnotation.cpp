#include "annotation/omAnnotationYaml.hpp"
#include "volume/omSegmentation.h"
#include "volume/omSegmentationFolder.h"

QString om::annotation::manager::filePathV1() const {
    return vol_->Folder()->AnnotationFile();
}

void om::annotation::manager::Save() const
{

}

void om::annotation::manager::Load()
{
    
}

