#pragma once

#include "utility/omTimer.hpp"

class OmChannel;
class OmSegmentation;

class OmVolumeProcessor {
public:
    OmVolumeProcessor()
    {}

    template <typename VOL>
    void BuildThreadedVolume(VOL* vol)
    {
        OmTimer timer;

        try{
            doBuildThreadedVolume(vol);
        } catch(...){
            throw OmIoException("volume build failed");
        }

        printf("OmVolumeBuilder: BuildThreadedVolume() done : %.6f secs\n",
               timer.s_elapsed());
    }

private:
    void doBuildThreadedVolume(OmSegmentation* vol);
    void doBuildThreadedVolume(OmChannel* vol);
};

