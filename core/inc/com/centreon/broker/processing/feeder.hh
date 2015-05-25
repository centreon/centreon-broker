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

#ifndef CCB_PROCESSING_FEEDER_HH
#  define CCB_PROCESSING_FEEDER_HH

#  include <string>
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/processing/thread.hh"

CCB_BEGIN()

// Forward declaration.
namespace         io {
  class           stream;
}

namespace         processing {
  /**
   *  @class feeder feeder.hh "com/centreon/broker/processing/feeder.hh"
   *  @brief Feed events from a source to a destination.
   *
   *  Take events from a source and send them to a destination.
   */
  class           feeder : public thread {
  public:
                  feeder();
                  ~feeder();
    void          prepare(
                    std::string const& name,
                    misc::shared_ptr<io::stream> in,
                    misc::shared_ptr<io::stream> out);
    void          run();

  private:
                  feeder(feeder const& other);
    feeder&       operator=(feeder const& other);

    misc::shared_ptr<io::stream>
                  _in;
    std::string   _name;
    misc::shared_ptr<io::stream>
                  _out;
  };
}

CCB_END()

#endif // !CCB_PROCESSING_FEEDER_HH
