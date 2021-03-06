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

#include <geode/geode_globals.hpp>

#include <geode/DistributedSystem.hpp>
#include "statistics/StatisticsManager.hpp"
#include <geode/SystemProperties.hpp>

#include <CppCacheLibrary.hpp>
#include <Utils.hpp>
#include <geode/Log.hpp>
#include <geode/CacheFactory.hpp>
#include <ace/OS.h>

#include <ExpiryTaskManager.hpp>
#include <CacheImpl.hpp>
#include <ace/Guard_T.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <geode/DataOutput.hpp>
#include <TcrMessage.hpp>
#include <DistributedSystemImpl.hpp>
#include <RegionStats.hpp>
#include <PoolStatistics.hpp>

#include <DiffieHellman.hpp>

#include "version.h"

using namespace apache::geode::client;
using namespace apache::geode::statistics;

DistributedSystemPtr* DistributedSystem::m_instance_ptr = nullptr;
bool DistributedSystem::m_connected = false;
DistributedSystemImpl* DistributedSystem::m_impl = nullptr;

ACE_Recursive_Thread_Mutex* g_disconnectLock = new ACE_Recursive_Thread_Mutex();

namespace {

StatisticsManager* g_statMngr = nullptr;

SystemProperties* g_sysProps = nullptr;
}  // namespace

DistributedSystem::DistributedSystem(const char* name) : m_name(nullptr) {
  LOGDEBUG("DistributedSystem::DistributedSystem");
  if (name != nullptr) {
    size_t len = strlen(name) + 1;
    m_name = new char[len];
    ACE_OS::strncpy(m_name, name, len);
  }
  if (strlen(g_sysProps->securityClientDhAlgo()) > 0) {
    DiffieHellman::initOpenSSLFuncPtrs();
  }
}
DistributedSystem::~DistributedSystem() { GF_SAFE_DELETE_ARRAY(m_name); }

DistributedSystemPtr DistributedSystem::connect(
    const char* name, const PropertiesPtr& configPtr) {
  ACE_Guard<ACE_Recursive_Thread_Mutex> disconnectGuard(*g_disconnectLock);
  if (m_connected == true) {
    throw AlreadyConnectedException(
        "DistributedSystem::connect: already connected, call getInstance to "
        "get it");
  }

  // make sure statics are initialized.
  if (m_instance_ptr == nullptr) {
    m_instance_ptr = new DistributedSystemPtr();
  }
  if (g_sysProps == nullptr) {
    g_sysProps = new SystemProperties(configPtr, nullptr);
  }
  Exception::setStackTraces(g_sysProps->debugStackTraceEnabled());

  if (name == nullptr) {
    delete g_sysProps;
    g_sysProps = nullptr;
    throw IllegalArgumentException(
        "DistributedSystem::connect: "
        "name cannot be nullptr");
  }
  if (name[0] == '\0') {
    name = "NativeDS";
  }

  // Fix for Ticket#866 on NC OR SR#13306117704
  // Set client name via native client API
  const char* propName = g_sysProps->name();
  if (propName != nullptr && strlen(propName) > 0) {
    name = propName;
  }

  // Trigger other library initialization.
  CppCacheLibrary::initLib();

  if (!TcrMessage::init()) {
    TcrMessage::cleanup();
    throw IllegalArgumentException(
        "DistributedSystem::connect: preallocate message buffer failed!");
  }

  const char* logFilename = g_sysProps->logFilename();
  if (logFilename != nullptr) {
    try {
      Log::close();
      Log::init(g_sysProps->logLevel(), logFilename,
                g_sysProps->logFileSizeLimit(),
                g_sysProps->logDiskSpaceLimit());
    } catch (const GeodeIOException&) {
      Log::close();
      TcrMessage::cleanup();
      CppCacheLibrary::closeLib();
      delete g_sysProps;
      g_sysProps = nullptr;
      *m_instance_ptr = nullptr;
      // delete g_disconnectLock;
      throw;
    }
  } else {
    Log::setLogLevel(g_sysProps->logLevel());
  }

  try {
    std::string gfcpp = CppCacheLibrary::getProductDir();
    LOGCONFIG("Using Geode Native Client Product Directory: %s", gfcpp.c_str());
  } catch (const Exception&) {
    LOGERROR(
        "Unable to determine Product Directory. Please set the "
        "GFCPP environment variable.");
    throw;
  }
  // Add version information, source revision, current directory etc.
  LOGCONFIG("Product version: %s", CacheFactory::getProductDescription());
  LOGCONFIG("Source revision: %s", PRODUCT_SOURCE_REVISION);
  LOGCONFIG("Source repository: %s", PRODUCT_SOURCE_REPOSITORY);

  ACE_utsname u;
  ACE_OS::uname(&u);
  LOGCONFIG(
      "Running on: SystemName=%s Machine=%s Host=%s Release=%s Version=%s",
      u.sysname, u.machine, u.nodename, u.release, u.version);

#ifdef _WIN32
  const uint32_t pathMax = _MAX_PATH;
#else
  const uint32_t pathMax = PATH_MAX;
#endif
  ACE_TCHAR cwd[pathMax + 1];
  (void)ACE_OS::getcwd(cwd, pathMax);
  LOGCONFIG("Current directory: %s", cwd);
  LOGCONFIG("Current value of PATH: %s", ACE_OS::getenv("PATH"));
#ifndef _WIN32
  const char* ld_libpath = ACE_OS::getenv("LD_LIBRARY_PATH");
  LOGCONFIG("Current library path: %s",
            ld_libpath == nullptr ? "nullptr" : ld_libpath);
#endif
  // Log the Geode system properties
  g_sysProps->logSettings();

  /* if (strlen(g_sysProps->securityClientDhAlgo())>0) {
     DiffieHellman::initDhKeys(g_sysProps->getSecurityProperties());
   }*/

  DistributedSystemPtr dptr;
  try {
    g_statMngr = StatisticsManager::initInstance(
        g_sysProps->statisticsArchiveFile(),
        g_sysProps->statisticsSampleInterval(), g_sysProps->statisticsEnabled(),
        g_sysProps->statsFileSizeLimit(), g_sysProps->statsDiskSpaceLimit());
  } catch (const NullPointerException&) {
    // close all open handles, delete whatever was newed.
    g_statMngr = nullptr;
    //:Merge issue
    /*if (strlen(g_sysProps->securityClientDhAlgo())>0) {
      DiffieHellman::clearDhKeys();
    }*/
    Log::close();
    TcrMessage::cleanup();
    CppCacheLibrary::closeLib();
    delete g_sysProps;
    g_sysProps = nullptr;
    *m_instance_ptr = nullptr;
    // delete g_disconnectLock;
    throw;
  }
  GF_D_ASSERT(g_statMngr != nullptr);

  CacheImpl::expiryTaskManager = new ExpiryTaskManager();
  CacheImpl::expiryTaskManager->begin();

  DistributedSystem* dp = new DistributedSystem(name);
  if (!dp) {
    throw NullPointerException("DistributedSystem::connect: new failed");
  }
  m_impl = new DistributedSystemImpl(name, dp);

  try {
    m_impl->connect();
  } catch (const apache::geode::client::Exception& e) {
    LOGERROR("Exception caught during client initialization: %s",
             e.getMessage());
    std::string msg = "DistributedSystem::connect: caught exception: ";
    msg.append(e.getMessage());
    throw NotConnectedException(msg.c_str());
  } catch (const std::exception& e) {
    LOGERROR("Exception caught during client initialization: %s", e.what());
    std::string msg = "DistributedSystem::connect: caught exception: ";
    msg.append(e.what());
    throw NotConnectedException(msg.c_str());
  } catch (...) {
    LOGERROR("Unknown exception caught during client initialization");
    throw NotConnectedException(
        "DistributedSystem::connect: caught unknown exception");
  }

  m_connected = true;
  dptr.reset(dp);
  *m_instance_ptr = dptr;
  LOGCONFIG("Starting the Geode Native Client");

  return dptr;
}

