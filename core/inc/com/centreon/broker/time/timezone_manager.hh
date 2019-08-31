/*
** Copyright 2014 Centreon
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

#ifndef CCB_CORE_TIME_TIMEZONE_MANAGER_HH
#  define CCB_CORE_TIME_TIMEZONE_MANAGER_HH

#  include <stack>
#  include <string>
#  include <mutex>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace time {
/**
 *  @class timezone_manager timezone_manager.hh "com/centreon/broker/time/timezone_manager.hh"
 *  @brief Manage timezone changes.
 *
 *  This class handle timezone change. This can either be setting a new
 *  timezone or restoring a previous one.
 */
  class                      timezone_manager {
  public:
    static void              load();
    void                     lock();
    void                     pop_timezone();
    void                     push_timezone(char const* tz);
    void                     unlock();
    static void              unload();

  /**
   *  Get class instance.
   *
   *  @return Class instance.
   */
    static timezone_manager& instance() {
      return (*_instance);
    }

  private:
    struct                   tz_info {
      bool                   is_set;
      std::string            tz_name;
    };

                             timezone_manager();
                             timezone_manager(
                               timezone_manager const& other);
                             ~timezone_manager();
    timezone_manager&        operator=(timezone_manager const& other);
    void                     _fill_tz_info(
                               tz_info* info,
                               char const* old_tz);
    void                     _set_timezone(
                               tz_info const& from,
                               tz_info const& to);

    tz_info                  _base;
    static timezone_manager* _instance;
    std::stack<tz_info>      _tz;
    std::recursive_mutex _timezone_manager_mutex;
  };
}

CCB_END()

#endif // !CCB_CORE_TIME_TIMEZONE_MANAGER_HH
