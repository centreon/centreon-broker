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

#ifndef CCB_BAM_BOOL_STATUS_HH
#  define CCB_BAM_BOOL_STATUS_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"

CCB_BEGIN()

namespace        bam {
  /**
   *  @class bool_status bool_status.hh "com/centreon/broker/bam/bool_status.hh"
   *  @brief Update status of a boolean expression.
   *
   *  Update the status of a boolean expression, used to update the
   *  mod_bam_boolean table.
   */
  class          bool_status : public io::data {
  public:
                 bool_status();
                 bool_status(bool_status const& other);
                 ~bool_status();
    bool_status& operator=(bool_status const& other);
    unsigned int type() const;
    static unsigned int
                 static_type();

    unsigned int bool_id;
    bool         state;

    static mapping::entry const
                   entries[];
    static io::event_info::event_operations const
                   operations;

  private:
    void         _internal_copy(bool_status const& other);
  };
}

CCB_END()

#endif // !CCB_BAM_BOOL_STATUS_HH
