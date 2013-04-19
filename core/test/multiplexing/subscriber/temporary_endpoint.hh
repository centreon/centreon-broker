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

#ifndef CCB_TEMPORARY_ENDPOINT_HH
#  define CCB_TEMPORARY_ENDPOINT_HH

#  include <QList>
#  include "com/centreon/broker/io/endpoint.hh"

CCB_BEGIN()

/**
 *  @class temporary_endpoint temporary_endpoint.hh
 *  @brief Temporary endpoint.
 *
 *  Endpoint that can be set to generate errors or not.
 */
class                 temporary_endpoint : public io::endpoint {
public:
                      temporary_endpoint(QString const& id = "");
                      temporary_endpoint(temporary_endpoint const& se);
                      ~temporary_endpoint();
  temporary_endpoint& operator=(temporary_endpoint const& se);
  io::endpoint*       clone() const;
  void                close();
  misc::shared_ptr<io::stream>
                      open();
  misc::shared_ptr<io::stream>
                      open(QString const& id);

private:
  QString             _id;
};

CCB_END()

#endif // !CCB_TEMPORARY_ENDPOINT_HH
