/*
** Copyright 2015 Merethis
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

#ifndef CCB_EXTCMD_COMMAND_RESULT_HH
#  define CCB_EXTCMD_COMMAND_RESULT_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                extcmd {
  /**
   *  @class command_result command_result.hh "com/centreon/broker/extcmd/command_result.hh"
   *  @brief Represent an external command result.
   *
   *  This event is sent when an external command was executed, either
   *  successfully or not.
   */
  class                  command_result : public io::data {
  public:
                         command_result();
                         command_result(command_result const& other);
                         ~command_result();
    command_result&      operator=(command_result const& other);
    unsigned int         type() const;
    static unsigned int  static_type();

    int                  code;
    unsigned int         id;
    QString              msg;

  private:
    void                 _internal_copy(command_result const& other);
  };
}

CCB_END()

#endif // !CCB_EXTCMD_COMMAND_RESULT_HH
