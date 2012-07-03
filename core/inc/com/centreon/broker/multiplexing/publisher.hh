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

#ifndef CCB_MULTIPLEXING_PUBLISHER_HH_
# define CCB_MULTIPLEXING_PUBLISHER_HH_

# include "com/centreon/broker/io/stream.hh"
# include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace      multiplexing {
  /**
   *  @class publisher publisher.hh "com/centreon/broker/multiplexing/publisher.hh"
   *  @brief Publish events to the multiplexing engine.
   *
   *  Publish events written to the publisher to the multiplexing
   *  engine.
   *
   *  @see engine
   */
  class        publisher : public io::stream {
  public:
               publisher();
               publisher(publisher const& p);
               ~publisher() throw ();
    publisher& operator=(publisher const& p);
    void       process(bool in = false, bool out = true);
    void       read(misc::shared_ptr<io::data>& d);
    void       write(misc::shared_ptr<io::data> const& d);

  private:
    bool       _process;
  };
}

CCB_END()

#endif /* !CCB_MULTIPLEXING_PUBLISHER_HH_ */
