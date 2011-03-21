#ifndef OM_RAND_COLOR_FILE_HPP
#define OM_RAND_COLOR_FILE_HPP

#include "common/omCommon.h"
#include "common/omString.hpp"
#include "datalayer/fs/omFile.hpp"
#include "datalayer/fs/omFileNames.hpp"
#include "utility/omRand.hpp"

class OmRandColorFile {
private:
    static const int version_ = 1;

    const std::string fnp_;

    boost::shared_ptr<QFile> file_;
    OmColor* values_;
    int64_t numEntries_;

    friend class OmProjectGlobals;

public:
    OmRandColorFile()
        : fnp_(fileName())
        , values_(NULL)
        , numEntries_(0)
    {}

    ~OmRandColorFile()
    {}

    OmColor GetRandomColor()
    {
        assert(values_);
        const int index = OmRand::GetRandomInt(0, numEntries_-1);
        return values_[index];
    }

private:
    void createOrLoad()
    {
        if(!om::file::exists(fnp_)){
            setupFile();
        }
        mapReadOnly();
    }

    void mapReadOnly()
    {
        if(!om::file::exists(fnp_)){
            throw OmIoException("file doesn't exist", fnp_);
        }

        om::file::openFileRO(file_, fnp_);

        values_ = om::file::mapFile<OmColor>(file_.get());

        numEntries_ = file_->size() / sizeof(OmColor);
    }

    std::string fileName() const
    {
        std::stringstream s;
        s << OmFileNames::GetRandColorFileName();
        s << ".ver";
        s << version_;
        return s.str();
    }

    static void buildColorTable(std::vector<OmColor>& colorTable)
    {
        // make sure to change version_ if color table algorithm changes...

        static const int min_variance = 120;

        colorTable.clear();
        colorTable.reserve(1952449); // 1,952,448 entries for min_var = 120

        for(int r = 0; r < 128; ++r){
            for(int g = 0; g < 128; ++g){
                for(int b = 0; b < 128; ++b){

                    const int avg  = ( r + g + b ) / 3;
                    const int avg2 = ( r*r + g*g + b*b ) / 3;
                    const int v = avg2 - avg*avg;

                    if(v >= min_variance){
                        const OmColor color = {r, g, b};
                        colorTable.push_back(color);
                    }
                }
            }
        }
    }

    void setupFile()
    {
        std::vector<OmColor> colorTable;

        buildColorTable(colorTable);

        QFile file(QString::fromStdString(fnp_));

        om::file::openFileRW(file);

        om::file::writeVec(file, colorTable);
    }
};

#endif
