#pragma once

#include "common/common.h"

class dataPath {
private:
    std::string path_;

public:
    dataPath()
    {}

    dataPath(const char* str)
        : path_(std::string(str))
    {}

    dataPath(const std::string& str)
        : path_(str)
    {}

    dataPath(const QString& str)
        : path_(str.toStdString())
    {}

    void setPath(const char* str){
        path_ = std::string(str);
    }

    void setPath(const std::string& str){
        path_ = str;
    }

    void setPath(const QString& str){
        path_ = str.toStdString();
    }

    const std::string& getString() const {
        return path_;
    }

    const QString getQString() const {
        return QString::fromStdString(path_);
    }

    friend std::ostream& operator<<(std::ostream& out, const dataPath& in)
    {
        out << "\"" << in.path_ << "\"";
        return out;
    }
};

