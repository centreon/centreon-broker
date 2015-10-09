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

#ifndef CCB_DUMPER_ENTRIES_DIFF_HH
#  define CCB_DUMPER_ENTRIES_DIFF_HH

#  include <list>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                            dumper {
  namespace                          entries {
    // Forward declarations.
    class                            ba;
    class                            ba_type;
    class                            boolean;
    class                            host;
    class                            kpi;
    class                            organization;
    class                            service;
    class                            state;

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
      std::list<ba_type> const&      ba_types_to_create() const;
      std::list<ba_type> const&      ba_types_to_update() const;
      std::list<ba_type> const&      ba_types_to_delete() const;
      std::list<ba> const&           bas_to_create() const;
      std::list<ba> const&           bas_to_update() const;
      std::list<ba> const&           bas_to_delete() const;
      std::list<boolean> const&      booleans_to_create() const;
      std::list<boolean> const&      booleans_to_update() const;
      std::list<boolean> const&      booleans_to_delete() const;
      std::list<host> const&         hosts_to_create() const;
      std::list<host> const&         hosts_to_update() const;
      std::list<host> const&         hosts_to_delete() const;
      std::list<kpi> const&          kpis_to_create() const;
      std::list<kpi> const&          kpis_to_update() const;
      std::list<kpi> const&          kpis_to_delete() const;
      std::list<organization> const& organizations_to_create() const;
      std::list<organization> const& organizations_to_update() const;
      std::list<organization> const& organizations_to_delete() const;
      std::list<service> const&      services_to_create() const;
      std::list<service> const&      services_to_update() const;
      std::list<service> const&      services_to_delete() const;

    private:
      void                           _internal_copy(diff const& other);

      std::list<ba_type>             _ba_types_to_create;
      std::list<ba_type>             _ba_types_to_update;
      std::list<ba_type>             _ba_types_to_delete;
      std::list<ba>                  _bas_to_create;
      std::list<ba>                  _bas_to_update;
      std::list<ba>                  _bas_to_delete;
      std::list<boolean>             _booleans_to_create;
      std::list<boolean>             _booleans_to_update;
      std::list<boolean>             _booleans_to_delete;
      std::list<host>                _hosts_to_create;
      std::list<host>                _hosts_to_update;
      std::list<host>                _hosts_to_delete;
      std::list<kpi>                 _kpis_to_create;
      std::list<kpi>                 _kpis_to_update;
      std::list<kpi>                 _kpis_to_delete;
      std::list<organization>        _organizations_to_create;
      std::list<organization>        _organizations_to_update;
      std::list<organization>        _organizations_to_delete;
      std::list<service>             _services_to_create;
      std::list<service>             _services_to_update;
      std::list<service>             _services_to_delete;
    };
  }
}

CCB_END()

#endif // !CCB_DUMPER_ENTRIES_DIFF_HH
