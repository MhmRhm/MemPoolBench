
/**
 * @file pool.h
 * @brief Object pool implementations for memory management
 */
#pragma once

#include <memory_resource>

#include <boost/lockfree/queue.hpp>

/**
 * @brief Object pool using standard library's memory resources
 * @tparam T Type of object to pool
 * @tparam MemoryResource Type of memory resource to use for allocation
 */
template <typename T,
          typename MemoryResource = std::pmr::synchronized_pool_resource>
class StdObjectPool {
  MemoryResource m_pool{};
  std::pmr::polymorphic_allocator<T> m_alloc{&m_pool};

  /**
   * @brief Custom deleter to call destructor and deallocate memory back to pool
   */
  struct Deleter {
    std::pmr::polymorphic_allocator<T> *m_alloc{};
    void operator()(T *ptr) {
      if (!ptr)
        return;
      std::destroy_at(ptr);
      m_alloc->deallocate(ptr, 1);
    }
  };

public:
  using unique_type = std::unique_ptr<T, Deleter>;

  /**
   * @brief Allocate and construct a new object, reusing from pool if available
   * @tparam Args Constructor argument types
   * @param args Constructor arguments
   * @return unique_type Unique pointer with custom deleter to the object
   */
  template <typename... Args> unique_type make(Args &&...args) {
    T *ptr{};
    try {
      ptr = m_alloc.allocate(1);
      std::construct_at(ptr, std::forward<Args>(args)...);
    } catch (...) {
      if (ptr)
        m_alloc.deallocate(ptr, 1);
      ptr = nullptr;
    }
    return unique_type{ptr, Deleter{&m_alloc}};
  }

  unique_type make_for_overwrite() {
    T *ptr{};
    try {
      ptr = m_alloc.allocate(1);
    } catch (...) {
      ptr = nullptr;
    }
    return unique_type{ptr, Deleter{&m_alloc}};
  }
};

template <typename T>
using SyncStdObjectPool =
    StdObjectPool<T, std::pmr::synchronized_pool_resource>;

template <typename T>
using UnsyncStdObjectPool =
    StdObjectPool<T, std::pmr::unsynchronized_pool_resource>;

/**
 * @brief Object pool using Boost's lock-free queue for memory management
 * @tparam T Type of object to pool
 */
template <typename T> class BLFObjectPool {
private:
  using queue_type = boost::lockfree::queue<void *>;
  queue_type m_queue{0};

  /**
   * @brief Custom deleter to call destructor and push memory back to the queue
   */
  struct Deleter {
    queue_type *queue{};
    void operator()(T *ptr) const {
      if (!ptr)
        return;
      std::destroy_at(ptr);
      queue->push(ptr);
    }
  };

public:
  using unique_type = std::unique_ptr<T, Deleter>;

  /**
   * @brief Allocate and construct a new object, reusing from queue if available
   * @tparam Args Constructor argument types
   * @param args Constructor arguments
   * @return unique_type Unique pointer with custom deleter to the object
   */
  template <typename... Args> unique_type make(Args &&...args) {
    T *ptr{};
    if (!m_queue.pop(ptr))
      ptr = static_cast<T *>(::operator new(sizeof(T), std::nothrow));

    if (!ptr)
      return {};

    try {
      std::construct_at(ptr, std::forward<Args>(args)...);
    } catch (...) {
      m_queue.push(ptr);
      ptr = nullptr;
    }

    return unique_type{ptr, Deleter{&m_queue}};
  }

  unique_type make_for_overwrite() {
    T *ptr{};
    if (!m_queue.pop(ptr))
      ptr = static_cast<T *>(::operator new(sizeof(T), std::nothrow));

    if (!ptr)
      return {};

    return unique_type{ptr, Deleter{&m_queue}};
  }

  /**
   * @brief Destructor, deletes all objects in the queue
   * Note: All objects must be returned to the pool before the pool is
   * destroyed, otherwise it will lead to undefined behavior.
   */
  virtual ~BLFObjectPool() {
    T *ptr{};
    while (m_queue.pop(ptr)) {
      ::operator delete(ptr);
    }
  }
};
