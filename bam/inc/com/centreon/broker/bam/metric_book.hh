/*
** Copyright 2014 Merethis
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

#ifndef CCB_BAM_METRIC_BOOK_HH
#  define CCB_BAM_METRIC_BOOK_HH

#  include <map>
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declaration.
namespace         storage {
  class           metric;
}

namespace         bam {
  // Forward declarations.
  class           metric_listener;

  /**
   *  @class metric_book metric_book.hh "com/centreon/broker/bam/metric_book.hh"
   *  @brief Propagate metric updates.
   *
   *  Propagate updates of metrics to metric listeners.
   */
  class           metric_book {
  public:
                  metric_book();
                  metric_book(metric_book const& other);
                  ~metric_book();
    metric_book&  operator=(metric_book const& other);
    void          listen(
                    unsigned int metric_id,
                    metric_listener* listnr);
    void          unlisten(
                    unsigned int metric_id,
                    metric_listener* listnr);
    void          update(
                    misc::shared_ptr<storage::metric> const& m,
                    io::stream* visitor = NULL);

  private:
    typedef std::multimap<unsigned int, metric_listener*> multimap;

    multimap      _book;
  };
}

CCB_END()

#endif // !CCB_BAM_METRIC_BOOK_HH
