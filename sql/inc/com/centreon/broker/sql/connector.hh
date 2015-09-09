/*
** Copyright 2011-2012 Centreon
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

#ifndef CCB_SQL_CONNECTOR_HH
#  define CCB_SQL_CONNECTOR_HH

#  include <QString>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                        sql {
  /**
   *  @class connector connector.hh "com/centreon/broker/sql/connector.hh"
   *  @brief Connect to a database.
   *
   *  Send events to a SQL database.
   */
  class                          connector : public io::endpoint {
  public:
                                 connector();
                                 connector(connector const& c);
                                 ~connector();
    connector&                   operator=(connector const& c);
    void                         connect_to(
                                   QString const& type,
                                   QString const& host,
                                   unsigned short port,
                                   QString const& user,
                                   QString const& password,
                                   QString const& db,
                                   unsigned int queries_per_transaction = 1,
                                   unsigned int cleanup_check_interval = 0,
                                   unsigned int instance_timeout = 15,
                                   bool check_replication = true,
                                   bool with_state_events = false);
    misc::shared_ptr<io::stream> open();

  private:
    bool                         _check_replication;
    unsigned int                 _cleanup_check_interval;
    QString                      _db;
    QString                      _host;
    unsigned int                 _instance_timeout;
    QString                      _password;
    unsigned short               _port;
    unsigned int                 _queries_per_transaction;
    QString                      _type;
    QString                      _user;
    bool                         _with_state_events;
  };
}

CCB_END()

#endif // !CCB_SQL_CONNECTOR_HH
