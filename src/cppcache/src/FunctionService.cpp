/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <geode/FunctionService.hpp>
#include <geode/ExceptionTypes.hpp>
#include <ExecutionImpl.hpp>
#include <ProxyRegion.hpp>
#include <UserAttributes.hpp>
#include <ProxyCache.hpp>
#include <geode/PoolManager.hpp>
#include <CacheRegionHelper.hpp>

using namespace apache::geode::client;

ExecutionPtr FunctionService::onRegion(RegionPtr region) {
  LOGDEBUG("FunctionService::onRegion(RegionPtr region)");
  if (region == nullptr) {
    throw NullPointerException("FunctionService::onRegion: region is null");
  }

  const PoolPtr& pool = region->getPool();

  if (pool == nullptr) {
    throw IllegalArgumentException("Pool attached with region is closed.");
  }
  ProxyCachePtr proxyCache = nullptr;

  if (pool->getMultiuserAuthentication()) {
    ProxyRegion* pr = dynamic_cast<ProxyRegion*>(region.get());
    if (pr != nullptr) {
      LOGDEBUG("FunctionService::onRegion(RegionPtr region) proxy cache");
      // it is in multiuser mode
      proxyCache = pr->m_proxyCache;
      PoolPtr userAttachedPool = proxyCache->m_userAttributes->getPool();
      PoolPtr pool = PoolManager::find(userAttachedPool->getName());
      if (!(pool != nullptr && pool.get() == userAttachedPool.get() &&
            !pool->isDestroyed())) {
        throw IllegalStateException(
            "Pool has been closed with attached Logical Cache.");
      }
      RegionPtr tmpRegion;
      tmpRegion = nullptr;
      // getting real region to execute function on region
      if (!CacheFactory::getAnyInstance()->isClosed()) {
        CacheRegionHelper::getCacheImpl(CacheFactory::getAnyInstance().get())
            ->getRegion(region->getName(), tmpRegion);
      } else {
        throw IllegalStateException("Cache has been closed");
      }

      if (tmpRegion == nullptr) {
        throw IllegalStateException("Real region has been closed.");
      }
      region = tmpRegion;
    } else {
      throw IllegalArgumentException(
          "onRegion() argument region should have get from RegionService.");
    }
  }

  return std::make_shared<ExecutionImpl>(region, proxyCache, pool);
}

ExecutionPtr FunctionService::onServerWithPool(const PoolPtr& pool) {
  if (pool == nullptr) {
    throw NullPointerException("FunctionService::onServer: pool is null");
  }
  if (pool->getMultiuserAuthentication()) {
    throw UnsupportedOperationException(
        "This API is not supported in multiuser mode. "
        "Please use FunctionService::onServer(RegionService) API.");
  }
  return std::make_shared<ExecutionImpl>(pool);
}

ExecutionPtr FunctionService::onServersWithPool(const PoolPtr& pool) {
  if (pool == nullptr) {
    throw NullPointerException("FunctionService::onServers: pool is null");
  }
  if (pool->getMultiuserAuthentication()) {
    throw UnsupportedOperationException(
        "This API is not supported in multiuser mode. "
        "Please use FunctionService::onServers(RegionService) API.");
  }

  return std::make_shared<ExecutionImpl>(pool, true);
}

ExecutionPtr FunctionService::onServerWithCache(const RegionServicePtr& cache) {
  if (cache->isClosed()) {
    throw IllegalStateException("Cache has been closed");
  }

  auto pc = std::dynamic_pointer_cast<ProxyCache>(cache);

  LOGDEBUG("FunctionService::onServer:");
  if (pc != nullptr) {
    PoolPtr userAttachedPool = pc->m_userAttributes->getPool();
    PoolPtr pool = PoolManager::find(userAttachedPool->getName());
    if (pool != nullptr && pool.get() == userAttachedPool.get() &&
        !pool->isDestroyed()) {
      return std::make_shared<ExecutionImpl>(pool, false, pc);
    }
    throw IllegalStateException(
        "Pool has been close to execute function on server");
  } else {
    CachePtr realcache = std::static_pointer_cast<Cache>(cache);
    return FunctionService::onServer(realcache->m_cacheImpl->getDefaultPool());
  }
}

ExecutionPtr FunctionService::onServersWithCache(
    const RegionServicePtr& cache) {
  if (cache->isClosed()) {
    throw IllegalStateException("Cache has been closed");
  }

  auto pc = std::dynamic_pointer_cast<ProxyCache>(cache);

  LOGDEBUG("FunctionService::onServers:");
  if (pc != nullptr && !cache->isClosed()) {
    auto userAttachedPool = pc->m_userAttributes->getPool();
    auto pool = PoolManager::find(userAttachedPool->getName());
    if (pool != nullptr && pool.get() == userAttachedPool.get() &&
        !pool->isDestroyed()) {
      return std::make_shared<ExecutionImpl>(pool, true, pc);
    }
    throw IllegalStateException(
        "Pool has been close to execute function on server");
  } else {
    auto realcache = std::static_pointer_cast<Cache>(cache);
    return FunctionService::onServers(realcache->m_cacheImpl->getDefaultPool());
  }
}
