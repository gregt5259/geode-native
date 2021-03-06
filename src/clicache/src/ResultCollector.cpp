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


//#include "geode_includes.hpp"
#include "ResultCollector.hpp"
#include "impl/ManagedString.hpp"
#include "ExceptionTypes.hpp"
#include "impl/SafeConvert.hpp"

using namespace System;

namespace Apache
{
  namespace Geode
  {
    namespace Client
    {

      namespace native = apache::geode::client;

      generic<class TResult>
      void ResultCollector<TResult>::AddResult( TResult rs )
      {
        _GF_MG_EXCEPTION_TRY2/* due to auto replace */

          try
          {
            auto result = std::shared_ptr<native::Cacheable>(SafeGenericMSerializableConvert((IGeodeSerializable^)rs));
            m_nativeptr->get()->addResult(result);
          }
          finally
          {
            GC::KeepAlive(m_nativeptr);
          }

        _GF_MG_EXCEPTION_CATCH_ALL2/* due to auto replace */
      }

      generic<class TResult>
      System::Collections::Generic::ICollection<TResult>^  ResultCollector<TResult>::GetResult()
      {
        return GetResult( DEFAULT_QUERY_RESPONSE_TIMEOUT );
      }

      generic<class TResult>
      System::Collections::Generic::ICollection<TResult>^  ResultCollector<TResult>::GetResult(UInt32 timeout)
      {
        _GF_MG_EXCEPTION_TRY2/* due to auto replace */
          try
          {
            auto results = m_nativeptr->get()->getResult(timeout);
            auto rs = gcnew array<TResult>(results->size());
            for (System::Int32 index = 0; index < results->size(); index++)
            {
              auto nativeptr = results->operator[](index);
              rs[index] = Serializable::GetManagedValueGeneric<TResult>(nativeptr);
            }
            auto collectionlist = (ICollection<TResult>^)rs;
            return collectionlist;
          }
          finally
          {
            GC::KeepAlive(m_nativeptr);
          }

        _GF_MG_EXCEPTION_CATCH_ALL2/* due to auto replace */
      }

      generic<class TResult>
      void ResultCollector<TResult>::EndResults()
      {
        _GF_MG_EXCEPTION_TRY2/* due to auto replace */
          try
          {
            m_nativeptr->get()->endResults();
          }
          finally
          {
            GC::KeepAlive(m_nativeptr);
          }
        _GF_MG_EXCEPTION_CATCH_ALL2/* due to auto replace */
      }

      generic<class TResult>
      void ResultCollector<TResult>::ClearResults(/*bool*/)
      {
        _GF_MG_EXCEPTION_TRY2/* due to auto replace */
          try
          {
            m_nativeptr->get()->clearResults();
          }
          finally
          {
            GC::KeepAlive(m_nativeptr);
          }
        _GF_MG_EXCEPTION_CATCH_ALL2/* due to auto replace */
      }
    }  // namespace Client
  }  // namespace Geode
}  // namespace Apache
