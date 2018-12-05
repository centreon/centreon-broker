/*
** Copyright 2014-2015 Centreon
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

#ifndef CCB_DATABASE_CONFIG_HH
#  define CCB_DATABASE_CONFIG_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declaration.
namespace             config {
  class               endpoint;
}

/**
 *  @class database_config database_config.hh "com/centreon/broker/database_config.hh"
 *  @brief Database configuration.
 *
 *  Hold the database information.
 */
class                 database_config {
public:
                      database_config();
                      database_config(
                        std::string const& type,
                        std::string const& host,
                        unsigned short port,
                        std::string const& user,
                        std::string const& password,
                        std::string const& name,
                        int queries_per_transaction = 1,
                        bool check_replication = true,
                        int connections_count = 1);
                      database_config(config::endpoint const& cfg);
                      database_config(database_config const& other);
                      ~database_config();
  database_config&    operator=(database_config const& other);

  std::string const&  get_type() const;
  std::string const&  get_host() const;
  unsigned short      get_port() const;
  std::string const&  get_user() const;
  std::string const&  get_password() const;
  std::string const&  get_name() const;
  int                 get_queries_per_transaction() const;
  bool                get_check_replication() const;
  int                 get_connections_count() const;

  void                set_type(std::string const& type);
  void                set_host(std::string const& host);
  void                set_port(unsigned short port);
  void                set_user(std::string const& user);
  void                set_password(std::string const& password);
  void                set_name(std::string const& name);
  void                set_connections_count(int count);
  void                set_queries_per_transaction(int qpt);
  void                set_check_replication(bool check_replication);

private:
  void                _internal_copy(database_config const& other);

  std::string         _type;
  std::string         _host;
  unsigned short      _port;
  std::string         _user;
  std::string         _password;
  std::string         _name;
  int                 _queries_per_transaction;
  bool                _check_replication;
  int                 _connections_count;
};

CCB_END()

#endif // !CCB_DATABASE_CONFIG_HH
