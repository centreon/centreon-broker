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

#ifndef CCB_ENGCMD_ENDPOINT_HH
#  define CCB_ENGCMD_ENDPOINT_HH

#  include <string>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace         neb {
namespace         engcmd {
  /**
   *  @class endpoint endpoint.hh "com/centreon/broker/engcmd/endpoint.hh"
   *  @brief Engine command endpoint.
   *
   *  Build engine command streams.
   */
  class           endpoint : public io::endpoint {
  public:
                  endpoint(
                    std::string const& name,
                    std::string const& command_module_path);
                  endpoint(endpoint const& other);
                  ~endpoint();
    endpoint&     operator=(endpoint const& other);
    misc::shared_ptr<io::stream>
                  open();
  private:
    std::string   _name;
    std::string  _command_module_path;
  };
}
}

CCB_END()

#endif // !CCB_ENGCMD_FACTORY_HH
