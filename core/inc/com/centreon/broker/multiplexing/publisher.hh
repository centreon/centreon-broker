/*
** Copyright 2011-2013,2015 Merethis
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

#ifndef CCB_MULTIPLEXING_PUBLISHER_HH
#  define CCB_MULTIPLEXING_PUBLISHER_HH

#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        multiplexing {
  /**
   *  @class publisher publisher.hh "com/centreon/broker/multiplexing/publisher.hh"
   *  @brief Publish events to the multiplexing engine.
   *
   *  Publish events written to the publisher to the multiplexing
   *  engine.
   *
   *  @see engine
   */
  class          publisher : public io::stream {
  public:
                 publisher();
                 publisher(publisher const& other);
                 ~publisher() throw ();
    publisher&   operator=(publisher const& other);
    bool         read(
                   misc::shared_ptr<io::data>& d,
                   time_t deadline = (time_t)-1);
    unsigned int write(misc::shared_ptr<io::data> const& d);
  };
}

CCB_END()

#endif // !CCB_MULTIPLEXING_PUBLISHER_HH
