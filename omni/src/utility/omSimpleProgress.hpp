#ifndef	OM_SIMPLE_PROGRESS_HPP
#define OM_SIMPLE_PROGRESS_HPP

#include "utility/omLockedPODs.hpp"
#include "threads/omTaskManager.hpp"
#include "utility/omTimer.hpp"

class OmSimpleProgress {
private:
    const int max_;
    const std::string title_;

    LockedInt32 cur_;
    size_t lineLen_;

    OmThreadPool threadPool_;
    OmTimer timer_;

public:
    OmSimpleProgress(const int max, const std::string& title)
        : max_(max)
        , title_(title)
        , lineLen_(0)
    {
        cur_.set(0);

        threadPool_.start(1);
        std::cout << title << "\n";
    }

    ~OmSimpleProgress()
    {
        std::cout << "done with " << title_ << "\n";
        threadPool_.join();
    }

    void Join(){
        threadPool_.join();
    }

    void DidOne(const std::string prefix = "")
    {
        threadPool_.push_back(
            zi::run_fn(
                zi::bind(
                    &OmSimpleProgress::didOne, this, prefix)));
    }

private:
    void didOne(const std::string prefix)
    {
        cur_.inc();

        coutLine(prefix);

        if(max_ == cur_.get()){
            printDone();
        }
    }

    void printDone(){
        timer_.Print("\ndone with "+title_);
    }

    void coutLine(const std::string& prefix)
    {
        std::ostringstream stm;
        stm << "\r\t" << prefix << ": "
            << cur_.get() << " of " << max_;

        std::string str = stm.str();
        if(str.size() > lineLen_){
            lineLen_ = str.size();
        } else {
            std::string pad(lineLen_ - str.size(), ' ');
            str += pad;
        }

        std::cout << str << "\r" << std::flush;
    }
};

#endif
