/*
** Copyright 2013,2015 Merethis
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

#ifndef CCB_EXTCMD_EXTERNAL_COMMAND_HH
#  define CCB_EXTCMD_EXTERNAL_COMMAND_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"

CCB_BEGIN()

namespace             extcmd {
  /**
   *  @class external_command external_command.hh "com/centreon/broker/extcmd/external_command.hh"
   *  @brief Represent an external command received.
   */
  class               external_command : public io::data {
  public:
                      external_command();
                      external_command(external_command const& right);
                      ~external_command();
    external_command& operator=(external_command const& right);
    unsigned int      type() const;
    static unsigned int
                      static_type();

    QString           command;

    static mapping::entry const
                      entries[];
    static io::event_info::event_operations const
                      operations;

  private:
    void              _internal_copy(external_command const& right);
  };
}

CCB_END()

#endif // !CCB_EXTCMD_EXTERNAL_COMMAND_HH
