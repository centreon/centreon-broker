/*
** Copyright 2015 Merethis
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

#ifndef CCB_PROCESSING_ACCEPTOR_HH
#  define CCB_PROCESSING_ACCEPTOR_HH

#  include <ctime>
#  include <string>
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/misc/unordered_hash.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/processing/thread.hh"

CCB_BEGIN()

// Forward declaration.
namespace       io {
  class         endpoint;
}

namespace       processing {
  // Forward declaration.
  class         feeder;

  /**
   *  @class acceptor acceptor.hh "com/centreon/broker/processing/acceptor.hh"
   *  @brief Accept incoming connections.
   *
   *  Accept incoming connections and launch a feeder thread.
   */
  class         acceptor : public thread {
  public:
                acceptor(
                  misc::shared_ptr<io::endpoint> endp,
                  std::string const& name);
                ~acceptor();
    void        accept();
    void        exit();
    void        run();
    void        set_read_filters(uset<unsigned int> const& filters);
    void        set_retry_interval(time_t retry_interval);
    void        set_write_filters(uset<unsigned int> const& filters);

  private:
                acceptor(acceptor const& other);
    acceptor&   operator=(acceptor const& other);
    void        _wait_feeders();

    misc::shared_ptr<io::endpoint>
                _endp;
    std::list<misc::shared_ptr<processing::feeder> >
                _feeders;
    std::string _name;
    uset<unsigned int>
                _read_filters;
    time_t      _retry_interval;
    uset<unsigned int>
                _write_filters;
  };
}

CCB_END()

#endif // !CCB_PROCESSING_ACCEPTOR_HH
