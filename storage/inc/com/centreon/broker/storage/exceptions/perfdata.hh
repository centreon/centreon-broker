/*
** Copyright 2011-2013 Centreon
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

#ifndef CCB_STORAGE_EXCEPTIONS_PERFDATA_HH
#define CCB_STORAGE_EXCEPTIONS_PERFDATA_HH

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace storage {
namespace exceptions {
/**
 *  @class perfdata perfdata.hh
 * "com/centreon/broker/storage/exceptions/perfdata.hh"
 *  @brief Perfdata exception.
 *
 *  Exception thrown when handling performance data.
 */
class perfdata : public broker::exceptions::msg {
 public:
  perfdata() throw();
  perfdata(perfdata const& pd) throw();
  ~perfdata() throw();
  perfdata& operator=(perfdata const& pdf) throw();
  virtual broker::exceptions::msg* clone() const;
  virtual void rethrow() const;

  /**
   *  Insert data in message.
   *
   *  @param[in] t Data to insert.
   */
  template <typename T>
  perfdata& operator<<(T t) throw() {
    broker::exceptions::msg::operator<<(t);
    return (*this);
  }
};
}  // namespace exceptions
}  // namespace storage

CCB_END()

#endif  // !CCB_STORAGE_EXCEPTIONS_PERFDATA_HH
