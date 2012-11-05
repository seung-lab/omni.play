#pragma once

#include "datalayer/hdf5/omExportVolToHdf5.hpp"
#include "actions/io/omActionDumper.h"
#include "actions/omActions.h"
#include "chunks/omRawChunk.hpp"
#include "chunks/uniqueValues/omChunkUniqueValuesManager.hpp"
#include "chunks/uniqueValues/omChunkUniqueValuesTypes.h"
#include "mesh/omMeshParams.hpp"
#include "project/omProject.h"
#include "segment/io/omMST.h"
#include "segment/io/omUserEdges.hpp"
#include "segment/omSegmentCenter.hpp"
#include "segment/omSegmentUtils.hpp"
#include "utility/dataWrappers.h"
#include "utility/color.hpp"
#include "utility/omFileHelpers.h"
#include "volume/build/omBuildSegmentation.hpp"
#include "volume/build/omDataCopierImages.hpp"
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
        bs.BuildAndMeshSegmentation();
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
        bs.BuildImage();

        OmProject::Save();
    }

    static void loadHDF5seg(const QString file, om::common::ID& segmentationID_)
    {
        OmBuildSegmentation bs;
        bs.addFileNameAndPath(file);
        bs.BuildImage();

        segmentationID_ = bs.GetDataWrapper().GetID();
    }

    static void ClearMST(const om::common::ID segmentationID)
    {
        SegmentationDataWrapper sdw(segmentationID);

        OmMST* mst = sdw.MST();
        OmMSTEdge* edges = mst->Edges();

        for(uint32_t i = 0; i < mst->NumEdges(); ++i)
        {
            edges[i].userSplit = 0;
            edges[i].userJoin = 0;
            edges[i].wasJoined = 0;
        }

        mst->Flush();

        sdw.GetSegmentation().MSTUserEdges()->Clear();
        sdw.GetSegmentation().MSTUserEdges()->Save();

        OmSegments* segments = sdw.Segments();

        for(om::common::SegID i = 1; i <= segments->getMaxValue(); ++i)
        {
            OmSegment* seg = segments->GetSegment(i);
            if(!seg){
                continue;
            }
            seg->SetListType(om::common::WORKING);
        }

        sdw.ValidGroupNum()->Clear();
        sdw.ValidGroupNum()->Save();

        OmActions::Save();
    }

    static void RecolorAllSegments(const om::common::ID segmentationID)
    {
        SegmentationDataWrapper sdw(segmentationID);

        OmSegments* segments = sdw.Segments();

        for(om::common::SegID i = 1; i <= segments->getMaxValue(); ++i)
        {
            OmSegment* seg = segments->GetSegment(i);
            if(!seg){
                continue;
            }
            seg->RandomizeColor();
        }

        OmActions::Save();
    }

    static void RebuildCenterOfSegmentData(const om::common::ID segmentationID)
    {
        SegmentationDataWrapper sdw(segmentationID);
        OmSegmentCenter::RebuildCenterOfSegmentData(sdw);
        OmActions::Save();
    }

    template <typename T>
    static void ChangeVolResolution(T& vol,
                                    const int xRes,
                                    const int yRes,
                                    const int zRes)
    {
        const Vector3i dims(xRes, yRes, zRes);

        vol.Coords().SetResolution(dims);

        std::cout << "\tvolume data resolution set to "
                  << vol.Coords().GetResolution()
                  << "\n";
    }

    template <typename T>
    static void ChangeVolAbsOffset(T& vol,
                                   const int xRes,
                                   const int yRes,
                                   const int zRes)
    {
        const Vector3i dims(xRes, yRes, zRes);

        vol.Coords().SetAbsOffset(dims);

        std::cout << "\tvolume data abs offset set to "
                  << vol.Coords().AbsOffset()
                  << "\n";
    }

    static void SetMeshDownScallingFactor(const double factor)
    {
        OmMeshParams::SetDownScallingFactor(factor);
        std::cout << "mesh downscalling factor set to "
                  << OmMeshParams::GetDownScallingFactor()
                  << "\n";
    }

    static void ReValidateEveryObject(const om::common::ID segmentationID)
    {
        SegmentationDataWrapper sdw(segmentationID);
        OmSegmentUtils::ReValidateEveryObject(sdw);
        OmActions::Save();
    }

    static void DumpSegmentColorHistograms(const om::common::ID segmentationID){
        dumpRootSegmentColorHistograms(segmentationID, false);
    }

    static void DumpRootSegmentColorHistograms(const om::common::ID segmentationID){
        dumpRootSegmentColorHistograms(segmentationID, true);
    }

