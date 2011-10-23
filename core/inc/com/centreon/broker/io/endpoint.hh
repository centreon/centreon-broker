/*
** Copyright 2011 Merethis
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

#ifndef CCB_IO_ENDPOINT_HH_
# define CCB_IO_ENDPOINT_HH_

# include <QSharedPointer>
# include "com/centreon/broker/io/stream.hh"
# include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                          io {
  /**
   *  @class endpoint endpoint.hh "com/centreon/broker/io/endpoint.hh"
   *  @brief Base class of connectors and acceptors.
   *
   *  Endpoint are used to open data streams. Endpoints can be either
   *  acceptors (which wait for incoming connections) or connectors
   *  (that initiate connections).
   */
  class                            endpoint {
   protected:
    QSharedPointer<endpoint>       _from;
    bool                           _is_acceptor;
    void                           _internal_copy(endpoint const& e);

   public:
                                   endpoint(bool is_accptr);
                                   endpoint(endpoint const& e);
    virtual                        ~endpoint();
    endpoint&                      operator=(endpoint const& e);
    virtual void                   close() = 0;
    void                           from(QSharedPointer<endpoint> endp);
    bool                           is_acceptor() const throw ();
    bool                           is_connector() const throw ();
    virtual QSharedPointer<stream> open() = 0;
  };
}

CCB_END()

#endif /* !CCB_IO_ENDPOINT_HH_ */
