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

#ifndef CCB_STORAGE_METRIC_MAPPING_HH
#  define CCB_STORAGE_METRIC_MAPPING_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"
#  include "com/centreon/broker/mapping/entry.hh"

CCB_BEGIN()

namespace          storage {
  /**
   *  @class metric_mapping metric_mapping.hh "com/centreon/broker/storage/metric_mapping.hh"
   *  @brief Information about a metric stored in the database.
   *
   *  Used to provide more informations about the mapping of
   *  the metrics to status.
   */
  class            metric_mapping : public io::data {
  public:
                   metric_mapping();
                   metric_mapping(metric_mapping const& s);
                   ~metric_mapping();
    metric_mapping&
                   operator=(metric_mapping const& s);
    unsigned int   type() const;
    static unsigned int
                   static_type();

    unsigned int   metric_id;
    unsigned int   status_id;

    static mapping::entry const
                   entries[];
    static io::event_info::event_operations const
                   operations;

  private:
    void           _internal_copy(metric_mapping const& s);
  };
}

CCB_END()

#endif // !CCB_STORAGE_METRIC_MAPPING_HH
