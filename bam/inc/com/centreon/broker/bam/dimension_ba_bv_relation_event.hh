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

#ifndef CCB_BAM_DIMENSION_BA_BV_RELATION_EVENT_HH
#  define CCB_BAM_DIMENSION_BA_BV_RELATION_EVENT_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace                bam {
  /**
   *  @class dimension_ba_bv_relation_event dimension_ba_bv_relation_event.hh "com/centreon/broker/bam/dimension_ba_bv_relation_event.hh"
   *  @brief Dimension Ba Bv relation event
   *
   */
  class                  dimension_ba_bv_relation_event : public io::data {
  public:
                         dimension_ba_bv_relation_event();
                         dimension_ba_bv_relation_event(
                           dimension_ba_bv_relation_event const& other);
                         ~dimension_ba_bv_relation_event();
    dimension_ba_bv_relation_event&
                         operator=(
                           dimension_ba_bv_relation_event const& other);
    bool                 operator==(
                           dimension_ba_bv_relation_event const& other) const;
    unsigned int         type() const;

    unsigned int         ba_id;
    unsigned int         bv_id;

  private:
    void                 _internal_copy(
                           dimension_ba_bv_relation_event const& other);
  };
}

CCB_END()

#endif // !CCB_BAM_DIMENSION_BA_BV_RELATION_EVENT_HH
