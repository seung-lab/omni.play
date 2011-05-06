#ifndef OM_VOLUME_PROCESSOR_HPP
#define OM_VOLUME_PROCESSOR_HPP

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

#endif
