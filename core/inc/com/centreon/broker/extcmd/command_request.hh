/*
** Copyright 2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_EXTCMD_COMMAND_REQUEST_HH
#  define CCB_EXTCMD_COMMAND_REQUEST_HH

#  include <QMutex>
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
                           std::string const& endp_name) const;
    void                 parse(std::string const& cmd);
    unsigned int         type() const;
    static unsigned int  static_type();

    std::string              cmd;
    std::string              endp;
    std::string              uuid;
    bool                 with_partial_result;

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
