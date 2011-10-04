/*
** Copyright 2009-2011 Merethis
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

#ifndef CCB_CORRELATION_ISSUE_HH_
# define CCB_CORRELATION_ISSUE_HH_

# include <time.h>
# include "com/centreon/broker/io/data.hh"
# include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          correlation {
  /**
   *  @class issue issue.hh "com/centreon/broker/correlation/issue.hh"
   *  @brief Issue event.
   *
   *  Update or create an issue.
   */
  class            issue : public io::data {
   private:
    void           _internal_copy(issue const& i);

   public:
    time_t         ack_time;
    time_t         end_time;
    unsigned int   host_id;
    unsigned int   service_id;
    time_t         start_time;
                   issue();
                   issue(issue const& i);
                   ~issue();
    issue&         operator=(issue const& i);
    bool           operator==(issue const& i) const;
    bool           operator!=(issue const& i) const;
    QString const& type() const;
  };
}

CCB_END()

#endif /* !CCB_CORRELATION_ISSUE_HH_ */
