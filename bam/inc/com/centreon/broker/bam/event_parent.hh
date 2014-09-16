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

#ifndef CCB_BAM_EVENT_PARENT_HH
#  define CCB_BAM_EVENT_PARENT_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        bam {
  /**
   *  @class event_parent event_parent.hh "com/centreon/broker/bam/event_parent.hh"
   *  @brief Parent event
   *
   *  An event linking kpi and ba events.
   */
  class           event_parent : public io::data {
  public:
                  event_parent();
                  event_parent(event_parent const& other);
                  ~event_parent();
    event_parent& operator=(event_parent const& other);
    unsigned int  type() const;

    unsigned int  kpi_id;
    unsigned int  ba_id;

  private:
    void          _internal_copy(event_parent const& other);
  };
}

CCB_END()

#endif // !CCB_BAM_EVENT_PARENT_HH
