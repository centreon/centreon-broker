/*
** Copyright 2018 Centreon
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

#ifndef CCB_REDIS_REDISDB_HH
#  define CCB_REDIS_REDISDB_HH

#  include <QString>
#  include <sstream>
#  include <string>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/service_status.hh"
#  include "com/centreon/broker/neb/host_status.hh"

// Forward declarations
class QTcpSocket;

CCB_BEGIN()

namespace               redis {
  class                 redisdb {
    public:
                        redisdb(
                          std::string const& address,
                          unsigned short port,
                          std::string const& user,
                          std::string const& password);
                        ~redisdb();
    void                clear();
    redisdb&            operator<<(std::string const& str);
    redisdb&            operator<<(int val);
    redisdb&            operator<<(neb::host_status const& status);
    redisdb&            operator<<(neb::service_status const& status);
    std::string         str(std::string const& cmd = "");
    QString&            send_command(std::string const& cmd = "");
    QString&            mset();

   private:
    QTcpSocket*         _socket;
    std::string         _address;
    unsigned short      _port;
    std::string         _user;
    std::string         _password;
    unsigned int        _size;
    std::ostringstream  _content;
    QString             _result;
  };
}

CCB_END()

#endif // !CCB_REDIS_REDISDB_HH
