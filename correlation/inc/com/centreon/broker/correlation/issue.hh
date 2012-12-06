/*
** Copyright 2009-2012 Merethis
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

#ifndef CCB_CORRELATION_ISSUE_HH
#  define CCB_CORRELATION_ISSUE_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace          correlation {
  /**
   *  @class issue issue.hh "com/centreon/broker/correlation/issue.hh"
   *  @brief Issue event.
   *
   *  Update or create an issue.
   */
  class            issue : public io::data {
  public:
                   issue();
                   issue(issue const& i);
                   ~issue();
    issue&         operator=(issue const& i);
    bool           operator==(issue const& i) const;
    bool           operator!=(issue const& i) const;
    QString const& type() const;

    timestamp      ack_time;
    timestamp      end_time;
    unsigned int   host_id;
    unsigned int   service_id;
    timestamp      start_time;

  private:
    void           _internal_copy(issue const& i);
  };
}

CCB_END()

#endif // !CCB_CORRELATION_ISSUE_HH
