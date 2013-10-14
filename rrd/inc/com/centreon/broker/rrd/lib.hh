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

#ifndef CCB_RRD_LIB_HH
#  define CCB_RRD_LIB_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/rrd/backend.hh"
#  include "com/centreon/broker/rrd/creator.hh"

CCB_BEGIN()

namespace            rrd {
  /**
   *  @class lib lib.hh "com/centreon/broker/rrd/lib.hh"
   *  @brief Handle RRD file access through librrd.
   *
   *  Handle creation, deletion, tuning and update of an RRD file with
   *  librrd.
   */
  class              lib : public backend {
  public:
                     lib(
                       std::string const& tmpl_path,
                       unsigned int cache_size);
                     lib(lib const& l);
                     ~lib();
    lib&             operator=(lib const& l);
    void             begin();
    void             clean();
    void             close();
    void             commit();
    void             open(std::string const& filename);
    void             open(
                       std::string const& filename,
                       unsigned int length,
                       time_t from,
                       unsigned int step,
                       short value_type = 0);
    void             remove(std::string const& filename);
    void             update(
                       time_t t,
                       std::string const& value);

   private:
    creator          _creator;
    std::string      _filename;
  };
}

CCB_END()

#endif // !CCB_RRD_LIB_HH