private:
    static void dumpRootSegmentColorHistograms(const om::common::ID segmentationID,
                                               const bool findRoot)
    {
        SegmentationDataWrapper sdw(segmentationID);

        std::map<OmColor, int> segColorHist;

        static const int min_variance = 120;

        for(uint8_t r = 0; r < 128; ++r){
            for(uint8_t g = 0; g < 128; ++g){
                for(uint8_t b = 0; b < 128; ++b){
                    const int avg  = ( r + g + b ) / 3;
                    const int avg2 = ( r*r + g*g + b*b ) / 3;
                    const int v = avg2 - avg*avg;
                    if(v >= min_variance){
                        const om::common::Color color = {r, g, b};
                        segColorHist[color] = 0;
                    }
                }
            }
        }

        std::cout << "found "
                  << om::string::humanizeNum(segColorHist.size())
                  << " colors\n";

        OmSegments* segments = sdw.Segments();
        for(om::common::SegID i = 1; i <= segments->getMaxValue(); ++i){
            OmSegment* seg = segments->GetSegment(i);
            if(!seg){
                continue;
            }
            if(findRoot){
                seg = segments->findRoot(seg);
            }
            const om::common::Color color = seg->GetColorInt();

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
            throw om::IoException("could not open file", outFile);
        }

        QTextStream out(&data);

        out << "red\tgreen\tblue\tnum\n";

        FOR_EACH(iter, segColorHist){
            out << iter->first << "\t"
                << iter->second << "\n";
        }
    }

public:

    static void TimeSegChunkReads(const om::common::ID segmentationID,
                                  const bool randomize,
                                  const bool useMeshChunk)
    {
        SegmentationDataWrapper sdw(segmentationID);
        OmSegmentation& vol = sdw.GetSegmentation();

        assert(4 == vol.GetBytesPerVoxel());

        double timeSecs = 0;

        boost::shared_ptr<std::deque<om::coords::Chunk> > coordsPtr =
            vol.MipChunkCoords();
        std::deque<om::coords::Chunk>& coords = *coordsPtr;
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

        const Vector3i chunkDims = vol.Coords().ChunkDimensions();
        const double totalMegs =
            static_cast<double>(chunkDims.x) *
            static_cast<double>(chunkDims.y) *
            static_cast<double>(chunkDims.z) *
            static_cast<double>(vol.GetBytesPerVoxel()) *
            static_cast<double>(numChunks) /
            static_cast<double>(om::math::bytesPerMB);
        const double megsPerSec = totalMegs / timeSecs;

        std::cout << "raw chunk read ";
        if(useMeshChunk){
            std::cout << "(fseek and read): ";
        } else {
            std::cout << "(get copy of whole chunk using mesh reader): ";
        }

        std::cout << megsPerSec << " MB/sec\n";
    }

    static void Mesh(const om::common::ID segmentationID)
    {
        const SegmentationDataWrapper sdw(segmentationID);
        OmBuildSegmentation bs(sdw);
        bs.BuildMesh();
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

    static void DumpActionsToTextFile()
    {
        OmActionDumper dumper;
        const QString fnp("/tmp/actionDump.txt");
        dumper.Dump(fnp);
        std::cout << "wrote action log to " << fnp.toStdString() << "\n";
    }

    static void DownsampleChannel(const ChannelDataWrapper& cdw)
    {
        OmChannel* vol = cdw.GetChannelPtr();
        vol->VolData()->downsample(vol);
    }

    static void CheckMeshes(const SegmentationDataWrapper& sdw)
    {
        OmMeshManagers* meshManagers = sdw.GetSegmentation().MeshManagers();
        OmMeshManager* meshManager = meshManagers->GetManager(1);

        boost::scoped_ptr<OmMeshWriterV2> meshWriter(new OmMeshWriterV2(meshManager));

        meshWriter->CheckEverythingWasMeshed();
    }

    template <typename VOL>
    static void ReplaceSlice(VOL* vol, const QString fnp, const int sliceNum)
    {
        std::vector<QFileInfo> file;
        file.push_back(QFileInfo(fnp));

        OmDataCopierImages<VOL> importer(vol, file);
        importer.ReplaceSlice(sliceNum);
    }

    static void ExportAndRerootSegments(const SegmentationDataWrapper& sdw,
                                        const QString fileName)
    {
        OmExportVolToHdf5::Export(sdw.GetSegmentationPtr(), fileName, true);
    }

    static void ExportSegmentationRaw(const SegmentationDataWrapper& sdw,
                                      const QString fileName)
    {
        OmExportVolToHdf5::Export(sdw.GetSegmentationPtr(), fileName, false);
    }

};
