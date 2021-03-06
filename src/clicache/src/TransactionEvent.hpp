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

#ifdef CSTX_COMMENTED
#pragma once

#include "geode_defs.hpp"
#include <cppcache/TransactionEvent.hpp>

//#include "TransactionId.hpp"
//#include "Cache.hpp"

using namespace System;

namespace Apache
{
  namespace Geode
  {
    namespace Client
    {

				ref class TransactionId;
				ref class Cache;

				generic<class TKey, class TValue>
        ref class EntryEvent;

        /// <summary>
        /// This class encapsulates events that occur for an transaction in a cache.
        /// </summary>
        generic<class TKey, class TValue>
        public ref class TransactionEvent sealed
          : public Internal::UMWrap<apache::geode::client::TransactionEvent>
        {
        public:
          /// <summary>
          /// Gets the transaction id for this transaction.
          /// </summary>
					property Apache::Geode::Client::TransactionId^ TransactionId
          {
						Apache::Geode::Client::TransactionId^ get( );
          }

        	/// <summary>
          /// Returns an ordered list of every event for this transaction.
	        /// The event order is consistent with the order in which the operations were
	        /// performed during the transaction.
          /// </summary>
          property array<EntryEvent<TKey, TValue>^>^ Events
          {
            array<EntryEvent<TKey, TValue>^>^ get( );
          }
          
          /// <summary>
          /// Gets the Cache for this transaction event
          /// </summary>
					property Apache::Geode::Client::Cache^ Cache
          {
            Apache::Geode::Client::Cache^ get( );
          }

        internal:
          /// <summary>
          /// Internal constructor to wrap a native object pointer
          /// </summary>
          /// <param name="nativeptr">The native object pointer</param>
          inline TransactionEvent( apache::geode::client::TransactionEvent* nativeptr )
            : UMWrap( nativeptr, false ) { }
        };
    }  // namespace Client
  }  // namespace Geode
}  // namespace Apache
 //namespace 
#endif
