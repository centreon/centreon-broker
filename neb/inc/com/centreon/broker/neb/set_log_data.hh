/*
** Copyright 2009-2011 Centreon
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

#ifndef CCB_NEB_SET_LOG_DATA_HH_
# define CCB_NEB_SET_LOG_DATA_HH_

# include "com/centreon/broker/neb/log_entry.hh"
# include "com/centreon/engine/host.hh"
# include "com/centreon/engine/service.hh"

namespace       com {
  namespace     centreon {
    namespace   broker {
      namespace neb {
        void    set_log_data(neb::log_entry& le, char const* log_data);
      }
    }
  }
}

#endif /* !CCB_NEB_SET_LOG_DATA_HH_ */
