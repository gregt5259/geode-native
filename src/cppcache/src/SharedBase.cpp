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

#include <geode/SharedBase.hpp>

#include <atomic>
#include <memory>
#include <functional>

namespace apache {
namespace geode {
namespace client {

using Counter = std::atomic<int32_t>;

SharedBase::SharedBase() : m_refCount((void*) new Counter(0), [](void* ptr){
  delete reinterpret_cast<Counter*>(ptr);
}) {}

int32_t SharedBase::refCount() { return *reinterpret_cast<Counter*>(m_refCount.get()); }

void SharedBase::preserveSB() const {
  ++*reinterpret_cast<Counter*>(this->m_refCount.get());
}

void SharedBase::releaseSB() const {
  if (--*reinterpret_cast<Counter*>(this->m_refCount.get()) == 0) {
    delete this;
  }
}

// dummy instance to use for NULLPTR
const NullSharedBase* const NullSharedBase::s_instancePtr = NULL;
}  // namespace client
}  // namespace geode
}  // namespace apache
