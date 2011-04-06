#ifndef OM_TILE_POOL_TESTS_HPP
#define OM_TILE_POOL_TESTS_HPP

#include "src/tiles/omTilePool.hpp"
#include "tests/testUtils.hpp"

class OmTilePoolTests {
private:
    static const uint32_t numTiles_ = 20000;
    static const uint32_t numRounds_ = 200;

public:
    OmTilePoolTests()
    {}

    void Run()
    {
        test1();
        test2();
        test3();
        std::cout << "tile pool tests OK\n";
    }

private:
    void test1()
    {
        const uint32_t numTiles_ = 20000;

        OmTimer timer;
        {
            OmTilePool<uint32_t> pool(20, 128*128);

            std::deque<uint32_t*> tiles;

            for(uint32_t i = 0; i < numTiles_; ++i)
            {
                uint32_t* tile = pool.GetTile();
                verify(tile);
                tiles.push_back(tile);
            }

            // verify(numTiles_ == pool.NumTilesInUse());
        }
        timer.Print("tile pool test1");

        timer.reset();
        {
            std::list<boost::shared_ptr<uint32_t> > tiles;

            for(uint32_t i = 0; i < numTiles_; ++i)
            {
                boost::shared_ptr<uint32_t> tile =
                    OmSmartPtr<uint32_t>::MallocNumElements(128*128,
                                                            om::DONT_ZERO_FILL);
                verify(tile);
                tiles.push_back(tile);
            }
        }
        timer.Print("shared_ptr test1");
    }

    void getAndReturnTiles(OmTilePool<uint32_t>* poolPtr)
    {
        OmTilePool<uint32_t>& pool = *poolPtr;

        std::deque<uint32_t*> tiles;

        for(uint32_t round = 0; round < numRounds_; ++round)
        {
            for(uint32_t i = 0; i < numTiles_; ++i)
            {
                uint32_t* tile = pool.GetTile();
                verify(tile);
                tiles.push_back(tile);
            }
            for(uint32_t i = 0; i < numTiles_; ++i)
            {
                uint32_t* tile = tiles.front();
                pool.FreeTile(tile);

                tiles.pop_front();
            }
        }
    }

    void getAndReturnTiles()
    {
        std::list<boost::shared_ptr<uint32_t> > tiles;

        for(uint32_t round = 0; round < numRounds_; ++round)
        {
            for(uint32_t i = 0; i < numTiles_; ++i)
            {
                boost::shared_ptr<uint32_t> tile =
                    OmSmartPtr<uint32_t>::MallocNumElements(128*128,
                                                            om::DONT_ZERO_FILL);
                verify(tile);
                tiles.push_back(tile);
            }
            for(uint32_t i = 0; i < numTiles_; ++i)
            {
                tiles.pop_front();
            }
        }
    }

    void test2()
    {
        OmTimer timer;
        {
            OmTilePool<uint32_t> pool(20, 128*128);
            getAndReturnTiles(&pool);
        }
        timer.Print("tile pool test2");

        timer.reset();
        {
            getAndReturnTiles();
        }
        timer.Print("shared_ptr test2");
    }

    void test3()
    {
        const int numTasks = 15;

        OmTimer timer;
        {
            OmTilePool<uint32_t> tilePool(20, 128*128);

            OmThreadPool threadPool;
            threadPool.start();

            for(int i = 0; i < numTasks; ++i)
            {
                threadPool.push_back(
                    zi::run_fn(
                        zi::bind(&OmTilePoolTests::getAndReturnTiles,
                                 this, &tilePool)));
            }
            threadPool.join();
        }
        timer.Print("tile pool test3");

        timer.reset();
        {
            OmThreadPool threadPool;
            threadPool.start();

            for(int i = 0; i < numTasks; ++i)
            {
                threadPool.push_back(
                    zi::run_fn(
                        zi::bind(&OmTilePoolTests::getAndReturnTiles,
                                 this)));
            }
            threadPool.join();
        }
        timer.Print("shared_ptr test3");
    }
};

#endif
