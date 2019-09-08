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

#include "com/centreon/broker/ceof/ceof_deserializer.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker::ceof;

/**
 *  Constructor.
 *
 *  @param[in] it  The iterator to use.
 */
ceof_deserializer::ceof_deserializer(ceof_iterator it)
    : ceof_visitor(false, true) {
  for (; !it.end(); ++it) {
    std::string key = it.get_value();
    ++it;
    if (!it.end()) {
      std::string val = it.get_value();
      _values[key] = val;
    }
  }
}

/**
 *  Destructor.
 */
ceof_deserializer::~ceof_deserializer() throw() {}

/**
 *  Get the value of this property.
 *
 *  @param[in] name  The name.
 *
 *  @return the value of this property.
 */
std::string ceof_deserializer::deserialize(std::string const& name) {
  std::map<std::string, std::string>::const_iterator found = _values.find(name);

  return (found == _values.end() ? std::string() : found->second);
}
