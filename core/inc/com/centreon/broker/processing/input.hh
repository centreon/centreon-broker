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

#ifndef CCB_PROCESSING_INPUT_HH
#  define CCB_PROCESSING_INPUT_HH

#  include <string>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/processing/thread.hh"

CCB_BEGIN()

namespace       processing {
  /**
   *  @class input input.hh "com/centreon/broker/processing/input.hh"
   *  @brief Process input endpoints.
   *
   *  This class process input endpoints that are not acceptors. It
   *  opens the endpoint, read as much data as possible from it and
   *  publish it into the multiplexing engine.
   */
  class         input : public thread {
  public:
                input(
                  misc::shared_ptr<io::endpoint> endp,
                  std::string const& name);
                ~input();
    void        exit();
    void        run();
    void        set_retry_interval(time_t retry_interval);

  private:
                input(input const& other);
    input&      operator=(input const& other);

    misc::shared_ptr<io::endpoint>
                _endp;
    std::string _name;
    time_t      _retry_interval;
  };
}

CCB_END()

#endif // !CCB_PROCESSING_INPUT_HH
