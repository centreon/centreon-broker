/*
** Copyright 2012-2013 Centreon
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

#ifndef CC_CONCURRENCY_THREAD_HH
#define CC_CONCURRENCY_THREAD_HH

#ifdef _WIN32
#include "com/centreon/concurrency/thread_win32.hh"
#else
#include "com/centreon/concurrency/thread_posix.hh"
#endif  // Windows or POSIX implementation.

#endif  // !CC_CONCURRENCY_THREAD_HH
