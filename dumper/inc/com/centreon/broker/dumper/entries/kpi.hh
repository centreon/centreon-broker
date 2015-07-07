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
      static unsigned int static_type();

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
