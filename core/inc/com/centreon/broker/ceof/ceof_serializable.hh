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

#ifndef CCB_CEOF_CEOF_SERIALIZABLE_HH
#define CCB_CEOF_CEOF_SERIALIZABLE_HH

#include <map>
#include <string>
#include "com/centreon/broker/ceof/ceof_iterator.hh"
#include "com/centreon/broker/ceof/ceof_visitor.hh"
#include "com/centreon/broker/ceof/ceof_writer.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace ceof {
/**
 *  @class ceof_serializable ceof_serializable.hh
 * "com/centreon/broker/ceof/ceof_serializable.hh"
 *  @brief Represent a serializable Centreon Engine Object File class.
 */
class ceof_serializable {
 public:
  virtual ~ceof_serializable() {}

  virtual void visit(ceof_visitor& visitor) = 0;
};
}  // namespace ceof

CCB_END()

#endif  // !CCB_CEOF_CEOF_SERIALIZABLE_HH
