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

#ifndef CCB_BAM_DIMENSION_BA_TIMEPERIOD_RELATION_HH
#  define CCB_BAM_DIMENSION_BA_TIMEPERIOD_RELATION_HH

#  include <memory>
#  include <vector>
#  include <QSqlDatabase>
#  include <QSqlQuery>
#  include <QThread>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          bam {
  /**
   *  @class dimension_ba_timeperiod_relation dimension_ba_timeperiod_relation.hh "com/centreon/broker/bam/dimension_ba_timeperiod_relation.hh"
   *  @brief Dimension timeperiod ba relation
   *
   */
  class            dimension_ba_timeperiod_relation : public io::data {
  public:
                   dimension_ba_timeperiod_relation();
                   ~dimension_ba_timeperiod_relation();
                   dimension_ba_timeperiod_relation(dimension_ba_timeperiod_relation const&);
    dimension_ba_timeperiod_relation&
                   operator=(dimension_ba_timeperiod_relation const&);
    bool           operator==(dimension_ba_timeperiod_relation const& other) const;
    unsigned int   type() const;

    unsigned int   ba_id;
    unsigned int   timeperiod_id;
    bool           is_default;

  private:
    void           _internal_copy(dimension_ba_timeperiod_relation const& other);
  };
}

CCB_END()

#endif // !CCB_BAM_DIMENSION_BA_TIMEPERIOD_RELATION_HH
