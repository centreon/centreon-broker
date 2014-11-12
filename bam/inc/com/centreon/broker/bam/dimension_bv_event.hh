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

#ifndef CCB_BAM_DIMENSION_BV_EVENT_HH
#  define CCB_BAM_DIMENSION_BV_EVENT_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace                bam {
  /**
   *  @class dimension_bv_event dimension_bv_event.hh "com/centreon/broker/bam/dimension_bv_event.hh"
   *  @brief Dimension Bv event
   *
   */
  class                  dimension_bv_event : public io::data {
  public:
                         dimension_bv_event();
                         dimension_bv_event(dimension_bv_event const& other);
                         ~dimension_bv_event();
    dimension_bv_event&  operator=(dimension_bv_event const& other);
    bool                 operator==(dimension_bv_event const& other) const;
    unsigned int         type() const;

    unsigned int         bv_id;
    QString              bv_name;
    QString              bv_description;

  private:
    void         _internal_copy(dimension_bv_event const& other);
  };
}

CCB_END()

#endif // !CCB_BAM_DIMENSION_BV_EVENT_HH
