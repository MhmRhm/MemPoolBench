# MemPoolBench

This repository contains benchmarks for different memory pool implementations.

The following benchmarks are included:
- **No Pool**: A baseline that does not use a memory pool.
- **Boost.Pool**: Benchmarks using the [Boost.Pool](https://www.boost.org/doc/libs/latest/libs/pool/doc/html/index.html) library.
- **UnsyncStdObjectPool**: Benchmarks using [std::pmr::unsynchronized_pool_resource](https://en.cppreference.com/w/cpp/memory/unsynchronized_pool_resource.html).
- **SyncStdObjectPool**: Benchmarks using [std::pmr::synchronized_pool_resource](https://en.cppreference.com/w/cpp/memory/synchronized_pool_resource.html).
- **BLFObjectPool**: Benchmarks using a custom memory pool implementation based
on the [Boost.Lockfree Queue](https://www.boost.org/doc/libs/latest/doc/html/doxygen/classboost_1_1lockfree_1_1queue.html).

## Thread Local Cache (tcache)

When benchmarking with no memory pool, memory is allocated and deallocated
directly using the global `new` and `delete` operators. Glibc's `malloc` and
`free` are used under the hood for dynamic memory management in this case. Since
[glibc-2.26](https://sourceware.org/git/?p=glibc.git;a=commit;h=d5c3fafc4307c9b7a4c7d5cb381fcdbfad340bcc),
glibc has implemented per-thread caching for small memory allocations (up to
1024 bytes) thus performance of the no-pool benchmarks are much better than what
one might expect from a naive `malloc`/`free` implementation.

[Here](https://sourceware.org/glibc/wiki/MallocInternals) you can read more
about Thread Local Cache (tcache) in glibc. To get a more accurate picture of
the performance benefits of tcache and memory pools, you can disable tcache by
setting some environment variables before running the benchmarks:

```bash
# Disable tcache for all threads
GLIBC_TUNABLES=glibc.malloc.tcache_count=0 MALLOC_ARENA_MAX=1 \
./google_bench_libpool --benchmark_filter=BM_NoObjectPool_CreateAndDestroy
```

## Building the Executables

This project uses the [**SeeMake**](https://github.com/MhmRhm/SeeMake) template.
To build it on **Windows** or **Linux**, you’ll need to install the required
dependencies first.

* **Linux setup:** Follow the instructions [here](https://github.com/MhmRhm/SeeMake?tab=readme-ov-file#setting-up-linux).
* **Windows setup:** Follow the instructions [here](https://github.com/MhmRhm/SeeMake?tab=readme-ov-file#setting-up-windows).

Once the dependencies are installed, you can build all executables using the
following commands:

```bash
# On Linux
cmake --workflow --preset linux-default-release

# On Windows
cmake --workflow --preset windows-clang-release
# or
cmake --workflow --preset windows-default-release
```

## Benchmarking Results

The following show benchmarking results on a Linux VM with 8 CPU cores running
on a MacBook Pro with Apple M2 Pro chip.

#### Benchmark Results (Single Thread)
<p align="left"><img src="https://i.postimg.cc/RVbQDJ9N/Screenshot-2026-03-26-at-5-42-20-AM.png" alt="Single Thread Benchmark Results" width="400"></img></p>

#### Benchmark Results (2 Threads)
<p align="left"><img src="https://i.postimg.cc/3JbCc07W/Screenshot-2026-03-26-at-5-42-01-AM.png" alt="2 Threads Benchmark Results" width="400"></img></p>

#### Benchmark Results (4 Threads)
<p align="left"><img src="https://i.postimg.cc/yYrXb3BD/Screenshot-2026-03-26-at-5-41-48-AM.png" alt="4 Threads Benchmark Results" width="400"></img></p>

Looking at the results, first thing that we can see is that the no-pool
implementation performs significantly better than the memory pool
implementations. This is due to the fact that glibc's `malloc` and `free` are
highly optimized and benefit from the thread local cache (tcache) for small
allocations. Disabling tcache shows a significant performance difference.

The `glibc` implementation of `malloc` is a complex piece of software that does
not always perform optimally in every scenario. The benchmarks I conducted are
relatively straightforward, focusing on repeated allocations of the same size on
a single thread. However, real-world applications often involve more complex
allocation patterns, such as varying sizes and non-deterministic allocation and
deallocation order across multiple threads.

Another important scenario involves large allocations that exceed the tcache
threshold (1024 bytes). In such cases, memory pools can offer significantly
better performance. For example, in single-threaded benchmarks comparing
`std::pmr::unsynchronized_pool_resource` with a no-pool implementation using 4
KB allocations, the memory pool performs roughly 20x faster. On the other hand,
the no-pool approach scales more effectively in multi-threaded environments.

It is also worth noting that my benchmarks involve repeated allocations and
deallocations across all threads. This setup places a heavier emphasis on
synchronization overhead rather than pure allocation performance. In real-world
applications, contention is typically lower, meaning the performance advantages
of memory pools may be more pronounced.

The key takeaway is that even widely accepted advice—such as using memory pools
for small, repeated allocations—should not be taken at face value. No single
implementation consistently outperforms others in all scenarios. The most
reliable approach is to benchmark using realistic workloads that closely reflect
the behavior of the target application.
