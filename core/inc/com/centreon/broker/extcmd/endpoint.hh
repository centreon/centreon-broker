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

#ifndef CCB_EXTCMD_ENDPOINT_HH
#  define CCB_EXTCMD_ENDPOINT_HH

#  include <string>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/persistent_cache.hh"

CCB_BEGIN()

namespace                        extcmd {
  /**
   *  @class endpoint endpoint.hh "com/centreon/broker/extcmd/endpoint.hh"
   *  @brief Open a command file stream.
   *
   *  Open a command file stream.
   */
  class                          endpoint : public io::endpoint {
  public:
                                 endpoint(std::string const& filename);
                                 endpoint(endpoint const& o);
                                 ~endpoint();
    endpoint&                    operator=(endpoint const& o);
    misc::shared_ptr<io::stream> open();

   private:
    std::string                  _filename;
  };
}

CCB_END()

#endif // !CCB_EXTCMD_ENDPOINT_HH
