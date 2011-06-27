/*
** Copyright 2011 Merethis
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

#ifndef CCB_SQL_CONNECTOR_HH_
# define CCB_SQL_CONNECTOR_HH_

# include <QString>
# include "com/centreon/broker/io/endpoint.hh"

namespace                            com {
  namespace                          centreon {
    namespace                        broker {
      namespace                      sql {
        /**
         *  @class connector connector.hh "com/centreon/broker/sql/connector.hh"
         *  @brief Connect to a database.
         *
         *  Send events to a SQL database.
         */
        class                        connector : public io::endpoint {
         private:
          QString                    _db;
          QString                    _host;
          QString                    _password;
          unsigned short             _port;
          QString                    _type;
          QString                    _user;

         public:
                                     connector();
                                     connector(connector const& c);
                                     ~connector();
          connector&                 operator=(connector const& c);
          void                       close();
          void                       connect_to(QString const& type,
                                       QString const& host,
                                       unsigned short port,
                                       QString const& user,
                                       QString const& password,
                                       QString const& db);
          QSharedPointer<io::stream> open();
        };
      }
    }
  }
}

#endif /* !CCB_SQL_CONNECTOR_HH_ */
