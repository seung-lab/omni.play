#ifndef HEADLESS_IMPL_HPP
#define HEADLESS_IMPL_HPP

#include "actions/omActions.h"
#include "chunks/omRawChunk.hpp"
#include "chunks/uniqueValues/omChunkUniqueValuesManager.hpp"
#include "chunks/uniqueValues/omChunkUniqueValuesTypes.h"
#include "mesh/omMeshParams.hpp"
#include "project/omProject.h"
#include "segment/io/omMST.h"
#include "segment/io/omUserEdges.hpp"
#include "segment/omSegmentLists.hpp"
#include "segment/omSegmentUtils.hpp"
#include "utility/dataWrappers.h"
#include "utility/omColorUtils.hpp"
#include "utility/omFileHelpers.h"
#include "volume/build/omBuildSegmentation.hpp"
#include "volume/build/omVolumeBuilder.hpp"
#include "volume/omSegmentation.h"

class HeadlessImpl {
public:
    static void OpenProject(const QString& fNameIn)
    {
        if("" == fNameIn){
            printf("no project file name\n");
            return;
        }

        QString fName = fNameIn;
        if(!fNameIn.endsWith(".omni")){
            fName = fNameIn + ".omni";
        }

        try {
            printf("Please wait: Opening project \"%s\"...\n", qPrintable(fName));
            OmProject::Load(fName);
            //Set current working directory to file path
            QDir::setCurrent(QFileInfo(fName).absolutePath());
            printf("Opened project \"%s\"\n", qPrintable(fName));
        } catch(...) {
            printf("Error while loading project \"%s\"\n", qPrintable(fName));
        }
    }

    static void buildHDF5(const QString file)
    {
        QString projectFileName = QFileInfo(file+".omni").fileName();

        OmProject::New(projectFileName);

        OmBuildSegmentation bs;
        bs.addFileNameAndPath(file);
        bs.BuildAndMeshSegmentation(om::BLOCKING);
    }

    static void importHDF5seg(const std::string& file) {
        importHDF5seg(QString::fromStdString(file));
    }

    static void importHDF5seg(const QString& file)
    {
        QString projectFileName = QFileInfo(file+".omni").fileName();

        OmProject::New(projectFileName);

        OmBuildSegmentation bs;
        bs.addFileNameAndPath(file);
        bs.BuildImage(om::BLOCKING);
    }

    static void loadHDF5seg(const QString file, OmID& segmentationID_)
    {
        OmBuildSegmentation bs;
        bs.addFileNameAndPath(file);
        bs.BuildImage(om::BLOCKING);

        segmentationID_ = bs.GetDataWrapper().getID();
    }

    static void ClearMST(const OmID segmentationID)
    {
        SegmentationDataWrapper sdw(segmentationID);

        OmMST* mst = sdw.MST();
        OmMSTEdge* edges = mst->Edges();

        for(uint32_t i = 0; i < mst->NumEdges(); ++i){
            edges[i].userSplit = 0;
            edges[i].userJoin = 0;
            edges[i].wasJoined = 0;
        }

        mst->Flush();

        sdw.GetSegmentation().MSTUserEdges()->Clear();
        sdw.GetSegmentation().MSTUserEdges()->Save();

        OmSegmentCache* segCache = sdw.SegmentCache();
        for(OmSegID i = 1; i <= segCache->getMaxValue(); ++i){
            OmSegment* seg = segCache->GetSegment(i);
            if(!seg){
                continue;
            }
            seg->SetListType(om::WORKING);
        }

        OmActions::Save();
    }

    static void RecolorAllSegments(const OmID segmentationID)
    {
        SegmentationDataWrapper sdw(segmentationID);

        OmSegmentCache* segCache = sdw.SegmentCache();
        for(OmSegID i = 1; i <= segCache->getMaxValue(); ++i){
            OmSegment* seg = segCache->GetSegment(i);
            if(!seg){
                continue;
            }
            seg->RandomizeColor();
        }

        OmActions::Save();
    }

    static void RebuildCenterOfSegmentData(const OmID segmentationID)
    {
        SegmentationDataWrapper sdw(segmentationID);
        OmSegmentUtils::RebuildCenterOfSegmentData(sdw);
        OmActions::Save();
    }

    template <typename T>
    static void ChangeVolResolution(T& vol,
                                    const float xRes,
                                    const float yRes,
                                    const float zRes)
    {
        const Vector3f dims(xRes, yRes, zRes);

        vol.Coords().SetDataResolution(dims);

        std::cout << "\tvolume data resolution set to "
                  << vol.Coords().GetDataResolution()
                  << "\n";
    }

    static void SetMeshDownScallingFactor(const double factor)
    {
        OmMeshParams::SetDownScallingFactor(factor);
        std::cout << "mesh downscalling factor set to "
                  << OmMeshParams::GetDownScallingFactor()
                  << "\n";
    }

    static void ReValidateEveryObject(const OmID segmentationID)
    {
        SegmentationDataWrapper sdw(segmentationID);

     	OmSegmentListContainer<OmSegmentListBySize>& validList =
            sdw.SegmentLists()->Valid();

        OmSegIDsSet allSegIDs = validList.List().AllSegIDs();

        FOR_EACH(iter, allSegIDs){
            SegmentDataWrapper sdw(segmentationID, *iter);
            OmActions::ValidateSegment(sdw, om::SET_VALID);
        }

        OmActions::Save();
    }

    static void DumpSegmentColorHistograms(const OmID segmentationID){
        dumpRootSegmentColorHistograms(segmentationID, false);
    }

