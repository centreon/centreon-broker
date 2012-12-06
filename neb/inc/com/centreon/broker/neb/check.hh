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

#ifndef CCB_NEB_CHECK_HH
#  define CCB_NEB_CHECK_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace   neb {
  /**
   *  @class check check.hh "com/centreon/broker/neb/check.hh"
   *  @brief Check that has been executed.
   *
   *  Once a check has been executed (the check itself, not
   *  deduced information), this kind of event is sent.
   *
   *  @see host_check
   *  @see service_check
   */
  class          check : public io::data {
  public:
                 check();
                 check(check const& c);
    virtual      ~check();
    check&       operator=(check const& c);

    bool         active_checks_enabled;
    short        check_type;
    QString      command_line;
    unsigned int host_id;
    timestamp    next_check;

  private:
    void         _internal_copy(check const& c);
  };
}

CCB_END()

#endif // !CCB_NEB_CHECK_HH
