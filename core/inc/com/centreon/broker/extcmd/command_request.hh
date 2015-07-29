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

#ifndef CCB_EXTCMD_COMMAND_REQUEST_HH
#  define CCB_EXTCMD_COMMAND_REQUEST_HH

#  include <QMutex>
#  include <QString>
#  include <string>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"

CCB_BEGIN()

namespace                extcmd {
  /**
   *  @class command_request command_request.hh "com/centreon/broker/extcmd/command_request.hh"
   *  @brief Represent an external command request.
   *
   *  This event is sent when an external command execution is
   *  requested.
   */
  class                  command_request : public io::data {
  public:
                         command_request();
                         command_request(command_request const& other);
                         ~command_request();
    command_request&     operator=(command_request const& other);
    bool                 is_addressed_to(
                           QString const& endp_name) const;
    void                 parse(std::string const& cmd);
    unsigned int         type() const;
    static unsigned int  static_type();

    QString              cmd;
    QString              endp;
    unsigned int         id;

    static mapping::entry const
                         entries[];
    static io::event_info::event_operations const
                         operations;

  private:
    void                 _internal_copy(command_request const& other);

    static unsigned int  _id;
    static QMutex        _mutex;
  };
}

CCB_END()

#endif // !CCB_EXTCMD_COMMAND_REQUEST_HH
