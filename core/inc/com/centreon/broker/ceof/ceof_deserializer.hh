/*
** Copyright 2009-2013 Centreon
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

#ifndef CCB_CEOF_CEOF_DESERIALIZER_HH
#define CCB_CEOF_CEOF_DESERIALIZER_HH

#include <map>
#include <string>
#include "com/centreon/broker/ceof/ceof_iterator.hh"
#include "com/centreon/broker/ceof/ceof_visitor.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace ceof {
/**
 *  @class ceof_deserializer ceof_deserializer.hh
 * "com/centreon/broker/ceof/ceof_deserializer.hh"
 *  @brief Represent a deserializer for Centreon Engine Object File class.
 */
class ceof_deserializer : public ceof_visitor {
 public:
  ceof_deserializer(ceof_iterator iterator);
  virtual ~ceof_deserializer() throw();

  virtual std::string deserialize(std::string const& name);

 private:
  std::map<std::string, std::string> _values;
};
}  // namespace ceof

CCB_END()

#endif  // !CCB_CEOF_CEOF_DESERIALIZER_HH
