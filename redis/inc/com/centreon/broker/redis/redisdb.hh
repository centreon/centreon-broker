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
                          std::string const& password,
                          unsigned int queries_per_transaction,
                          std::string const& path);
                        ~redisdb();
    void                clear();
    redisdb&            operator<<(std::string const& str);
    redisdb&            operator<<(int val);
    void                push_array(size_t size);
    std::string         str(std::string const& cmd = "");
    std::string const&  get_content() const;
    static QVariantList parse(QByteArray const& array);
    std::string const&  get_address() const;
    unsigned short const
                        get_port() const;
    QByteArray&         send(bool force);

    // Redis Write commands
    void                push(instance_broadcast const& ib);
    void                push(neb::custom_variable const& cv);
    void                push(neb::custom_variable_status const& cvs);
    void                push(neb::host_group_member const& hgm);
    void                push(neb::host const& h);
    void                push(neb::host_status const& hs);
    void                push(neb::instance const& inst);
    void                push(neb::service_group_member const& sgm);
    void                push(neb::service const& s);
    void                push(neb::service_status const& ss);
    
    std::string         script_load(std::string const& fname);
    int                 del(std::string const& key);
    int                 unlink(std::string const& key);
    int                 hset(std::string const& key, std::string const& member,
                             std::string const& value);
    int                 set(std::string const& key, std::string const& value);
    int                 set(std::string const& key, int value);
    int                 incr(std::string const& key);
    int                 hmset(std::string const& key, int count);
    int                 sadd(std::string const& key, int item);
    int                 sadd(std::string const& key, std::string const& item);
    int                 setbit(std::string const& key,
                               long int row, int value);
    int                 srem(std::string const& key, std::string const& item);

    // Redis Read commands
    int                 get(std::string const& key);
    int                 getbit(std::string const& key, long long index);
    int                 hget(std::string const& key, std::string const& item);
    int                 hgetall(std::string const& key);
    int                 hmget(std::string const& key, int count, ...);
    int                 info(std::string const& size);
    int                 keys(std::string const& pattern);
    int                 module(std::string const& arg);
    int                 smembers(std::string const& key);
    int                 sismember(std::string const& key,
                                  std::string const& item);

   private:
    void                _check_validity() const;
    void                _connect();
    void                _check_redis_server(std::string const& path);
    void                _init_redis_scripts(std::string const& path);
    static QVariant     _parse(QByteArray const& str, QByteArray::const_iterator& it);
    static QVariant     _parse_array(QByteArray const& str, QByteArray::const_iterator& it);
    static QVariant     _parse_int(QByteArray const& str, QByteArray::const_iterator& it);
    static QVariant     _parse_str(QByteArray const& str, QByteArray::const_iterator& it);
    static QVariant     _parse_simple_str(QByteArray const& str, QByteArray::const_iterator& it);

    QTcpSocket*         _socket;
    std::string         _address;
    unsigned short      _port;
    std::string         _password;
    unsigned int        _queries_per_transaction;
    unsigned int        _size;
    std::string         _content;
    QByteArray          _result;
    umap<std::string, std::string>
                        _script;
  };
}

CCB_END()

#endif // !CCB_REDIS_REDISDB_HH
