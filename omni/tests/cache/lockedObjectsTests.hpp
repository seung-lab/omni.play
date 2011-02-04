#ifndef LOCKED_OBJECTS_TESTS_HPP
#define LOCKED_OBJECTS_TESTS_HPP

#include "tests/testUtils.hpp"
#include "utility/omTimer.hpp"
#include "system/cache/omLockedCacheObjects.hpp"

class LockedObjectsTests {
public:
    LockedObjectsTests()
    {}

    void RunAll()
    {
        testMuiltiIndex<LockedKeyMultiIndex<int> >();
        testMuiltiIndex<LockedKeyList<int> >();

        //runPerfTests();
    }

private:

    void runPerfTests()
    {
        const uint64_t num = 200;
        runNtimes<LockedKeyMultiIndex<int> >(num);
        runNtimes<LockedKeyList<int> >(num);
    }

    template <typename T>
    void runNtimes(const uint64_t num)
    {
        std::cout << "running " << num << " times\n";

        OmTimer timer;
        for(uint64_t i = 0; i < num; ++i){
            std::cout << "\r\t" << i << std::flush;
            testMuiltiIndex<T>();
        }

        printf("\n");
        timer.PrintDone();
    }

    template <typename T>
    void testMuiltiIndex()
    {
        {
            T lru;
            verify(lru.empty());
        }
        {
            T lru;
            const int ret = lru.remove_oldest();
            verify(ret == 0);
        }
        {
            T lru;

            const int maxN = 1000;

            for(int i = 0; i < maxN; ++i){
                lru.touch(i);
            }

            verify(!lru.empty());

            for(int i = maxN-1; i > (maxN/2)-1; --i){
                lru.touch(i);
            }

            for(int i = 0; i < maxN/2; ++i){
                const int ret = lru.remove_oldest();
                verify(i == ret);
            }

            for(int i = maxN-1; i > (maxN/2)-1; --i){
                const int ret = lru.remove_oldest();
                verify(i == ret);
            }

            verify(lru.empty());
        }
    }
};

#endif
