/*
** Copyright 2011-2012 Merethis
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

#ifndef CCB_SQL_CONNECTOR_HH
#  define CCB_SQL_CONNECTOR_HH

#  include <QString>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                        notification {
  /**
   *  @class connector connector.hh "com/centreon/broker/notification/connector.hh"
   *  @brief Connect to a database.
   *
   *  Send events to a notification cache database.
   */
  class                          connector : public io::endpoint {
  public:
                                 connector();
                                 connector(connector const& c);
                                 ~connector();
    connector&                   operator=(connector const& c);
    io::endpoint*                clone() const;
    void                         close();
    void                         connect_to(
                                   QString const& type,
                                   QString const& host,
                                   unsigned short port,
                                   QString const& user,
                                   QString const& password,
                                   QString const& db,
                                   unsigned int queries_per_transaction = 1,
                                   unsigned int cleanup_check_interval = 0,
                                   bool check_replication = true,
                                   bool with_state_events = false);
    misc::shared_ptr<io::stream> open();
    misc::shared_ptr<io::stream> open(QString const& id);

  private:
    bool                         _check_replication;
    unsigned int                 _cleanup_check_interval;
    QString                      _db;
    QString                      _host;
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
