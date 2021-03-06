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

#pragma once

#ifndef GEODE_EVENTIDMAP_H_
#define GEODE_EVENTIDMAP_H_

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include <utility>

#include <ace/ACE.h>
#include <ace/Time_Value.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Guard_T.h>

#include <geode/utils.hpp>
#include <memory>

#include "EventId.hpp"
#include "EventSource.hpp"

namespace apache {
namespace geode {
namespace client {

class EventSequence;
class EventIdMap;

typedef std::shared_ptr<EventSequence> EventSequencePtr;
typedef std::shared_ptr<EventIdMap> EventIdMapPtr;

typedef std::pair<EventSourcePtr, EventSequencePtr> EventIdMapEntry;
typedef std::vector<EventIdMapEntry> EventIdMapEntryList;

typedef ACE_Guard<ACE_Recursive_Thread_Mutex> MapGuard;

#define GUARD_MAP MapGuard mapguard(m_lock)

/** @class EventIdMap EventIdMap.hpp
 *
 * This is the class that encapsulates a HashMap and
 * provides the operations for duplicate checking and
 * expiry of idle event IDs from notifications.
 */
class CPPCACHE_EXPORT EventIdMap {
 private:
  typedef std::unordered_map<EventSourcePtr, EventSequencePtr,
                             dereference_hash<EventSourcePtr>,
                             dereference_equal_to<EventSourcePtr>>
      map_type;

  int32_t m_expiry;
  map_type m_map;
  ACE_Recursive_Thread_Mutex m_lock;

  // hidden
  EventIdMap(const EventIdMap &);
  EventIdMap &operator=(const EventIdMap &);

 public:
  EventIdMap() : m_expiry(0){};

  void clear();

  /** Initialize with preset expiration time in seconds */
  void init(int32_t expirySecs);

  ~EventIdMap();

  /** Find out if entry is duplicate
   * @return true if the entry exists else false
   */
  bool isDuplicate(EventSourcePtr key, EventSequencePtr value);

  /** Construct an EventIdMapEntry from an EventIdPtr */
  static EventIdMapEntry make(EventIdPtr eventid);

  /** Put an item and return true if it is new or false if it existed and was
   * updated
   * @param onlynew Only put if the sequence id does not exist or is higher
   * @return true if the entry was updated or inserted otherwise false
   */
  bool put(EventSourcePtr key, EventSequencePtr value, bool onlynew = false);

  /** Update the deadline for the entry
   * @return true if the entry exists else false
   */
  bool touch(EventSourcePtr key);

  /** Remove an item from the map
   *  @return true if the entry was found and removed else return false
   */
  bool remove(EventSourcePtr key);

  /** Collect all map entries who acked flag is false and set their acked flags
   * to true */
  EventIdMapEntryList getUnAcked();

  /** Clear all acked flags in the list and return the number of entries cleared
   * @param entries List of entries whos flags are to be cleared
   * @return The number of entries whos flags were cleared
   */
  uint32_t clearAckedFlags(EventIdMapEntryList &entries);

  /** Remove entries whos deadlines have passed and return the number of entries
   * removed
   * @param onlyacked Either check only entries whos acked flag is true
   * otherwise check all entries
   * @return The number of entries removed
   */
  uint32_t expire(bool onlyacked);
};

/** @class EventSequence
 *
 * EventSequence is the combination of SequenceNum from EventId, a timestamp and
 * a flag indicating whether or not it is ACKed
 */
class CPPCACHE_EXPORT EventSequence {
  int64_t m_seqNum;
  bool m_acked;
  ACE_Time_Value m_deadline;  // current time plus the expiration delay (age)

  void init();

 public:
  void clear();

  EventSequence();
  EventSequence(int64_t seqNum);
  ~EventSequence();

  void touch(int32_t ageSecs);  // update deadline
  void touch(
      int64_t seqNum,
      int32_t ageSecs);  // update deadline, clear acked flag and set seqNum

  // Accessors:

  int64_t getSeqNum();
  void setSeqNum(int64_t seqNum);

  bool getAcked();
  void setAcked(bool acked);

  ACE_Time_Value getDeadline();
  void setDeadline(ACE_Time_Value deadline);

  bool operator<=(const EventSequence &rhs) const;
};
}  // namespace client
}  // namespace geode
}  // namespace apache
#endif  // GEODE_EVENTIDMAP_H_
