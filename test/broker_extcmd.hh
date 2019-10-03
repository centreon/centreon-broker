/*
** Copyright 2015 Centreon
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

#ifndef TEST_BROKER_EXTCMD_HH
#define TEST_BROKER_EXTCMD_HH

#include <QLocalSocket>
#include <string>

/**
 *  @class broker_extcmd broker_extcmd.hh "test/broker_extcmd.hh"
 *  @brief Interface between Broker and users.
 *
 *  Handle Centreon Broker external command file access.
 */
class broker_extcmd {
 public:
  broker_extcmd();
  broker_extcmd(broker_extcmd const& other);
  ~broker_extcmd();
  broker_extcmd& operator=(broker_extcmd const& other);
  bool execute(std::string const& query, bool wait_command = true);
  std::string const& get_file() const throw();
  void set_file(std::string const& file);

 private:
  void _internal_copy(broker_extcmd const& other);
  void _read(QLocalSocket& sockt,
             uint32_t& id,
             bool& pending,
             std::string& msg,
             std::string const& query = std::string());
  void _write(QLocalSocket& sockt, std::string const& query);

  std::string _file;
};

#endif  // !TEST_BROKER_EXTCMD_HH
