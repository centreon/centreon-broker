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

#ifndef CCB_MULTIPLEXING_SUBSCRIBER_HH
#  define CCB_MULTIPLEXING_SUBSCRIBER_HH

#  include <memory>
#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                multiplexing {
  // Forward declaration.
  class                  muxer;

  /**
   *  @class subscriber subscriber.hh "com/centreon/broker/multiplexing/subscriber.hh"
   *  @brief Handle registration of a muxer.
   *
   *  Handle registration of a muxer against the multiplexing engine.
   */
  class                  subscriber {
  public:
                         subscriber(
                           std::string const& name,
                           bool persistent = false);
                         ~subscriber();
    muxer&               get_muxer() const;

  private:
                         subscriber(subscriber const& other);
    subscriber&          operator=(subscriber const& other);

    std::auto_ptr<muxer> _muxer;
  };
}

CCB_END()

#endif // !CCB_MULTIPLEXING_SUBSCRIBER_HH
