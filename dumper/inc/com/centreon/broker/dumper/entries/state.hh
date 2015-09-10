/*
** Copyright 2015 Centreon
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
    class                   boolean;

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
      std::list<boolean> const&
                            get_booleans() const;
      std::list<boolean>&   get_booleans();
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
      std::list<boolean>    _booleans;
      std::list<kpi>        _kpis;
      std::list<host>       _hosts;
      std::list<service>    _services;
    };
  }
}

CCB_END()

#endif // !CCB_DUMPER_ENTRIES_STATE_HH
