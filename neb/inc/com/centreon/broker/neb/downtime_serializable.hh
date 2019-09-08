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

#ifndef CCB_NEB_DOWNTIME_SERIALIZABLE_HH
#define CCB_NEB_DOWNTIME_SERIALIZABLE_HH

#include <istream>
#include <memory>
#include <ostream>
#include <string>
#include "com/centreon/broker/ceof/ceof_serializable.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/neb/downtime.hh"

CCB_BEGIN()

namespace neb {
/**
 *  @class downtime_serializable downtime_serializable.hh
 * "com/centreon/broker/neb/downtime_serializable.hh"
 *  @brief Represent a serializable Centreon Engine Object File downtime.
 */
class downtime_serializable : public ceof::ceof_serializable {
 public:
  downtime_serializable();
  downtime_serializable(downtime_serializable const& other);
  downtime_serializable& operator=(downtime_serializable const& other);
  virtual ~downtime_serializable();

  template <typename U, U(downtime::*member)>
  std::string get_downtime_member() const;

  template <typename U, U(downtime::*member)>
  void set_downtime_member(std::string const& val);

  std::shared_ptr<downtime> get_downtime() const;

  virtual void visit(ceof::ceof_visitor& visitor);

 private:
  std::shared_ptr<downtime> _downtime;
};
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_TIMEPERIOD_SERIALIZABLE_HH
