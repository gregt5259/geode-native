#pragma once

#ifndef GEODE_TYPES_H_
#define GEODE_TYPES_H_

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

#include <memory>

#include "geode_globals.hpp"

/**
 * @file
 */

namespace apache {
namespace geode {
namespace client {

class CacheFactory;

#define _GF_PTR_DEF_(a, b) \
  class CPPCACHE_EXPORT a; \
  typedef std::shared_ptr<a> b;

_GF_PTR_DEF_(DistributedSystem, DistributedSystemPtr);
_GF_PTR_DEF_(CacheFactory, CacheFactoryPtr);
_GF_PTR_DEF_(RegionService, RegionServicePtr);
_GF_PTR_DEF_(GeodeCache, GeodeCachePtr);
_GF_PTR_DEF_(Cache, CachePtr);
_GF_PTR_DEF_(RegionFactory, RegionFactoryPtr);
_GF_PTR_DEF_(AttributesFactory, AttributesFactoryPtr);
_GF_PTR_DEF_(Region, RegionPtr);
_GF_PTR_DEF_(AttributesMutator, AttributesMutatorPtr);
_GF_PTR_DEF_(MapEntry, MapEntryPtr);
_GF_PTR_DEF_(RegionEntry, RegionEntryPtr);
_GF_PTR_DEF_(EventId, EventIdPtr);
_GF_PTR_DEF_(CacheStatistics, CacheStatisticsPtr);
_GF_PTR_DEF_(PersistenceManager, PersistenceManagerPtr);
_GF_PTR_DEF_(Properties, PropertiesPtr);
_GF_PTR_DEF_(FunctionService, FunctionServicePtr);
_GF_PTR_DEF_(CacheLoader, CacheLoaderPtr);
_GF_PTR_DEF_(CacheListener, CacheListenerPtr);
_GF_PTR_DEF_(CacheWriter, CacheWriterPtr);
_GF_PTR_DEF_(MembershipListener, MembershipListenerPtr);
_GF_PTR_DEF_(RegionAttributes, RegionAttributesPtr);
_GF_PTR_DEF_(CacheableDate, CacheableDatePtr);
_GF_PTR_DEF_(CacheableFileName, CacheableFileNamePtr);
_GF_PTR_DEF_(CacheableKey, CacheableKeyPtr);
_GF_PTR_DEF_(CacheableObjectArray, CacheableObjectArrayPtr);
_GF_PTR_DEF_(CacheableString, CacheableStringPtr);
_GF_PTR_DEF_(CacheableUndefined, CacheableUndefinedPtr);
_GF_PTR_DEF_(Serializable, SerializablePtr);
_GF_PTR_DEF_(PdxSerializable, PdxSerializablePtr);
_GF_PTR_DEF_(StackTrace, StackTracePtr);
_GF_PTR_DEF_(SelectResults, SelectResultsPtr);
_GF_PTR_DEF_(CqResults, CqResultsPtr);
_GF_PTR_DEF_(ResultSet, ResultSetPtr);
_GF_PTR_DEF_(StructSet, StructSetPtr);
_GF_PTR_DEF_(Struct, StructPtr);
_GF_PTR_DEF_(Query, QueryPtr);
_GF_PTR_DEF_(QueryService, QueryServicePtr);
_GF_PTR_DEF_(AuthInitialize, AuthInitializePtr);
_GF_PTR_DEF_(CqQuery, CqQueryPtr);
_GF_PTR_DEF_(CqListener, CqListenerPtr);
_GF_PTR_DEF_(CqAttributes, CqAttributesPtr);
_GF_PTR_DEF_(CqServiceStatistics, CqServiceStatisticsPtr);
_GF_PTR_DEF_(CqStatistics, CqStatisticsPtr);
_GF_PTR_DEF_(CqAttributesMutator, CqAttributesMutatorPtr);
_GF_PTR_DEF_(ClientHealthStats, ClientHealthStatsPtr);
_GF_PTR_DEF_(Pool, PoolPtr);
_GF_PTR_DEF_(PoolFactory, PoolFactoryPtr);
_GF_PTR_DEF_(PoolAttributes, PoolAttributesPtr);
_GF_PTR_DEF_(ResultCollector, ResultCollectorPtr);
_GF_PTR_DEF_(Execution, ExecutionPtr);
_GF_PTR_DEF_(Delta, DeltaPtr);
_GF_PTR_DEF_(PartitionResolver, PartitionResolverPtr);
_GF_PTR_DEF_(FixedPartitionResolver, FixedPartitionResolverPtr);
_GF_PTR_DEF_(CacheTransactionManager, CacheTransactionManagerPtr);
_GF_PTR_DEF_(TransactionId, TransactionIdPtr);
_GF_PTR_DEF_(EntryEvent, EntryEventPtr);
_GF_PTR_DEF_(PdxReader, PdxReaderPtr);
_GF_PTR_DEF_(PdxWriter, PdxWriterPtr);
_GF_PTR_DEF_(PdxWrapper, PdxWrapperPtr);
_GF_PTR_DEF_(PdxSerializer, PdxSerializerPtr);
_GF_PTR_DEF_(PdxInstanceFactory, PdxInstanceFactoryPtr);
_GF_PTR_DEF_(PdxInstance, PdxInstancePtr);
_GF_PTR_DEF_(WritablePdxInstance, WritablePdxInstancePtr);
_GF_PTR_DEF_(PdxUnreadFields, PdxUnreadFieldsPtr);
_GF_PTR_DEF_(CacheableEnum, CacheableEnumPtr);
_GF_PTR_DEF_(CqStatusListener, CqStatusListenerPtr);
_GF_PTR_DEF_(InternalCacheTransactionManager2PC,
             InternalCacheTransactionManager2PCPtr);
}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_TYPES_H_
