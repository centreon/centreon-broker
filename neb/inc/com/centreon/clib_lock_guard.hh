/*
** Copyright 2020 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CC_CLIB_LOCK_GUARD_HH
#define CC_CLIB_LOCK_GUARD_HH
#include <cstdio>

#define DEBUG_CLIB

#ifdef DEBUG_CLIB
template<typename M>
class clib_lock_guard : public std::lock_guard<M> {
  static const constexpr char* filename = "/tmp/lock.log";
  const char* _msg;

 public:
  explicit clib_lock_guard<M>(M& m, const char* msg) : std::lock_guard<M>(m), _msg(msg) {
    FILE* f = fopen(filename, "a+");
    fprintf(f, "lock_guard lock mutex '%s'\n", _msg);
    fclose(f);
  }

  ~clib_lock_guard<M>() {
    FILE* f = fopen(filename, "a+");
    fprintf(f, "lock_guard unlock mutex '%s'\n", _msg);
    fclose(f);
  }
};

template<typename M>
class clib_unique_lock : public std::unique_lock<M> {
  static const constexpr char* filename = "/tmp/lock.log";
  const char* _msg;

 public:
  explicit clib_unique_lock<M>(M& m, const char* msg) : std::unique_lock<M>(m), _msg(msg) {
    FILE* f = fopen(filename, "a+");
    fprintf(f, "unique_lock lock mutex '%s'\n", _msg);
    fclose(f);
  }

  ~clib_unique_lock<M>() {
    FILE* f = fopen(filename, "a+");
    fprintf(f, "unique_lock unlock mutex '%s'\n", _msg);
    fclose(f);
  }

  void lock() {
    FILE* f = fopen(filename, "a+");
    fprintf(f, "unique_lock lock() mutex '%s'\n", _msg);
    fclose(f);
    std::unique_lock<M>::lock();
  }

  void unlock() {
    FILE* f = fopen(filename, "a+");
    fprintf(f, "unique_lock unlock() mutex '%s'\n", _msg);
    fclose(f);
    std::unique_lock<M>::unlock();
  }
};
#else
template<typename M>
class clib_lock_guard : public std::lock_guard<M> {
 public:
  explicit clib_lock_guard<M>(M& m, const char* msg __attribute__((unused))) : std::lock_guard<M>(m) {
  }
};

template<typename M>
class clib_unique_lock : public std::unique_lock<M> {
 public:
  explicit clib_unique_lock<M>(M& m, const char* msg __attribute__((unused))) : std::unique_lock<M>(m) {
  }
};
#endif

#endif /* !CC_CLIB_LOCK_GUARD_HH */
