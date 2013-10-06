#pragma once

#include "yaml-cpp-old/yaml.h"
#include "datalayer/fs/omFile.hpp"
#include <fstream>

namespace om {
namespace yaml {

class util {
public:
    // based on http://code.google.com/p/yaml-cpp/wiki/HowToParseADocument#A_Complete_Example
    template <typename T>
    static std::vector<T> Parse(const std::string& fnp)
    {
        if(!om::file::old::exists(fnp)){
            throw om::IoException("could not find file");
        }

        std::ifstream fin(fnp.c_str());

        YAMLold::Parser parser(fin);

        YAMLold::Node doc;

        parser.GetNextDocument(doc);

        std::vector<T> ret;

        for(uint32_t i = 0; i < doc.size(); ++i)
        {
            T newDoc;
            doc[i] >> newDoc;
            ret.push_back(newDoc);
        }

        return ret;
    }

    // based on http://code.google.com/p/yaml-cpp/wiki/HowToParseADocument#A_Complete_Example
    static void Read(const std::string& fnp, YAMLold::Node& node)
    {
        if(!om::file::old::exists(fnp)){
            throw om::IoException("could not find file");
        }

        std::ifstream fin(fnp.c_str());

        YAMLold::Parser parser(fin);

        parser.GetNextDocument(node);
    }

    // based on http://code.google.com/p/yaml-cpp/wiki/HowToParseADocument#A_Complete_Example
    static void Write(const std::string& fnp, YAMLold::Emitter& emitter)
    {
        std::ofstream fout(fnp.c_str());

        fout << emitter.c_str();

        fout.close();
    }

    template <typename T>
    static void OptionalRead(const YAMLold::Node& n, const std::string& name, T &data, const T &defaultValue)
    {
        if(n.FindValue(name)) {
            n[name] >> data;
        } else {
            data = defaultValue;
        }
    }

    template <typename T>
    static void OptionalRead(const YAMLold::Node& n, const std::string& name, boost::optional<T> &data)
    {
        if(n.FindValue(name)) {
            T read;
            n[name] >> read;
            data = read;
        }
    }
};

} // namespace yaml
} // namespace om

