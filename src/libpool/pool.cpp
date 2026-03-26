#include "libpool/pool.h"

template class StdObjectPool<int, std::pmr::synchronized_pool_resource>;
template class StdObjectPool<int, std::pmr::unsynchronized_pool_resource>;
template class StdObjectPool<std::array<uint8_t, 16>,
                             std::pmr::synchronized_pool_resource>;
template class StdObjectPool<std::array<uint8_t, 16>,
                             std::pmr::unsynchronized_pool_resource>;
