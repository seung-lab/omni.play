#pragma once

#include "common/common.h"

/**
 * metadata file format:
 *   segmentation: seg.raw 32 123 123 123
 *   dendrogram: dend.raw 32 1
 **/

class OmWatershedMetadata {
private:
    QString fnp_;

    QStringList lines_;
    int segmentationCounter_;
    int dendCounter_;

    struct SegmentationLine{
        QString fnp;
        int bpp;
        int xSize;
        int ySize;
        int zSize;
    };
    std::map<int, SegmentationLine> segmentationFiles_;

    struct DendLine{
        QString fnp;
        int bpp;
        int numEdges;
    };
    DendLine dendLine_;

public:
    OmWatershedMetadata()
        : segmentationCounter_(0)
        , dendCounter_(0)
    {}

    void ReadMetata(const QString& fnp)
    {
        fnp_ = fnp;

        readFile();
        extractMetadata();
    }

    Vector3i GetMip0Dims() const
    {
        if(!segmentationFiles_.count(0)){
            throw om::IoException("no mip zero vol defined");
        }
        const SegmentationLine& segLine = segmentationFiles_.at(0);

        return Vector3i(segLine.xSize,
                        segLine.ySize,
                        segLine.zSize);
    }

    int GetColorDepth() const
    {
        if(!segmentationFiles_.count(0)){
            throw om::IoException("no mip zero vol defined");
        }
        const SegmentationLine& segLine = segmentationFiles_.at(0);

        return segLine.bpp;
    }

    std::vector<int> GetMipLevels() const
    {
        std::vector<int> mipLevels;

        FOR_EACH(iter, segmentationFiles_){
            const int mipLevel = iter->first;
            mipLevels.push_back(mipLevel);
        }

        return mipLevels;
    }

    QString GetMipLevelFileName(const int mipLevel) const {
        return segmentationFiles_.at(mipLevel).fnp;
    }

    QString MstFileName() const
    {
        if(!dendCounter_){
            throw om::IoException("no MST found");
        }

        return dendLine_.fnp;
    }

    int MstBitsPerNode() const
    {
        if(!dendCounter_){
            throw om::IoException("no MST found");
        }

        return dendLine_.bpp;
    }

    int MstNumEdges() const
    {
        if(!dendCounter_){
            throw om::IoException("no MST found");
        }

        return dendLine_.numEdges;
    }

private:
    void extractMetadata()
    {
        FOR_EACH(iter, lines_){
            parseLine(*iter);
        }
    }

    void parseLine(const QString& line)
    {
        const QStringList tokens = line.split(':', QString::SkipEmptyParts);

        if(!tokens.size()){
            return;
        }

        if(2 != tokens.size()){
            throw om::IoException("invalid line", line);
        }

        if("segmentation" == tokens[0]){
            parseLineSegmentation(tokens[1]);

        } else if("dendrogram" == tokens[0]){
            parseLineDend(tokens[1]);

        } else {
            throw om::IoException("invalid line", line);
        }
    }

    // example:  segmentation: seg.raw 32 123 123 123
    void parseLineSegmentation(const QString& tokens)
    {
        const QStringList args = tokens.split(' ', QString::SkipEmptyParts);
        if(5 != args.size()){
            throw om::IoException("invalid line args", tokens);
        }

        SegmentationLine segInfo = { args[0],
                                     OmStringHelpers::getInt(args[1]),
                                     OmStringHelpers::getInt(args[2]),
                                     OmStringHelpers::getInt(args[3]),
                                     OmStringHelpers::getInt(args[4]) };

        segmentationFiles_[segmentationCounter_++] = segInfo;
    }

    // example:   dendrogram: dend.raw 32 1
    void parseLineDend(const QString& tokens)
    {
        const QStringList args = tokens.split(' ', QString::SkipEmptyParts);
        if(3 != args.size()){
            throw om::IoException("invalid line args", tokens);
        }

        if(dendCounter_){
            throw om::IoException("more than one dend found");
        }

        DendLine dendInfo = { args[0],
                              OmStringHelpers::getInt(args[1]),
                              OmStringHelpers::getInt(args[2]) };

        dendLine_ = dendInfo;

        ++dendCounter_;
    }

    void readFile()
    {
        QFile file(fnp_);
        if(!file.open(QIODevice::ReadOnly)){
            throw om::IoException("could not open", fnp_);
        }

        QTextStream in(&file);

        while(1){
            const QString line = in.readLine();
            if(NULL == line ||
               ""   == line ||
               line.startsWith("#"))
            {
                break;
            }

            lines_ << line;
        }
    }
};

