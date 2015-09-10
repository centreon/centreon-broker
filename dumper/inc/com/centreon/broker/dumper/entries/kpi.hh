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

#ifndef CCB_DUMPER_ENTRIES_KPI_HH
#  define CCB_DUMPER_ENTRIES_KPI_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                 dumper {
  namespace               entries {
    /**
     *  @class kpi kpi.hh "com/centreon/broker/dumper/entries/kpi.hh"
     *  @brief KPI entry.
     *
     *  Some KPI row of the KPI configuration table.
     */
    class                 kpi : public io::data {
    public:
                          kpi();
                          kpi(kpi const& other);
                          ~kpi();
      kpi&                operator=(kpi const& other);
      bool                operator==(kpi const& other) const;
      bool                operator!=(kpi const& other) const;
      unsigned int        type() const;

      bool                enable;
      unsigned int        poller_id;
      unsigned int        kpi_id;
      short               kpi_type;
      unsigned int        ba_id;
      unsigned int        host_id;
      unsigned int        service_id;
      unsigned int        ba_indicator_id;
      unsigned int        meta_id;
      unsigned int        boolean_id;
      double              drop_warning;
      double              drop_critical;
      double              drop_unknown;

    private:
      void                _internal_copy(kpi const& other);
    };
  }
}

CCB_END()

#endif // !CCB_DUMPER_ENTRIES_KPI_HH
