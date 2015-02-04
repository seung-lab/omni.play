#pragma once

#include <mutex>
#include <condition_variable>

#include <zi/utility/non_copyable.hpp>

namespace zi {
namespace mesh {

template<int N>
class semaphore: non_copyable
{
private:
    int credit_ ;
    int waiters_;

    std::condition_variable cv_;
    std::mutex              m_ ;

public:
    semaphore()
        : credit_(N)
        , waiters_(0)
        , cv_()
        , m_()
    {}

    void acquire()
    {
        std::unique_lock<std::mutex> g(m_);

        while ( credit_ == 0 )
        {
            ++waiters_;
            cv_.wait(g);
            --waiters_;
        }

        --credit_;
    }

    void release()
    {
        std::unique_lock<std::mutex> g(m_);

        ++credit_;

        if ( waiters_ > 0 )
        {
            cv_.notify_one();
        }
    }

    struct guard
    {
    private:
        semaphore& s;

    public:
        guard(semaphore& _s)
            : s(_s)
        {
            s.acquire();
        }

        ~guard()
        {
            s.release();
        }
    };

}; // class semaphore

}} // namespace zi::mesh
