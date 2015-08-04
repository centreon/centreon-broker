/*
** Copyright 2011-2012,2015 Merethis
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

#ifndef CCB_CONFIG_APPLIER_STATE_HH
#  define CCB_CONFIG_APPLIER_STATE_HH

#  include <string>
#  include "com/centreon/broker/config/state.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                config {
  namespace              applier {
    /**
     *  @class state state.hh "com/centreon/broker/config/applier/state.hh"
     *  @brief Apply a configuration.
     *
     *  Apply some configuration state.
     */
    class                state {
    public:
                         ~state();
      void               apply(
                           config::state const& s,
                           bool run_mux = true);
      std::string const& cache_dir() const throw ();
      static state&      instance();
      static void        load();
      unsigned int       poller_id() const throw ();
      std::string const& poller_name() const throw ();
      static void        unload();

    private:
                         state();
                         state(state const& other);
      state&             operator=(state const& other);

      std::string        _cache_dir;
      unsigned int       _poller_id;
      std::string        _poller_name;
    };
  }
}

CCB_END()

#endif // !CCB_CONFIG_APPLIER_STATE_HH
