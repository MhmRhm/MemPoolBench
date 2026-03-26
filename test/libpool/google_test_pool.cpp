#include "libpool/pool.h"
#include "gtest/gtest.h"

#include <latch>

namespace {
struct SimpleObject {
  std::atomic<size_t> &m_constructed;
  std::atomic<size_t> &m_destroyed;
  int x{};

  SimpleObject(int v, std::atomic<size_t> &constructed,
               std::atomic<size_t> &destroyed)
      : m_constructed{constructed}, m_destroyed{destroyed}, x{v} {
    m_constructed += 1;
  }
  ~SimpleObject() { m_destroyed += 1; }
};

struct ThrowingObject {
  size_t &m_constructed, &m_destroyed;

  ThrowingObject(size_t &constructed, size_t &destroyed)
      : m_constructed{constructed}, m_destroyed{destroyed} {
    ++m_constructed;
    throw std::exception{};
  }
  ~ThrowingObject() { ++m_destroyed; }
};

struct ComplexObject {
  std::string m_name{};
  std::vector<int> m_values{};
  size_t &m_constructed, &m_destroyed;
  ComplexObject(std::string name, std::vector<int> values, size_t &constructed,
                size_t &destroyed)
      : m_name(std::move(name)), m_values(std::move(values)),
        m_constructed(constructed), m_destroyed(destroyed) {
    ++m_constructed;
  }
  ~ComplexObject() { ++m_destroyed; }
};
} // namespace

template <typename T> class SimpleObjectPoolTest : public testing::Test {
protected:
  static inline std::atomic<size_t> m_constructed{};
  static inline std::atomic<size_t> m_destroyed{};

private:
  void SetUp() override {
    m_constructed = 0;
    m_destroyed = 0;
  }
};
TYPED_TEST_SUITE_P(SimpleObjectPoolTest);

TYPED_TEST_P(SimpleObjectPoolTest, SingleAllocation) {
  // given
  TypeParam pool{};

  // when
  auto obj{pool.make(42, this->m_constructed, this->m_destroyed)};

  // then
  ASSERT_NE(obj.get(), nullptr);
  EXPECT_EQ(obj->x, 42);

  // when
  obj.reset();

  // then
  EXPECT_EQ(this->m_constructed, 1);
  EXPECT_EQ(this->m_destroyed, 1);
}

TYPED_TEST_P(SimpleObjectPoolTest, MultipleAllocations) {
  // given
  TypeParam pool{};

  // when
  auto obj1{pool.make(1, this->m_constructed, this->m_destroyed)};
  auto obj2{pool.make(2, this->m_constructed, this->m_destroyed)};

  // then
  EXPECT_EQ(this->m_constructed, 2);
  EXPECT_EQ(this->m_destroyed, 0);

  // when
  {
    decltype(obj1) temp1{}, temp2{};
    std::swap(obj1, temp1);
    std::swap(obj2, temp2);
  }

  // then
  EXPECT_EQ(this->m_destroyed, 2);
}

TYPED_TEST_P(SimpleObjectPoolTest, MassAllocations) {
  using namespace std;

  // given
  TypeParam pool{};
  using unique_type = decltype(pool)::unique_type;

  const size_t num_threads{10}, num_allocs{1000};
  std::latch sync_point{num_threads};
  vector<jthread> threads{};

  auto create_objects{[&](const int count) {
    sync_point.arrive_and_wait();
    vector<unique_type> objects{};
    for (int i{}; i < count; i += 1) {
      objects.push_back(pool.make(i, this->m_constructed, this->m_destroyed));
    }
  }};

  // when
  for (int i{}; i < num_threads; i += 1) {
    threads.emplace_back(create_objects, num_allocs);
  }
  threads.clear();

  // then
  EXPECT_EQ(this->m_constructed, num_allocs * num_threads);
  EXPECT_EQ(this->m_destroyed, num_allocs * num_threads);
}

REGISTER_TYPED_TEST_SUITE_P(SimpleObjectPoolTest, SingleAllocation,
                            MultipleAllocations, MassAllocations);
INSTANTIATE_TYPED_TEST_SUITE_P(SyncStd, SimpleObjectPoolTest,
                               SyncStdObjectPool<SimpleObject>);
INSTANTIATE_TYPED_TEST_SUITE_P(BLF, SimpleObjectPoolTest,
                               BLFObjectPool<SimpleObject>);

template <typename T> class ComplexObjectPoolTest : public testing::Test {
protected:
  static inline size_t m_constructed{};
  static inline size_t m_destroyed{};

private:
  void SetUp() override {
    m_constructed = 0;
    m_destroyed = 0;
  }
};
TYPED_TEST_SUITE_P(ComplexObjectPoolTest);

TYPED_TEST_P(ComplexObjectPoolTest, SingleAllocation) {
  // given
  TypeParam pool{};

  // when
  auto obj{pool.make("test", std::vector<int>{1, 2, 3}, this->m_constructed,
                     this->m_destroyed)};

  // then
  ASSERT_NE(obj, nullptr);

  EXPECT_EQ(obj->m_name, "test");
  EXPECT_EQ(obj->m_values.size(), 3);
  EXPECT_EQ(obj->m_values[1], 2);

  EXPECT_EQ(this->m_constructed, 1);
  EXPECT_EQ(this->m_destroyed, 0);

  // when
  {
    decltype(obj) temp{};
    std::swap(obj, temp);
  }

  // then
  EXPECT_EQ(this->m_constructed, 1);
  EXPECT_EQ(this->m_destroyed, 1);
}

REGISTER_TYPED_TEST_SUITE_P(ComplexObjectPoolTest, SingleAllocation);
INSTANTIATE_TYPED_TEST_SUITE_P(UnsyncStd, ComplexObjectPoolTest,
                               UnsyncStdObjectPool<ComplexObject>);
INSTANTIATE_TYPED_TEST_SUITE_P(SyncStd, ComplexObjectPoolTest,
                               SyncStdObjectPool<ComplexObject>);
INSTANTIATE_TYPED_TEST_SUITE_P(BLF, ComplexObjectPoolTest,
                               BLFObjectPool<ComplexObject>);

template <typename T> class ThrowingObjectPoolTest : public testing::Test {
protected:
  static inline size_t m_constructed{};
  static inline size_t m_destroyed{};

private:
  void SetUp() override {
    m_constructed = 0;
    m_destroyed = 0;
  }
};
TYPED_TEST_SUITE_P(ThrowingObjectPoolTest);

TYPED_TEST_P(ThrowingObjectPoolTest, AllocationThrows) {
  // given
  TypeParam pool{};

  // when
  auto obj{pool.make(this->m_constructed, this->m_destroyed)};

  // then
  ASSERT_EQ(obj.get(), nullptr);
  EXPECT_EQ(this->m_constructed, 1);
  EXPECT_EQ(this->m_destroyed, 0);
}

REGISTER_TYPED_TEST_SUITE_P(ThrowingObjectPoolTest, AllocationThrows);
INSTANTIATE_TYPED_TEST_SUITE_P(UnsyncStd, ThrowingObjectPoolTest,
                               UnsyncStdObjectPool<ThrowingObject>);
INSTANTIATE_TYPED_TEST_SUITE_P(SyncStd, ThrowingObjectPoolTest,
                               SyncStdObjectPool<ThrowingObject>);
INSTANTIATE_TYPED_TEST_SUITE_P(BLF, ThrowingObjectPoolTest,
                               BLFObjectPool<ThrowingObject>);