    static void DumpRootSegmentColorHistograms(const OmID segmentationID){
        dumpRootSegmentColorHistograms(segmentationID, true);
    }

private:
    static void dumpRootSegmentColorHistograms(const OmID segmentationID,
                                               const bool findRoot)
    {
        SegmentationDataWrapper sdw(segmentationID);

        std::map<OmColor, int> segColorHist;

        static const int min_variance = 120;

        for(int r = 0; r < 128; ++r){
            for(int g = 0; g < 128; ++g){
                for(int b = 0; b < 128; ++b){
                    const int avg  = ( r + g + b ) / 3;
                    const int avg2 = ( r*r + g*g + b*b ) / 3;
                    const int v = avg2 - avg*avg;
                    if(v >= min_variance){
                        const OmColor color = {r, g, b};
                        segColorHist[color] = 0;
                    }
                }
            }
        }

        std::cout << "found "
                  << om::string::humanizeNum(segColorHist.size())
                  << " colors\n";

        OmSegmentCache* segCache = sdw.SegmentCache();
        for(OmSegID i = 1; i <= segCache->getMaxValue(); ++i){
            OmSegment* seg = segCache->GetSegment(i);
            if(!seg){
                continue;
            }
            if(findRoot){
                seg = segCache->findRoot(seg);
            }
            const OmColor color = seg->GetColorInt();

            ++(segColorHist[color]);
        }

        QString outFile("/tmp/segmentColors.txt");
        if(findRoot){
            outFile = "/tmp/segmentRootColors.txt";
        }

        QFile data(outFile);
        if(data.open(QFile::WriteOnly | QFile::Truncate)) {
            printf("writing segment file %s\n", qPrintable(outFile));
        } else{
            throw OmIoException("could not open file", outFile);
        }

        QTextStream out(&data);

        out << "red\tgreen\tblue\tnum\n";

        FOR_EACH(iter, segColorHist){
            out << iter->first << "\t"
                << iter->second << "\n";
        }
    }

public:

    static void TimeSegChunkReads(const OmID segmentationID,
                                  const bool randomize,
                                  const bool useMeshChunk)
    {
        SegmentationDataWrapper sdw(segmentationID);
        OmSegmentation& vol = sdw.GetSegmentation();

        assert(4 == vol.GetBytesPerVoxel());

        double timeSecs = 0;

        boost::shared_ptr<std::deque<OmChunkCoord> > coordsPtr =
            vol.GetMipChunkCoords();
        std::deque<OmChunkCoord>& coords = *coordsPtr;
        const uint32_t numChunks = coords.size();

        if(randomize){
            zi::random_shuffle(coords.begin(), coords.end());
        }

        for(uint32_t i = 0; i < numChunks; ++i){
            printf("\rreading chunk %d of %d...", i, numChunks);
            fflush(stdout);

            OmTimer timer;
            if(useMeshChunk){
                OmRawChunk<uint32_t> chunk(&vol, coords[i]);
            }else{
                assert(0 && "fixme!");
            }
            timeSecs += timer.s_elapsed();
        }

        const Vector3i chunkDims = vol.Coords().GetChunkDimensions();
        const double totalMegs =
            static_cast<double>(chunkDims.x) *
            static_cast<double>(chunkDims.y) *
            static_cast<double>(chunkDims.z) *
            static_cast<double>(vol.GetBytesPerVoxel()) *
            static_cast<double>(numChunks) /
            static_cast<double>(BYTES_PER_MB);
        const double megsPerSec = totalMegs / timeSecs;

        std::cout << "raw chunk read ";
        if(useMeshChunk){
            std::cout << "(fseek and read): ";
        } else {
            std::cout << "(get copy of whole chunk using mesh reader): ";
        }

        std::cout << megsPerSec << " MB/sec\n";
    }

    static void RefindUniqueChunkValues(const OmID segmentationID_)
    {
        OmTimer timer;

        SegmentationDataWrapper sdw(segmentationID_);
        OmSegmentation& vol = sdw.GetSegmentation();

        OmFileHelpers::RemoveDir(OmFileNames::GetChunksFolder(&vol));

        boost::shared_ptr<std::deque<OmChunkCoord> > coordsPtr =
            vol.GetMipChunkCoords();

        const uint32_t numChunks = coordsPtr->size();

        int counter = 0;
        FOR_EACH(iter, *coordsPtr){
            const OmChunkCoord& coord = *iter;

            ++counter;
            printf("\rfinding values in chunk %d of %d...", counter, numChunks);
            fflush(stdout);

            OmTimer timer;
            const ChunkUniqueValues segIDs =
                vol.ChunkUniqueValues()->Values(coord, 1);

            const double time = timer.s_elapsed();
            std::cout << " (done in " << time << " secs, "
                      << 1. / time << " chunks per sec)"
                      << std::flush;

            vol.ChunkUniqueValues()->Clear();
        }

        timer.PrintDone();
    }

    static void Mesh(const OmID segmentationID)
    {
        const SegmentationDataWrapper sdw(segmentationID);
        OmBuildSegmentation bs(sdw);
        bs.BuildMesh(om::BLOCKING);
    }

    static void ImportWatershed(const QString& fnp)
    {
        SegmentationDataWrapper sdw;
        OmSegmentation& segmentation = sdw.Create();

        std::vector<QFileInfo> files;
        files.push_back(QFileInfo(fnp));

        OmVolumeBuilder<OmSegmentation> builder(&segmentation, files);
        builder.BuildWatershed();

        OmActions::Save();
    }
};

#endif
