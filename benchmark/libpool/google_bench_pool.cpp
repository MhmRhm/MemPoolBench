/**
 * @file
 * @brief Benchmarks for object pool implementations
 */

#include "benchmark/benchmark.h"
#include "libpool/pool.h"

#include <boost/pool/object_pool.hpp>

namespace {
using namespace std;
using namespace chrono;
/**
 * @brief Number of successive allocations then deallocations in each benchmark
 */
static constexpr size_t SUCCESSION{64};

/**
 * @brief Custom deleter for boost::object_pool to call destructor and push
 * memory back to the pool
 * @tparam T Type of object in the pool
 */
template <typename T> struct BoostDeleter {
  boost::object_pool<T> *pool{};
  void operator()(T *ptr) const {
    if (!ptr)
      return;
    pool->destroy(ptr);
  }
};
} // namespace

/**
 * @brief Benchmark for creating and destroying objects without using a pool
 * @tparam N Size of the object in bytes
 * @note To disable *tcache* optimizations:
 * @code
 * GLIBC_TUNABLES=glibc.malloc.tcache_count=0 \
 * MALLOC_ARENA_MAX=1 \
 * ./google_bench_libcore --benchmark_filter=BM_NoObjectPool_CreateAndDestroy
 * @endcode
 */
template <size_t N>
void BM_NoObjectPool_CreateAndDestroy(benchmark::State &state) {
  using object_type = array<uint8_t, N>;

  vector<unique_ptr<object_type>> collection{};
  high_resolution_clock::time_point start{}, end{};
  object_type *obj{};

  collection.reserve(SUCCESSION);

  for (auto _ : state) {
    start = high_resolution_clock::now();

    for (size_t i{}; i < SUCCESSION; i += 1) {
      collection.push_back(make_unique_for_overwrite<object_type>());
      obj = collection.back().get();
      (*obj)[N - 1] = 0xFF; // forces OS to actually allocate
      benchmark::DoNotOptimize((*obj)[N - 1]);
    }
    benchmark::ClobberMemory();
    collection.clear();

    end = high_resolution_clock::now();
    state.SetIterationTime(
        duration_cast<duration<double>>(end - start).count());
  }

  state.counters["Allocations"] =
      benchmark::Counter(static_cast<int64_t>(state.iterations() * SUCCESSION),
                         benchmark::Counter::kIsRate);
}

/**
 * @brief Benchmark for creating and destroying objects using Boost object pool
 * @tparam N Size of the object in bytes
 */
template <size_t N>
void BM_BoostObjectPool_CreateAndDestroy(benchmark::State &state) {
  using object_type = array<uint8_t, N>;
  using deleter_type = BoostDeleter<object_type>;
  using unique_type = unique_ptr<object_type, deleter_type>;

  high_resolution_clock::time_point start{}, end{};
  boost::object_pool<object_type> pool{};
  vector<unique_type> collection{};
  object_type *obj{};

  collection.reserve(SUCCESSION);

  for (auto _ : state) {
    start = high_resolution_clock::now();

    for (size_t i{}; i < SUCCESSION; i += 1) {
      collection.push_back(unique_type{pool.construct(), deleter_type{&pool}});
      obj = collection.back().get();
      (*obj)[N - 1] = 0xFF; // forces OS to actually allocate
      benchmark::DoNotOptimize((*obj)[N - 1]);
    }
    benchmark::ClobberMemory();
    collection.clear();

    end = high_resolution_clock::now();
    state.SetIterationTime(
        duration_cast<duration<double>>(end - start).count());
  }

  state.counters["Allocations"] =
      benchmark::Counter(static_cast<int64_t>(state.iterations() * SUCCESSION),
                         benchmark::Counter::kIsRate);
}

/**
 * @brief Benchmark fixture using an object pool
 * @tparam Pool Type of object pool to benchmark
 */
template <typename Pool>
class BM_ObjectPool_Fixture : public benchmark::Fixture {
protected:
  using pool_type = Pool;
  using unique_type = pool_type::unique_type;
  using object_type = Pool::unique_type::element_type;

  pool_type m_pool{};

  /**
   * @brief Set up the fixture by pre-allocating objects in the pool
   */
  void SetUp(benchmark::State &state) {
    std::vector<unique_type> collection{};
    collection.reserve(SUCCESSION);

    for (size_t i{}; i < SUCCESSION; i += 1) {
      collection.push_back(this->m_pool.make());
    }
  }
};
BENCHMARK_TEMPLATE_METHOD_F(BM_ObjectPool_Fixture,
                            CreateAndDestroy)(benchmark::State &state) {
  std::vector<typename Base::unique_type> collection{};
  high_resolution_clock::time_point start{}, end{};
  typename Base::object_type *obj{};

  collection.reserve(SUCCESSION);

  for (auto _ : state) {
    start = high_resolution_clock::now();

    for (size_t i{}; i < SUCCESSION; i += 1) {
      collection.push_back(this->m_pool.make_for_overwrite());
      obj = collection.back().get();
      (*obj)[obj->size() - 1] = 0xFF; // forces OS to actually allocate
      benchmark::DoNotOptimize((*obj)[obj->size() - 1]);
    }
    benchmark::ClobberMemory();
    collection.clear();

    end = high_resolution_clock::now();
    state.SetIterationTime(
        duration_cast<duration<double>>(end - start).count());
  }

  state.counters["Allocations"] =
      benchmark::Counter(static_cast<int64_t>(state.iterations() * SUCCESSION),
                         benchmark::Counter::kIsRate);
}

