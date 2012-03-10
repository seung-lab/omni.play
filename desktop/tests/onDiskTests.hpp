#pragma once

#include "datalayer/fs/omOnDiskBoostUnorderedMap.hpp"
#include "tests/testUtils.hpp"

class OnDiskTests {
public:
    OnDiskTests()
    {}

    void Run()
    {
        const int max = 5000000;

        {
            OmTimer timer;

            boost::unordered_map<int, int> map;
            for(int i = 0; i < max; ++i){
                map[i] = 2*i;
            }

            timer.Print(om::string::humanizeNum(max) + " in memory write tests done");

            std::cout << "bucket count: "
                      << om::string::humanizeNum(map.bucket_count())
                      << "\n";

            timer.reset();

            for(int i = 0; i < max; ++i){
                verify(2*i == map.at(i));
            }

            timer.Print(om::string::humanizeNum(max) + " in memory read tests done");
        }

        std::cout << "\nredo, increasing initial number of buckets\n";

        {
            OmTimer timer;

            boost::unordered_map<int, int> map;
            map.rehash(10000000);

            for(int i = 0; i < max; ++i){
                map[i] = 2*i;
            }

            timer.Print(om::string::humanizeNum(max) + " in memory write tests done");

            std::cout << "bucket count: "
                      << om::string::humanizeNum(map.bucket_count())
                      << "\n";

            timer.reset();

            for(int i = 0; i < max; ++i){
                verify(2*i == map.at(i));
            }

            timer.Print(om::string::humanizeNum(max) + " in memory read tests done");
        }

        {
            OmTimer timer;

            OmOnDiskBoostUnorderedMap<int, int> map;

            for(int i = 0; i < max; ++i){
                map.insert(i, 2*i);
            }

            timer.Print(om::string::humanizeNum(max) + " on disk write tests done");

            timer.reset();

            for(int i = 0; i < max; ++i){
                verify(2*i == map.at(i));
            }

            timer.Print(om::string::humanizeNum(max) + " on disk read tests done");
        }
    }
};

