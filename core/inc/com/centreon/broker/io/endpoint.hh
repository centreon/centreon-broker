/*
** Copyright 2011-2012,2015 Merethis
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

#ifndef CCB_IO_ENDPOINT_HH
#  define CCB_IO_ENDPOINT_HH

#  include <QString>
#  include <string>
#  include <set>
#  include "com/centreon/broker/io/properties.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declaration.
class                                persistent_cache;

namespace                            io {
  /**
   *  @class endpoint endpoint.hh "com/centreon/broker/io/endpoint.hh"
   *  @brief Base class of connectors and acceptors.
   *
   *  Endpoint are used to open data streams. Endpoints can be either
   *  acceptors (which wait for incoming connections) or connectors
   *  (that initiate connections).
   */
  class                              endpoint {
   public:
                                     endpoint(bool is_accptr);
                                     endpoint(endpoint const& other);
    virtual                          ~endpoint();
    endpoint&                        operator=(endpoint const& other);
    void                             from(
                                       misc::shared_ptr<endpoint> endp);
    bool                             is_acceptor() const throw ();
    bool                             is_connector() const throw ();
    virtual misc::shared_ptr<stream> open() = 0;
    virtual void                     stats(io::properties& tree);
    void                             set_filter(
                                       std::set<unsigned int> const& filter);

   protected:
    void                             _internal_copy(
                                       endpoint const& other);

    misc::shared_ptr<endpoint>       _from;
    bool                             _is_acceptor;
    std::set<unsigned int>           _filter;
  };
}

CCB_END()

#endif // !CCB_IO_ENDPOINT_HH