// benchmarks for NoObjectPool
BENCHMARK(BM_NoObjectPool_CreateAndDestroy<1>)
    ->ThreadRange(1, 1 << 6)
    ->UseRealTime();
;
BENCHMARK(BM_NoObjectPool_CreateAndDestroy<8>)
    ->ThreadRange(1, 1 << 6)
    ->UseRealTime();
;
BENCHMARK(BM_NoObjectPool_CreateAndDestroy<64>)
    ->ThreadRange(1, 1 << 6)
    ->UseRealTime();
;
BENCHMARK(BM_NoObjectPool_CreateAndDestroy<512>)
    ->ThreadRange(1, 1 << 6)
    ->UseRealTime();
;
BENCHMARK(BM_NoObjectPool_CreateAndDestroy<4096>)
    ->ThreadRange(1, 1 << 6)
    ->UseRealTime();
;

// benchmarks for BoostObjectPool
BENCHMARK(BM_BoostObjectPool_CreateAndDestroy<1>)->UseRealTime();
BENCHMARK(BM_BoostObjectPool_CreateAndDestroy<8>)->UseRealTime();
BENCHMARK(BM_BoostObjectPool_CreateAndDestroy<64>)->UseRealTime();
BENCHMARK(BM_BoostObjectPool_CreateAndDestroy<512>)->UseRealTime();
BENCHMARK(BM_BoostObjectPool_CreateAndDestroy<4096>)->UseRealTime();

// benchmarks for UnsyncStdObjectPool
BENCHMARK_TEMPLATE_INSTANTIATE_F(BM_ObjectPool_Fixture, CreateAndDestroy,
                                 UnsyncStdObjectPool<std::array<uint8_t, 1>>)
    ->UseRealTime();
BENCHMARK_TEMPLATE_INSTANTIATE_F(BM_ObjectPool_Fixture, CreateAndDestroy,
                                 UnsyncStdObjectPool<std::array<uint8_t, 8>>)
    ->UseRealTime();
BENCHMARK_TEMPLATE_INSTANTIATE_F(BM_ObjectPool_Fixture, CreateAndDestroy,
                                 UnsyncStdObjectPool<std::array<uint8_t, 64>>)
    ->UseRealTime();
BENCHMARK_TEMPLATE_INSTANTIATE_F(BM_ObjectPool_Fixture, CreateAndDestroy,
                                 UnsyncStdObjectPool<std::array<uint8_t, 512>>)
    ->UseRealTime();
BENCHMARK_TEMPLATE_INSTANTIATE_F(BM_ObjectPool_Fixture, CreateAndDestroy,
                                 UnsyncStdObjectPool<std::array<uint8_t, 4096>>)
    ->UseRealTime();

// benchmarks for SyncStdObjectPool
BENCHMARK_TEMPLATE_INSTANTIATE_F(BM_ObjectPool_Fixture, CreateAndDestroy,
                                 SyncStdObjectPool<std::array<uint8_t, 1>>)
    ->ThreadRange(1, 1 << 6)
    ->UseRealTime();
BENCHMARK_TEMPLATE_INSTANTIATE_F(BM_ObjectPool_Fixture, CreateAndDestroy,
                                 SyncStdObjectPool<std::array<uint8_t, 8>>)
    ->ThreadRange(1, 1 << 6)
    ->UseRealTime();
BENCHMARK_TEMPLATE_INSTANTIATE_F(BM_ObjectPool_Fixture, CreateAndDestroy,
                                 SyncStdObjectPool<std::array<uint8_t, 64>>)
    ->ThreadRange(1, 1 << 6)
    ->UseRealTime();
BENCHMARK_TEMPLATE_INSTANTIATE_F(BM_ObjectPool_Fixture, CreateAndDestroy,
                                 SyncStdObjectPool<std::array<uint8_t, 512>>)
    ->ThreadRange(1, 1 << 6)
    ->UseRealTime();
BENCHMARK_TEMPLATE_INSTANTIATE_F(BM_ObjectPool_Fixture, CreateAndDestroy,
                                 SyncStdObjectPool<std::array<uint8_t, 4096>>)
    ->ThreadRange(1, 1 << 6)
    ->UseRealTime();

// benchmarks for BLFObjectPool
BENCHMARK_TEMPLATE_INSTANTIATE_F(BM_ObjectPool_Fixture, CreateAndDestroy,
                                 BLFObjectPool<std::array<uint8_t, 1>>)
    ->ThreadRange(1, 1 << 6)
    ->UseRealTime();
BENCHMARK_TEMPLATE_INSTANTIATE_F(BM_ObjectPool_Fixture, CreateAndDestroy,
                                 BLFObjectPool<std::array<uint8_t, 8>>)
    ->ThreadRange(1, 1 << 6)
    ->UseRealTime();
BENCHMARK_TEMPLATE_INSTANTIATE_F(BM_ObjectPool_Fixture, CreateAndDestroy,
                                 BLFObjectPool<std::array<uint8_t, 64>>)
    ->ThreadRange(1, 1 << 6)
    ->UseRealTime();
BENCHMARK_TEMPLATE_INSTANTIATE_F(BM_ObjectPool_Fixture, CreateAndDestroy,
                                 BLFObjectPool<std::array<uint8_t, 512>>)
    ->ThreadRange(1, 1 << 6)
    ->UseRealTime();
BENCHMARK_TEMPLATE_INSTANTIATE_F(BM_ObjectPool_Fixture, CreateAndDestroy,
                                 BLFObjectPool<std::array<uint8_t, 4096>>)
    ->ThreadRange(1, 1 << 6)
    ->UseRealTime();
