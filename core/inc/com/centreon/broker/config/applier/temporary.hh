/*
** Copyright 2011-2013 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
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
