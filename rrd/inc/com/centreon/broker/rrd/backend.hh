/*
** Copyright 2011-2013 Merethis
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

#ifndef CCB_RRD_BACKEND_HH
#  define CCB_RRD_BACKEND_HH

#  include <ctime>
#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        rrd {
  /**
   *  @class backend backend.hh "com/centreon/broker/rrd/backend.hh"
   *  @brief Generic access to RRD files.
   *
   *  Provide a unified access to RRD files. Files can be accessed
   *  either through librrd or with rrdcached.
   *
   *  @see rrd::lib
   *  @see rrd::cached
   */
  class          backend {
  public:
                 backend();
                 backend(backend const& b);
    virtual      ~backend();
    backend&     operator=(backend const& b);
    virtual void begin() = 0;
    virtual void clean() = 0;
    virtual void close() = 0;
    virtual void commit() = 0;
    virtual void open(std::string const& filename) = 0;
    virtual void open(
                   std::string const& filename,
                   unsigned int length,
                   time_t from,
                   unsigned int step,
                   short value_type = 0) = 0;
    virtual void remove(std::string const& filename) = 0;
    virtual void update(time_t t, std::string const& value) = 0;
  };
}

CCB_END()

#endif // !CCB_RRD_BACKEND_HH
