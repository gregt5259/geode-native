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

#define ROOT_NAME "testAttributesFactory"

#include "fw_helper.hpp"
#include <geode/GeodeCppCache.hpp>
#include <CacheRegionHelper.hpp>

using namespace apache::geode::client;

// BEGIN_TEST(ATTRIBUTE_FACTORY)
//  {
//    AttributesFactory af;
//    RegionAttributesPtr ra;
//    RegionPtr region;
//
//    CacheFactoryPtr cacheFactoryPtr = CacheFactory::createCacheFactory();
//    CachePtr cache = cacheFactoryPtr->create();
//    ra = af.createRegionAttributes();
//
//    CacheImpl* cacheImpl = CacheRegionHelper::getCacheImpl(cache.get());
//    cacheImpl->createRegion("region1", ra, region);
//    LOG("local region created with HA cache specification.");
//    cache->close();
//  }
// END_TEST(ATTRIBUTE_FACTORY)

/* testing attributes with invalid value */
/* testing with negative values */          /*see bug no #865 */
/* testing with exceed boundry condition */ /*see bug no #865 */
BEGIN_TEST(REGION_FACTORY)
  {
    CacheFactoryPtr cf = CacheFactory::createCacheFactory();
    CachePtr m_cache = cf->create();

    RegionFactoryPtr rf = m_cache->createRegionFactory(LOCAL);
    /*see bug no #865 */
    try {
      rf->setInitialCapacity(-1);
      FAIL("Should have got expected IllegalArgumentException");
    } catch (IllegalArgumentException&) {
      LOG("Got expected IllegalArgumentException");
    }

    RegionPtr m_region = rf->create("Local_ETTL_LI");
    LOGINFO("m_region->getAttributes()->getInitialCapacity() = %d ",
            m_region->getAttributes()->getInitialCapacity());
    ASSERT(m_region->getAttributes()->getInitialCapacity() == 10000,
           "Incorrect InitialCapacity");

    m_region->put(1, 1);
    auto res = std::dynamic_pointer_cast<CacheableInt32>(m_region->get(1));
    ASSERT(res->value() == 1, "Expected to find value 1.");

    m_region->destroyRegion();
    m_cache->close();
    m_cache = nullptr;
    m_region = nullptr;

    CacheFactoryPtr cf1 = CacheFactory::createCacheFactory();
    CachePtr m_cache1 = cf1->create();

    RegionFactoryPtr rf1 = m_cache1->createRegionFactory(LOCAL);
    /*see bug no #865 */
    try {
      rf1->setInitialCapacity(2147483648U);
      FAIL("Should have got expected IllegalArgumentException");
    } catch (IllegalArgumentException&) {
      LOG("Got expected IllegalArgumentException");
    }
    RegionPtr m_region1 = rf1->create("Local_ETTL_LI");
    LOGINFO("m_region1->getAttributes()->getInitialCapacity() = %d ",
            m_region1->getAttributes()->getInitialCapacity());
    ASSERT(m_region1->getAttributes()->getInitialCapacity() == 10000,
           "Incorrect InitialCapacity");

    m_region1->put(1, 1);
    auto res1 = std::dynamic_pointer_cast<CacheableInt32>(m_region1->get(1));
    ASSERT(res1->value() == 1, "Expected to find value 1.");

    m_region1->destroyRegion();
    m_cache1->close();
    m_cache1 = nullptr;
    m_region1 = nullptr;
  }
END_TEST(REGION_FACTORY)
