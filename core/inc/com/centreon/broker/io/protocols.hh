/*
** Copyright 2011-2012 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_IO_PROTOCOLS_HH
#define CCB_IO_PROTOCOLS_HH

#include <map>
#include <string>
#include "com/centreon/broker/io/factory.hh"

CCB_BEGIN()

namespace io {
/**
 *  @class protocols protocols.hh "com/centreon/broker/io/protocols.hh"
 *  @brief Reference available protocols.
 *
 *  This class registers every available protocol that are used
 *  to build input or output objects.
 */
class protocols {
 public:
  struct protocol {
    std::shared_ptr<factory> endpntfactry;
    unsigned short osi_from;
    unsigned short osi_to;
  };

  ~protocols();
  std::map<std::string, protocol>::const_iterator begin() const;
  std::map<std::string, protocol>::const_iterator end() const;
  static protocols& instance();
  static void load();
  void reg(std::string const& name,
           factory const& fac,
           unsigned short osi_from,
           unsigned short osi_to);
  static void unload();
  void unreg(std::string const& name);

 private:
  protocols();
  protocols(protocols const& p);
  protocols& operator=(protocols const& p);

  std::map<std::string, protocol> _protocols;
};
}  // namespace io

CCB_END()

#endif  // !CCB_IO_PROTOCOLS_HH
