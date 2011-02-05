#ifndef TESTS_HPP
#define TESTS_HPP

#include "segment/lowLevel/DynamicForestPool.hpp"
#include "common/omSet.hpp"
#include "cache/lockedObjectsTests.hpp"
#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/omDataPaths.h"
#include "segment/segmentTests.hpp"
#include "src/common/omCommon.h"
#include "src/utility/image/omImage.hpp"
#include "tests/datalayer/omMeshChunkAllocTableTests.hpp"
#include "tests/datalayer/vecInFileTests.hpp"
#include "tests/testUtils.hpp"
#include "utility/omRand.hpp"
#include "utility/omTimer.hpp"
#include "view2d/omPointsInCircle.hpp"
#include "volume/omChannel.h"
#include "volume/omMipVolume.h"
#include "volume/omSegmentation.h"
#include "volume/omVolCoordsMipped.hpp"

#include <QTextStream>

class Tests{
    const bool runPerfTests_;
    const bool justRunSegments_;

public:
    Tests()
        : runPerfTests_(ZiARG_perf)
        , justRunSegments_(ZiARG_segments)
    {
        setupTestProject();
    }

    ~Tests(){
        OmProject::Close();
    }

    void Run()
    {
        setDiffTests();

        //dynamicForest();

        return;

        // imageResize();

        {
            VecInFileTests vif;
            vif.RunAll();

            OmMeshChunkAllocTableTests mcatt;
            mcatt.RunAll();
        }


        {
            LockedObjectsTests lot;
            lot.RunAll();
            printf("locked objects tests OK!\n\n");
        }

        if(justRunSegments_){
            SegmentTests segTests;
            segTests.RunAll();
            return;
        }

        DataToMipCoordTest();
        rounding();
        powersOf2();
        if(runPerfTests_){
            mapTests();
        }
        hdf5Paths();
        pointsInCircle();

        printf("done\n");
    }

private:
    void imageResize()
    {
        uint32_t data[] = {
            0,1,
            2,3,
            4,5,
            6,7
        };

        OmImage<uint32_t, 3> d(OmExtents
                               [2]
                               [2]
                               [2],
                               data);

        verify(8 == d.size());
        const uint32_t* ptr = d.getScalarPtr();
        for(uint32_t i = 0; i <d.size(); ++i){
            printf("%d: %d\n", i, ptr[i]);
            verify(ptr[i] == i);
        }

        d.resize(Vector3i(4,4,4));

        const uint32_t* ptr2 = d.getScalarPtr();
        for(uint32_t i = 0; i <d.size(); ++i){
            printf("%d: %d\n", i, ptr2[i]);
        }

    }

    void rounding()
    {
        // corner case?
        //verify( 1 == ROUNDUP(0,1));

        verify( 10 == ROUNDUP(1,10) );
        verify( 0 == ROUNDUP(0,10));

        verify(128 == ROUNDUP(127,128));

        verify(0 == ROUNDDOWN(5,10));
        verify(10 == ROUNDDOWN(11,10));
        verify(128 == ROUNDDOWN(129,128));

        // ROUNDDOWN and ROUNDUP not yet valid for negative numbers...
        /*
          std::cout << ROUNDDOWN(-8,128) << "\n";
          std::cout << -8 % 128 << "\n";
          std::cout << abs(-8) % 128 << "\n";
          verify(-128 == ROUNDDOWN(-8,128));
          verify(0 == ROUNDUP(-8,128));
        */

        printf("rounding OK\n");
    }

    void DataToMipCoordTest()
    {
        const Vector3i chunkDims(128,128,128);

        verify(OmChunkCoord(0,0,0,0) ==
               OmMipVolCoords::DataToMipCoord(DataCoord(1,1,1), 0, chunkDims));

        verify(OmChunkCoord(0,1,1,1) ==
               OmMipVolCoords::DataToMipCoord(DataCoord(129,129,129), 0, chunkDims));

        verify(OmChunkCoord(1,0,0,0) ==
               OmMipVolCoords::DataToMipCoord(DataCoord(255,255,255), 1, chunkDims));

        verify(OmChunkCoord(1,1,1,1) ==
               OmMipVolCoords::DataToMipCoord(DataCoord(256,256,256), 1, chunkDims));

        verify(OmChunkCoord(2,0,0,0) ==
               OmMipVolCoords::DataToMipCoord(DataCoord(511,511,511), 2, chunkDims));

        verify(OmChunkCoord(2,1,1,1) ==
               OmMipVolCoords::DataToMipCoord(DataCoord(512,512,512), 2, chunkDims));

        verify(OmChunkCoord::NULL_COORD ==
               OmMipVolCoords::DataToMipCoord(DataCoord(-8, 0, 0), 0, chunkDims));

        const Vector3<uint64_t> chunkDims64(128,128,128);
        verify(OmChunkCoord(0,1,1,1) ==
               OmMipVolCoords::DataToMipCoord(DataCoord(130,128,128), 0, chunkDims64));

        if(runPerfTests_){
            OmTimer timer;
            const uint64_t max = 2000000;
            for(uint64_t i = 0; i < max; ++i){
                verify(OmChunkCoord(2,1,1,1) ==
                       OmMipVolCoords::DataToMipCoord(DataCoord(512,512,512), 2, chunkDims));
            }
            std::cout << max << " conversions in " << timer.s_elapsed() << " secs\n";
        }

        printf("DataToMipCoordTest OK\n");
    }

