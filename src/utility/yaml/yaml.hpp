#pragma once

#include "common/common.h"
#include "yaml-cpp/yaml.h"
#include "boost/filesystem.hpp"
#include "boost/optional.hpp"

#include <fstream>

namespace om {
namespace yaml {

class yamlUtil {
public:
    // based on http://code.google.com/p/yaml-cpp/wiki/HowToParseADocument#A_Complete_Example
    template <typename T>
    static std::vector<T> Parse(const std::string& fnp)
    {
        if(!boost::filesystem::exists(fnp)){
            throw common::ioException("could not find file", fnp);
        }

        std::ifstream fin(fnp.c_str());

        YAML::Parser parser(fin);

        YAML::Node doc;

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
    static void Read(const std::string& fnp, YAML::Node& node)
    {
        if(!boost::filesystem::exists(fnp)){
            throw common::ioException("could not find file", fnp);
        }

        std::ifstream fin(fnp.c_str());

        YAML::Parser parser(fin);

        parser.GetNextDocument(node);
    }

    // based on http://code.google.com/p/yaml-cpp/wiki/HowToParseADocument#A_Complete_Example
    static void Write(const std::string& fnp, YAML::Emitter& emitter)
    {
        std::ofstream fout(fnp.c_str());

        fout << emitter.c_str();

        fout.close();
    }

    template <typename T>
    static void OptionalRead(const YAML::Node& n, const std::string& name, T &data, const T &defaultValue)
    {
        if(n.FindValue(name)) {
            n[name] >> data;
        } else {
            data = defaultValue;
        }
    }

    template <typename T>
    static void OptionalRead(const YAML::Node& n, const std::string& name, boost::optional<T> &data)
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

