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

#ifndef CCB_IO_STREAM_HH
#  define CCB_IO_STREAM_HH

#  include <ctime>
#  include <string>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/properties.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                    io {
  /**
   *  @class stream stream.hh "com/centreon/broker/io/stream.hh"
   *  @brief Class used to exchange data.
   *
   *  Interface to exchange data.
   *
   *  The read() method is used to get data while waiting for it. It
   *  should work as follow :
   *    - has data :        shared pointer is properly set, true returned
   *    - spurious wake :   null shared pointer, true returned
   *    - timeout :         null shared pointer, false returned
   *    - known shutdown :  throw io::exceptions::shutdown
   *    - error :           any exception
   */
  class                      stream {
  public:
                             stream();
                             stream(stream const& other);
    virtual                  ~stream();
    stream&                  operator=(stream const& other);
    virtual bool             read(
                               misc::shared_ptr<io::data>& d,
                               time_t deadline = (time_t)-1) = 0;
    virtual void             set_substream(
                               misc::shared_ptr<stream> substream);
    virtual void             statistics(io::properties& tree) const;
    virtual void             update();
    virtual unsigned int     write(misc::shared_ptr<data> const& d) = 0;

  protected:
    misc::shared_ptr<stream> _substream;
  };
}

CCB_END()

#endif // !CCB_IO_STREAM_HH
