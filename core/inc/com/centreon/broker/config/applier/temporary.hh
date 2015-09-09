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

#ifndef CCB_CONFIG_APPLIER_TEMPORARY_HH
#  define CCB_CONFIG_APPLIER_TEMPORARY_HH

#  include "com/centreon/broker/config/endpoint.hh"
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace           config {
  namespace         applier {
    /**
     *  @class temporary temporary.hh "com/centreon/broker/config/applier/temporary.hh"
     *  @brief Apply a configuration.
     *
     *  Apply some temporary configuration.
     */
    class               temporary {
    public:
      void              apply(config::endpoint cfg);
      static temporary& instance();
      static void       load();
      static void       unload();

    private:
                        temporary();
                        temporary(temporary const& right);
                        ~temporary();
      temporary&        operator=(temporary const& right);
      misc::shared_ptr<io::endpoint>
                        _create_temporary(config::endpoint& cfg);

      config::endpoint  _cfg;
    };
  }
}

CCB_END()

#endif // !CCB_CONFIG_APPLIER_TEMPORARY_HH
