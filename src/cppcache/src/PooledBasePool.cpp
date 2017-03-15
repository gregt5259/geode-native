
#include <mutex>

#include "PooledBasePool.hpp"
#include "util/concurrent/spinlock_mutex.hpp"

namespace apache {
namespace geode {
namespace client {

using util::concurrent::spinlock_mutex;

PooledBasePool::~PooledBasePool() {
  std::lock_guard<spinlock_mutex> guard(m_poolLock);
  while (!m_pooldata.empty()) {
    PooledBase* item = m_pooldata.front();
    m_pooldata.pop_front();
    delete item;
  }
}

void PooledBasePool::returnToPool(PooledBase* poolable) {
  poolable->prePool();
  {
    std::lock_guard<spinlock_mutex> guard(m_poolLock);
    m_pooldata.push_back(const_cast<PooledBase*>(poolable));
  }
}

PooledBase* PooledBasePool::takeFromPool() {
  PooledBase* result = nullptr;
  {
    std::lock_guard<spinlock_mutex> guard(m_poolLock);
    if (!m_pooldata.empty()) {
      result = m_pooldata.front();
      m_pooldata.pop_front();
    }
  }
  if (result != nullptr) {
    result->postPool();
  }
  return result;
}

void PooledBasePool::clear() {
  std::lock_guard<spinlock_mutex> guard(m_poolLock);
  while (!m_pooldata.empty()) {
    PooledBase* item = m_pooldata.front();
    m_pooldata.pop_front();
    delete item;
  }
}

}  // namespace client
}  // namespace geode
}  // namespace apache
