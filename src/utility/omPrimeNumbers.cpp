#include "utility/omPrimeNumbers.hpp"
#include "common/omException.h"

int om::constants::getNextBiggestPrime(const int num)
{
    const int* nextBiggestPrime =
        std::lower_bound(first1000Primes.begin(),
                         first1000Primes.end(),
                         num);

    if(nextBiggestPrime == first1000Primes.end()){
        throw OmIoException("couldn't find next biggest prime");
    }

    return *nextBiggestPrime;
}
