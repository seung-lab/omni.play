#include "annotation/omAnnotationYaml.hpp"
#include "volume/omSegmentation.h"
#include "volume/omSegmentationFolder.h"

QString om::annotation::manager::filePathV1() const {
    return vol_->Folder()->AnnotationFile();
}

void om::annotation::manager::Save() const
{
    YAML::Emitter out;

    out << YAML::BeginSeq;
    out << YAML::Flow;
    out << annotations_;
    out << YAML::EndSeq;

    const QString filePath = filePathV1();

    std::ofstream fout(qPrintable(filePath));

    fout << out.c_str();
}

void om::annotation::manager::Load()
{
    for(int i = 0; i < 10; ++i)
    {
        om::annotation::data d;
        d.id = i;

        annotations_.push_back(d);
    }

    return;

    const QString filePath = filePathV1();

    if(!QFile::exists(filePath)){
        return;
    }

    om::yaml::parser<data> yamlParser;

    annotations_ = yamlParser.Parse(filePath.toStdString());
}

