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
#  include <QVariant>
#  include <sstream>
#  include <string>
#  include "com/centreon/broker/instance_broadcast.hh"
#  include "com/centreon/broker/neb/custom_variable.hh"
#  include "com/centreon/broker/neb/host.hh"
#  include "com/centreon/broker/neb/host_group_member.hh"
#  include "com/centreon/broker/neb/instance.hh"
#  include "com/centreon/broker/neb/service.hh"
#  include "com/centreon/broker/neb/service_group_member.hh"

// Forward declarations
class QTcpSocket;

CCB_BEGIN()

namespace               redis {
  class                 redisdb {
    public:
                        redisdb(
                          std::string const& address,
                          unsigned short port,
                          std::string const& password);
                        ~redisdb();
    void                clear();
    redisdb&            operator<<(std::string const& str);
    redisdb&            operator<<(int val);
    QString&            push(instance_broadcast const& ib);
    QString&            push(neb::custom_variable const& cv);
    QString&            push(neb::host_group_member const& hgm);
    QString&            push(neb::host const& h);
    QString&            push(neb::host_status const& hs);
    QString&            push(neb::instance const& inst);
    QString&            push(neb::service_group_member const& sgm);
    QString&            push(neb::service const& s);
    QString&            push(neb::service_status const& ss);
    std::string         str(std::string const& cmd = "");

    QString&            push_command(std::string const& cmd = "");
    QString&            del();
    std::string const&  get_content() const;
    static QVariant     parse(QString const& str);
    std::string const&  get_address() const;
    unsigned short const
                        get_port() const;

   private:
    void                _connect();
    void                _check_redis_server();
    void                _check_redis_documents();
    static QVariant     _parse(QString const& str, QString::const_iterator& it);
    static QVariant     _parse_array(QString const& str, QString::const_iterator& it);
    static QVariant     _parse_int(QString const& str, QString::const_iterator& it);
    static QVariant     _parse_str(QString const& str, QString::const_iterator& it);

    QTcpSocket*         _socket;
    std::string         _address;
    unsigned short      _port;
    std::string         _password;
    unsigned int        _size;
    std::ostringstream  _oss;
    std::string         _content;
    QString             _result;
  };
}

CCB_END()

#endif // !CCB_REDIS_REDISDB_HH
