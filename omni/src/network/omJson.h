#ifndef OM_JSON_H
#define OM_JSON_H

#include "network/details/omJsonTypes.hpp"

class OmJson {
private:
    om::json::reader_t value_;

public:
    OmJson()
    {}

    void Read(const std::string& str)
    {
        om::json::read(str, value_);
    }

    bool HasAction() const {
        return om::json::HasKey(value_, "action");
    }

    boost::optional<std::string> GetAction() const {
        return om::json::GetValue<std::string>(value_, "action");
    }

    template <typename T>
    boost::optional<std::vector<T> > Params() const {
        return om::json::GetVector<T>(value_, "params");
    }

    template <typename T>
    boost::optional<std::vector<T> > Params(const std::string& name) const {
        return om::json::GetVector<T>(value_, name);
    }

    std::string TypeStr() const {
        return om::json::TypeStr(value_);
    }

    template <typename T>
    boost::optional<T> GetValue(const std::string& name) const {
        return om::json::GetValue<T>(value_, name);
    }

    std::string Write(/*type....*/)
    {
        om::json::writer_t out;

        // push_back onto out

        return om::json::write(out);
    }
};

#endif
