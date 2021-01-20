/*
** Copyright 2011 Centreon
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

#ifndef CCB_RRD_EXCEPTIONS_OPEN_HH_
#define CCB_RRD_EXCEPTIONS_OPEN_HH_

#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;

namespace com {
namespace centreon {
namespace broker {
namespace rrd {
namespace exceptions {
/**
 *  @class open open.hh "com/centreon/broker/rrd/exceptions/open.hh"
 *  @brief RRD file open error.
 *
 *  Exception thrown when unable to open an RRD file.
 */
class open : public msg_fmt {
 public:
  template <typename... Args>
  explicit open(std::string const& str, const Args&... args)
      : msg_fmt(str, args...) {}

  open() = delete;
  ~open() noexcept = default;
  open& operator=(const open&) = delete;
};
}  // namespace exceptions
}  // namespace rrd
}  // namespace broker
}  // namespace centreon
}  // namespace com

#endif /* !CCB_RRD_EXCEPTIONS_OPEN_HH_ */
