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

#ifndef CCB_BAM_DIMENSION_TRUNCATE_TABLE_SIGNAL_HH
#  define CCB_BAM_DIMENSION_TRUNCATE_TABLE_SIGNAL_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"

CCB_BEGIN()

namespace                bam {
  /**
   *  @class dimension_truncate_table_signal dimension_truncate_table_signal.hh "com/centreon/broker/bam/dimension_truncate_table_signal.hh"
   *  @brief Dimension truncate table signal
   *
   *  This empty data signals the reporting stream
   *  to truncate the dimension tables.
   */
  class                  dimension_truncate_table_signal : public io::data {
  public:
                         dimension_truncate_table_signal();
                         dimension_truncate_table_signal(
                           dimension_truncate_table_signal const& other);
                         ~dimension_truncate_table_signal();
    dimension_truncate_table_signal&
                         operator=(
                           dimension_truncate_table_signal const& other);
    bool                 operator==(
                           dimension_truncate_table_signal const& other) const;
    unsigned int         type() const;
    static unsigned int  static_type();

    bool                 update_started;

    static mapping::entry const
                         entries[];
    static io::event_info::event_operations const
                         operations;

  private:
    void                 _internal_copy(
                           dimension_truncate_table_signal const& other);
  };
}

CCB_END()

#endif // !CCB_BAM_DIMENSION_TRUNCATE_TABLE_SIGNAL_HH
