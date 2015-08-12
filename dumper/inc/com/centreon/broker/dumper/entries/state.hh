/*
** Copyright 2015 Merethis
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

#ifndef CCB_DUMPER_ENTRIES_STATE_HH
#  define CCB_DUMPER_ENTRIES_STATE_HH

#  include <list>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                   dumper {
  namespace                 entries {
    // Forward declarations.
    class                   ba;
    class                   kpi;
    class                   host;
    class                   service;

    /**
     *  @class state state.hh "com/centreon/broker/dumper/entries/dumper.hh"
     *  @brief Database state.
     *
     *  Holds all synchronizable entries of configuration database.
     */
    class                   state {
    public:
                            state();
                            state(state const& other);
                            ~state();
      state&                operator=(state const& other);
      std::list<ba> const&  get_bas() const;
      std::list<ba>&        get_bas();
      std::list<kpi> const& get_kpis() const;
      std::list<kpi>&       get_kpis();
      std::list<host> const&
                            get_hosts() const;
      std::list<host>&      get_hosts();
      std::list<service> const&
                            get_services() const;
      std::list<service>&   get_services();

    private:
      void                  _internal_copy(state const& other);

      std::list<ba>         _bas;
      std::list<kpi>        _kpis;
      std::list<host>       _hosts;
      std::list<service>    _services;
    };
  }
}

CCB_END()

#endif // !CCB_DUMPER_ENTRIES_STATE_HH
