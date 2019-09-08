/*
** Copyright 2011-2013 Centreon
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

#ifndef CC_CLIB_HH
#define CC_CLIB_HH

#include "com/centreon/namespace.hh"

CC_BEGIN()

/**
 *  @class clib clib.hh "com/centreon/clib.hh"
 *  @brief Initialize all clib manager.
 *
 *  Initialize all clib manager and make base configuration.
 */
class clib {
 public:
  enum load_flags {
    with_logging_engine = 1 << 0,
    with_process_manager = 1 << 1
  };

  static void load(unsigned int flags = with_logging_engine |
                                        with_process_manager);
  static void unload();

 private:
  clib(unsigned int flags);
  clib(clib const& right);
  ~clib() throw();
  clib& operator=(clib const& right);
};

CC_END()

#endif  // !CC_CLIB_HH
