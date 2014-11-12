/*
** Copyright 2014 Merethis
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

#ifndef CCB_BAM_INTERNAL_HH
#  define CCB_BAM_INTERNAL_HH

#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
  // Data elements.
  enum data_element {
    de_ba_status = 1,
    de_bool_status,
    de_kpi_status,
    de_meta_service_status,
    de_ba_event,
    de_kpi_event,
    de_ba_duration_event,
    de_dimension_ba_event,
    de_dimension_kpi_event,
    de_dimension_ba_bv_relation_event,
    de_dimension_bv_event,
    de_dimension_truncate_table_signal,
    de_rebuild,
    de_dimension_timeperiod,
    de_dimension_ba_timeperiod_relation
  };
}

CCB_END()

#endif // !CCB_BAM_INTERNAL_HH
