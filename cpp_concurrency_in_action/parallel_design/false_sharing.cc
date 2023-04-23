#include <iostream>
#include <thread>
#include <new>
#include <atomic>
#include <chrono>
#include <latch>
#include <vector>

using namespace std;
using namespace chrono;

#if defined(__cpp_lib_hardware_interference_size)
// default cacheline size from runtime
constexpr size_t CL_SIZE = hardware_constructive_interference_size;
#else
// most common cacheline size otherwise
constexpr size_t CL_SIZE = 64;
#endif

constexpr bool FALSE_SHARING = false;
constexpr size_t SECOND_ALIGN = FALSE_SHARING ? sizeof(int) : CL_SIZE;

struct shared_or_not
{
    atomic<int> a alignas(CL_SIZE);
    atomic<int> b alignas(SECOND_ALIGN);
};

int main()
{
    vector<jthread> threads;
    int hc = thread::hardware_concurrency();
    for( int nThreads = 1; nThreads <= hc; ++nThreads )
    {
        latch latSync( nThreads );
        atomic_uint sync( nThreads );
        // as much atomics as would fit into a cacheline
        struct { atomic_char atomics[CL_SIZE] alignas(CL_SIZE); } cacheLine;
        atomic_int64_t nsSum( 0 ); // sum of both thread execution times
        for( int t = 0; t != nThreads; ++t )
        {
            threads.emplace_back(
                [&]( atomic_char &a )
                {
                    latSync.arrive_and_wait(); // synch beginning of thread execution on kernel-level
                    if( sync.fetch_sub( 1, memory_order::relaxed ) != 1 ) // fine-synch on user-level
                        while( sync.load( memory_order::relaxed ) );
                    auto start = high_resolution_clock::now();
                    for( size_t r = 10'000'000; r--; )
                        a.fetch_add( 1, memory_order::relaxed ); // atomic increment
                    nsSum += duration_cast<nanoseconds>( high_resolution_clock::now() - start ).count();
                },
                ref( cacheLine.atomics[t % CL_SIZE] ) );
        }
        threads.resize( 0 ); // join all threads
        cout << nThreads << ": " << (int)(nsSum / (1.0e7 * nThreads) + 0.5) << endl;
    }

    shared_or_not sharedOrNot;
    auto theThread = []( atomic<int> &atomicValue )
    {
        for( size_t r = 100'000'000; r--; )
            ++atomicValue;
    };
    auto start = high_resolution_clock::now();
    threads.emplace_back( theThread, ref(sharedOrNot.a ));
    threads.emplace_back( theThread, ref(sharedOrNot.b ));
    threads.resize( 0 ); // join all threads
    cout << "Time Cost : " << (duration_cast<milliseconds>( high_resolution_clock::now() - start ).count()) << " with false_sharing: " << FALSE_SHARING <<endl;
}
