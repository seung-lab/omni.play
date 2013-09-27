#pragma once

#include "volume/omSegmentationFolder.h"
#include "segment/omSegmentEdge.h"
#include "volume/omSegmentation.h"
#include "datalayer/fs/omFileNames.hpp"

#include <QList>
#include <QFile>
#include <QDir>

class OmUserEdges {
private:
    // version 1: initial move from HDF5
    static const int CurrentFileVersion = 1;

    OmSegmentation *const vol_;
    QList<OmSegmentEdge> edges_;

public:
    OmUserEdges(OmSegmentation* vol)
        : vol_(vol)
    {}

    void Load()
    {
        const QString path = filePathQStr();

        QFile file(path);

        if(!file.open(QIODevice::ReadOnly)){
            throw om::IoException("error reading file");
        }

        QDataStream in(&file);
        in.setByteOrder( QDataStream::LittleEndian );
        in.setVersion(QDataStream::Qt_4_6);

        int version;
        in >> version;

        if(CurrentFileVersion != version){
            throw om::IoException("versions differ");
        }

        int size;
        in >> size;
        for(int i = 0; i < size; ++i){
            OmSegmentEdge e;
            in >> e;
            AddEdgeFromProjectLoad(e);
        }

        printf("loaded %d user edges\n", edges_.size());

        if(!in.atEnd()){
            throw om::IoException("corrupt file?");
        }
    }

    void Save()
    {
        const QString path = filePathQStr();

        QFile file(path);

        if (!file.open(QIODevice::WriteOnly)) {
            throw om::IoException("could not write file");
        }

        QDataStream out(&file);
        out.setByteOrder( QDataStream::LittleEndian );
        out.setVersion(QDataStream::Qt_4_6);

        out << CurrentFileVersion;

        int size = edges_.size();
        out << size;
        FOR_EACH(iter, edges_){
            out << *iter;
        }

        printf("saved %s\n", qPrintable(path));
    }

    void AddEdgeFromProjectLoad(OmSegmentEdge e)
    {
        if(0 == e.childID  ||
           0 == e.parentID ||
           std::isnan(e.threshold)){
            printf("warning: bad edge found: %d, %d, %f\n",
                   e.parentID,
                   e.childID,
                   e.threshold);
            return;
        }
        e.valid = true;
        edges_.push_back(e);
    }

    void AddEdge(const OmSegmentEdge& e){
        edges_.push_back(e);
    }

    int RemoveEdge(const OmSegmentEdge& edge){
        return edges_.removeAll(edge);
    }

    QList<OmSegmentEdge>& Edges(){
        return edges_;
    }

    void Clear(){
        edges_.clear();
    }

private:
    QString filePathQStr()
    {
        return QString::fromStdString(
            vol_->Folder()->GetVolSegmentsPathAbs("mstUserEdges.data")
            );
    }
};

