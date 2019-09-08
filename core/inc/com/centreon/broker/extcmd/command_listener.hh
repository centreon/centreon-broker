/*
** Copyright 2015-2016 Centreon
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

#ifndef CCB_EXTCMD_COMMAND_LISTENER_HH
#define CCB_EXTCMD_COMMAND_LISTENER_HH

#include <map>
#include <mutex>
#include "com/centreon/broker/extcmd/command_result.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace extcmd {
/**
 *  @class command_listener command_listener.hh
 * "com/centreon/broker/extcmd/command_listener.hh"
 *  @brief Command listener.
 *
 *  Listen to status of ongoing commands and provide them for some
 *  time. This is used by all command clients to fetch status of
 *  pending commands.
 */
class command_listener : public io::stream {
 public:
  command_listener();
  ~command_listener();
  command_listener(command_listener const& other) = delete;
  command_listener& operator=(command_listener const& other) = delete;
  command_result command_status(std::string const& command_uuid);
  bool read(std::shared_ptr<io::data>& d, time_t deadline = (time_t)-1);
  int write(std::shared_ptr<io::data> const& d);

 private:
  struct pending_command {
    time_t invalid_time;
    std::string uuid;
    int code;
    bool with_partial_result;
    std::list<std::string> msgs;
  };

  void _check_invalid();
  void _extract_command_result(command_result& res, pending_command& pending);

  time_t _next_invalid;
  std::map<std::string, pending_command> _pending;
  std::mutex _pendingm;
  static int const _request_timeout = 30;
  static int const _result_timeout = 60;
};
}  // namespace extcmd

CCB_END()

#endif  // !CCB_EXTCMD_COMMAND_LISTENER_HH
