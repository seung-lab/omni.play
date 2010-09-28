#ifndef OM_DATA_PATH_H
#define OM_DATA_PATH_H

#include "common/omCommon.h"

class OmDataPath
{
public:
    OmDataPath()
    {
    }

    OmDataPath( const std::string &str ): mPath( str )
    {
    }

    void setPath( const std::string &str )
    {
        mPath = str;
    }

    // why did we have two different function names? weird...
    void setPath( const QString &str )
    {
        mPath = str.toStdString();
    }

    void setPathQstr( const QString & str )
    {
        mPath = str.toStdString();
    }

    const std::string& getString() const
    {
        return mPath;
    }

	friend std::ostream& operator<<(std::ostream &out, const OmDataPath &in)
	{
		out << "\"" << in.mPath << "\"";
		return out;
	}

private:
    std::string mPath;
};

#endif
