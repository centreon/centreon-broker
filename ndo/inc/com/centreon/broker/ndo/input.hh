/*
** Copyright 2009-2011 Merethis
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

#ifndef CCB_NDO_INPUT_HH_
# define CCB_NDO_INPUT_HH_

# include <sstream>
# include "com/centreon/broker/io/stream.hh"
# include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        ndo {
  /**
   *  @class input input.hh "com/centreon/broker/ndo/input.hh"
   *  @brief NDO input source.
   *
   *  The class converts an input stream into events using a modified
   *  version of the NDO protocol.
   */
  class          input : virtual public io::stream {
   private:
    std::string  _buffer;
    bool         _process_in;
    std::string  _old;
    char const*  _get_line();
    template     <typename T>
    T*           _handle_event();

   public:
                 input();
                 input(input const& i);
    virtual      ~input();
    input&       operator=(input const& i);
    void         process(bool in = false, bool out = false);
    virtual QSharedPointer<io::data>
                 read();
    virtual void write(QSharedPointer<io::data> d);
  };
}

CCB_END()

#endif /* !CCB_NDO_INPUT_HH_ */
