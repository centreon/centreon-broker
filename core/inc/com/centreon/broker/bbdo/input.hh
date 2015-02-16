/*
** Copyright 2013,2015 Merethis
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

#ifndef CCB_BBDO_INPUT_HH
#  define CCB_BBDO_INPUT_HH

#  include <ctime>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                bbdo {
  /**
   *  @class input input.hh "com/centreon/broker/bbdo/input.hh"
   *  @brief BBDO input source.
   *
   *  The class converts an input stream into events using the BBDO
   *  (Broker Binary Data Objects) protocol.
   */
  class                  input : virtual public io::stream {
  public:
                         input();
                         input(input const& other);
    virtual              ~input();
    input&               operator=(input const& other);
    void                 process(bool in = false, bool out = false);
    virtual void         read(misc::shared_ptr<io::data>& d);
    virtual unsigned int read_any(
                           misc::shared_ptr<io::data>& d,
                           time_t timeout = (time_t)-1);
    virtual unsigned int write(misc::shared_ptr<io::data> const& d);

  private:
    void                 _buffer_must_have_unprocessed(
                           unsigned int bytes,
                           time_t timeout = (time_t)-1);
    void                 _internal_copy(input const& right);

    std::string          _buffer;
    bool                 _process_in;
    unsigned int         _processed;
  };
}

CCB_END()

#endif // !CCB_BBDO_INPUT_HH
