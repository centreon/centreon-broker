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

#ifndef CCB_DUMPER_ENTRIES_DIFF_HH
#  define CCB_DUMPER_ENTRIES_DIFF_HH

#  include <list>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                            dumper {
  namespace                          entries {
    // Forward declarations.
    class                            ba;
    class                            kpi;
    class                            state;
    class                            host;
    class                            service;

    /**
     *  @class diff diff.hh "com/centreon/broker/dumper/entries/diff.hh"
     *  @brief Perform a diff on database entries.
     *
     *  Perform a diff on database entries.
     */
    class                            diff {
    public:
                                     diff();
                                     diff(
                                       state const& older,
                                       state const& newer);
                                     diff(diff const& other);
                                     ~diff();
      diff&                          operator=(diff const& other);
      std::list<ba> const&           bas_to_create() const;
      std::list<ba> const&           bas_to_update() const;
      std::list<ba> const&           bas_to_delete() const;
      std::list<kpi> const&          kpis_to_create() const;
      std::list<kpi> const&          kpis_to_update() const;
      std::list<kpi> const&          kpis_to_delete() const;
      std::list<host> const&         hosts_to_create() const;
      std::list<host> const&         hosts_to_update() const;
      std::list<host> const&         hosts_to_delete() const;
      std::list<service> const&      services_to_create() const;
      std::list<service> const&      services_to_update() const;
      std::list<service> const&      services_to_delete() const;

    private:
      void                           _internal_copy(diff const& other);

      std::list<ba>                  _bas_to_create;
      std::list<ba>                  _bas_to_update;
      std::list<ba>                  _bas_to_delete;
      std::list<kpi>                 _kpis_to_create;
      std::list<kpi>                 _kpis_to_update;
      std::list<kpi>                 _kpis_to_delete;
      std::list<host>                _hosts_to_create;
      std::list<host>                _hosts_to_update;
      std::list<host>                _hosts_to_delete;
      std::list<service>             _services_to_create;
      std::list<service>             _services_to_update;
      std::list<service>             _services_to_delete;

    };
  }
}

CCB_END()

#endif // !CCB_DUMPER_ENTRIES_DIFF_HH
