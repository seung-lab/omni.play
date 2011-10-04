#pragma once

#include "yaml-cpp/yaml.h"

#include <fstream>

namespace om {
namespace yaml {

// // our data types
// struct Vec3 {
//    float x, y, z;
// };

// struct Power {
//    std::string name;
//    int damage;
// };

// struct Monster {
//    std::string name;
//    Vec3 position;
//    std::vector <Power> powers;
// };

// // now the extraction operators for these types
// void operator >> (const YAML::Node& node, Vec3& v) {
//    node[0] >> v.x;
//    node[1] >> v.y;
//    node[2] >> v.z;
// }

// void operator >> (const YAML::Node& node, Power& power) {
//    node["name"] >> power.name;
//    node["damage"] >> power.damage;
// }

// void operator >> (const YAML::Node& node, Monster& monster) {
//    node["name"] >> monster.name;
//    node["position"] >> monster.position;
//    const YAML::Node& powers = node["powers"];
//    for(unsigned i=0;i<powers.size();i++) {
//       Power power;
//       powers[i] >> power;
//       monster.powers.push_back(power);
//    }
// }

template <typename T>
class parser {
public:
    // based on http://code.google.com/p/yaml-cpp/wiki/HowToParseADocument#A_Complete_Example
    std::vector<T> Parse(const std::string& fnp)
    {
        if(!QFile::exists(QString::fromStdString(fnp))){
            throw OmIoException("could not find file", fnp);
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
};

} // namespace yaml
} // namespace om

