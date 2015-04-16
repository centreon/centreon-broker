/*
** Copyright 2014-2015 Merethis
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

#ifndef CCB_BAM_SERVICE_BOOK_HH
#  define CCB_BAM_SERVICE_BOOK_HH

#  include <map>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declarations.
namespace         neb {
  class           acknowledgement;
  class           downtime;
  class           service_status;
}

namespace         bam {
  // Forward declarations.
  class           service_listener;

  /**
   *  @class service_book service_book.hh "com/centreon/broker/bam/service_book.hh"
   *  @brief Propagate service updates.
   *
   *  Propagate updates of services to service listeners.
   */
  class           service_book {
  public:
                  service_book();
                  service_book(service_book const& other);
                  ~service_book();
    service_book& operator=(service_book const& other);
    void          listen(
                    unsigned int host_id,
                    unsigned int service_id,
                    service_listener* listnr);
    void          unlisten(
                    unsigned int host_id,
                    unsigned int service_id,
                    service_listener* listnr);
    void          update(
                    misc::shared_ptr<neb::service_status> const& ss,
                    io::stream* visitor = NULL);
    void          update(
                    misc::shared_ptr<neb::acknowledgement> const& ack,
                    io::stream* visitor = NULL);
    void          update(
                    misc::shared_ptr<neb::downtime> const& dt,
                    io::stream* visitor = NULL);

  private:
    typedef std::multimap<std::pair<unsigned int, unsigned int>, service_listener*> multimap;

    multimap      _book;
  };
}

CCB_END()

#endif // !CCB_BAM_SERVICE_BOOK_HH