/**
 *@brief disconnect from the distributed system
 */
void DistributedSystem::disconnect() {
  ACE_Guard<ACE_Recursive_Thread_Mutex> disconnectGuard(*g_disconnectLock);

  if (!m_connected) {
    throw NotConnectedException(
        "DistributedSystem::disconnect: connect "
        "not called");
  }

  try {
    CachePtr cache = CacheFactory::getAnyInstance();
    if (cache != nullptr && !cache->isClosed()) {
      cache->close();
    }
  } catch (const apache::geode::client::Exception& e) {
    LOGWARN("Exception while closing: %s: %s", e.getName(), e.getMessage());
  }

  if (CacheImpl::expiryTaskManager != nullptr) {
    CacheImpl::expiryTaskManager->stopExpiryTaskManager();
  }

  if (m_impl) {
    m_impl->disconnect();
    delete m_impl;
    m_impl = nullptr;
  }

  LOGFINEST("Deleted DistributedSystemImpl");

  if (strlen(g_sysProps->securityClientDhAlgo()) > 0) {
    //  DistributedSystem::getInstance()->m_dh->clearDhKeys();
  }

  // Clear DH Keys
  /* if (strlen(g_sysProps->securityClientDhAlgo())>0) {
     DiffieHellman::clearDhKeys();
   }*/

  GF_D_ASSERT(!!g_sysProps);
  delete g_sysProps;
  g_sysProps = nullptr;

  LOGFINEST("Deleted SystemProperties");

  if (CacheImpl::expiryTaskManager != nullptr) {
    delete CacheImpl::expiryTaskManager;
    CacheImpl::expiryTaskManager = nullptr;
  }

  LOGFINEST("Deleted ExpiryTaskManager");

  TcrMessage::cleanup();

  LOGFINEST("Cleaned TcrMessage");

  GF_D_ASSERT(!!g_statMngr);
  g_statMngr->clean();
  g_statMngr = nullptr;

  LOGFINEST("Cleaned StatisticsManager");

  RegionStatType::clean();

  LOGFINEST("Cleaned RegionStatType");

  PoolStatType::clean();

  LOGFINEST("Cleaned PoolStatType");

  *m_instance_ptr = nullptr;

  // Free up library resources
  CppCacheLibrary::closeLib();

  LOGCONFIG("Stopped the Geode Native Client");

  Log::close();

  m_connected = false;
}

SystemProperties* DistributedSystem::getSystemProperties() {
  return g_sysProps;
}

const char* DistributedSystem::getName() const { return m_name; }

bool DistributedSystem::isConnected() {
  CppCacheLibrary::initLib();
  return m_connected;
}

DistributedSystemPtr DistributedSystem::getInstance() {
  CppCacheLibrary::initLib();
  if (m_instance_ptr == nullptr) {
    return nullptr;
  }
  return *m_instance_ptr;
}