    void powersOf2()
    {
        for(int i = 0; i < 32; ++i){
            if(std::pow(static_cast<float>(2),
                        static_cast<float>(i)) != om::pow2int(i)){
                std::cout << "pow2 fail at i==" << i
                          << "; was " << om::pow2int(i)
                          << ", but should be: "
                          << std::pow(static_cast<float>(2),
                                      static_cast<float>(i))
                          << "\n";
                verify(0);
            }
        }

        if(runPerfTests_){
            OmTimer timer;
            const uint64_t max = 2000000;
            uint64_t sum = 0;
            for(uint64_t i = 0; i < max; ++i){
                sum += om::pow2int(8);

            }
            std::cout << "\t" << max << " pow2 lookups in " << timer.s_elapsed() << " secs\n";

            timer.restart();
            sum = 0;
            for(uint64_t i = 0; i < max; ++i){
                sum += std::pow(static_cast<double>(2),
                                static_cast<double>(8));
            }
            std::cout << "\t" << max << " std::pow in " << timer.s_elapsed() << " secs\n";
        }

        printf("powersOf2 ok\n");
    }

    void mapTests()
    {
        const uint64_t max = 2000000;

        std::map<int, int> stdMap;
        testMapSpeed(stdMap, max, "std::map");

        boost::unordered_map<int,int> boostMap;
        testMapSpeed(boostMap, max, "boost::unordered_map");
    }

    template <typename T>
    void testMapSpeed(T& map, const uint64_t max,
                      const std::string& mapType)
    {
        OmTimer timer;
        for(uint64_t i = 0; i < max; ++i){
            map[i] = i;

        }
        std::cout << "\t" << max << " inserts into " << mapType
                  << " in " << timer.s_elapsed() << " secs\n";

        timer.restart();
        double sum = 0;
        FOR_EACH(iter, map){
            const int key = iter->first;
            const int val = iter->second;
            sum += key + val;
        }
        std::cout << "\t" << max << " {key,val} walk in " << mapType
                  << " in " << timer.s_elapsed() << " secs\n";

        timer.restart();
        sum = 0;
        for(uint64_t i=0; i < max; ++i){
            sum += map[ OmRand::GetRandomInt(0,max) ];
        }
        std::cout << "\t" << max << " rand gets in " << mapType
                  << " in " << timer.s_elapsed() << " secs\n";

    }

    void hdf5Paths()
    {
        OmSegmentation seg1(1);

        verify("segmentations/segmentation1/" ==
               OmDataPaths::getDirectoryPath(&seg1));

        const QString basePath = OmProject::FilesFolder();
        const QString fullPathToVolData =
            basePath + "/segmentations/segmentation1/0/volume.unknown.raw";
        verify(fullPathToVolData.toStdString() ==
               OmFileNames::GetMemMapFileName(&seg1, 0));

        OmChannel chann1(1);
        verify("channels/channel1/" ==
               OmDataPaths::getDirectoryPath(&chann1));

        printf("hdf5 path tests OK\n");
    }

    void pointsInCircle()
    {
        static bool BrushTool32[33][33] = {
            {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0},
            {0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0},
            {0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
            {0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0},
            {0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0},
            {0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
            {0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
            {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
            {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
            {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
            {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
            {0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
            {0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
            {0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0},
            {0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0},
            {0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
            {0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0},
            {0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
        };

        const int r = 16;

        OmPointsInCircle pc;
        const std::vector<om::point2d>& pointsVec = pc.GetPtsInCircle(r);

        std::set<om::point2d> points;
        FOR_EACH(iter, pointsVec){
            points.insert(*iter);
        }

        for(int x = 0; x < 33; ++x){
            for(int y = 0; y < 33; ++y){
                om::point2d fp = { x, y };
                if(BrushTool32[x][y]){
                    verify(points.count(fp));
                } else {
                    verify(!points.count(fp));
                }
            }
        }

        // (mostly) generate above circle
        if(0){
            for( int y = 2*r; y >= 0; --y){
                for(int x =0; x <= 2*r; ++x){
                    om::point2d fp = { x, y };
                    if(points.count(fp)){
                        printf("1,");
                    }else{
                        printf("0,");
                    }
                }
                printf("\n");
            }
        }

        printf("points in circle OK\n");
    }

    void setupTestProject()
    {
        const QString path = "/tmp/omni_test_project.omni";
        OmProject::New(path);
    }

    void setDiffTests()
    {
        {
            std::set<int> a;
            for(int i = 0; i < 100; ++i){
                a.insert(i);
            }

            std::set<int> b;
            for(int i = 200; i < 300; ++i){
                b.insert(i);
            }

            verify(false == om::sets::SetAContainsB(a, b));
            verify(true == om::sets::SetsAreDisjoint(a, b));
        }
        {
            std::set<int> a;
            for(int i = 0; i < 100; ++i){
                a.insert(i);
            }

            std::set<int> b;
            for(int i = 50; i < 90; ++i){
                b.insert(i);
            }

            verify(true == om::sets::SetAContainsB(a, b));
            verify(false == om::sets::SetsAreDisjoint(a, b));

            b.insert(200);
            verify(false == om::sets::SetAContainsB(a, b));
            verify(false == om::sets::SetsAreDisjoint(a, b));
        }
        std::cout << "set tests OK\n";
    }

    void dynamicForest()
    {
        zi::DynamicForestPool<uint32_t> forest(1000);

        for(int i = 1; i < 1000; ++i){
            std::cout << i << ", " << forest.root(i) << "\n";
        }

        std::cout << "dynamicForest\n";
    }
};

#endif
