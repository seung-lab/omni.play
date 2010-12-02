#include <zi/time/timer.hpp>
#include <zi/parallel/algorithm.hpp>
#include <zi/parallel/numeric.hpp>

#include <zi/watershed/quickie.hpp>

#include <iostream>
#include <vector>
#include <algorithm>

const float conn[ 100*100*100*3 ] = {};
int resu[ 100*100*100 ];

int main()
{
    std::vector< int > x( 50000000 );
    for ( unsigned i = 0; i < 50000000; ++i )
    {
        x[ i ] = i;
    }

    zi::timer::wall t;

    t.reset();
    for ( int i = 0; i < 3; ++i )
    {
        std::random_shuffle( x.begin(), x.end() );
        std::sort( x.begin(), x.end() );
    }
    std::cout << t.elapsed< double >() << std::endl;

    t.reset();
    for ( int i = 0; i < 3; ++i )
    {
        zi::random_shuffle( x.begin(), x.end() );
        zi::sort( x.begin(), x.end() );
    }
    std::cout << t.elapsed< double >() << std::endl;

    //zi::watershed::quickie_impl< float, int > wimp( conn, 10, 10, 10, 10, 0.1, 0.9, 0.3, resu );
    //wimp.doit();
}

// g++ example.cpp -DZI_USE_OPENMP -I/Users/zlateski/zi_lib -fopenmp -lpthread -lrt -O3
