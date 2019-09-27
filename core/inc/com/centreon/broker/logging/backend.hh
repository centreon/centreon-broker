/*
** Copyright 2009-2011 Centreon
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

#ifndef CCB_LOGGING_BACKEND_HH_
#define CCB_LOGGING_BACKEND_HH_

#include <mutex>
#include "com/centreon/broker/logging/defines.hh"

namespace com {
namespace centreon {
namespace broker {
namespace logging {
/**
 *  @class backend backend.hh "com/centreon/broker/logging/backend.hh"
 *  @brief Interface for log backends.
 *
 *  In Broker, multiple backends are available for log writing.
 *  This can either be plain-text files, syslog or standard
 *  outputs. This class defines an interface to communicate with
 *  any of these backends.
 *
 *  @see file
 *  @see syslog
 */
class backend : public std::mutex {
 public:
  backend();
  backend(backend const& b) = delete;
  virtual ~backend() = default;
  backend& operator=(backend const& b) = delete;
  virtual void log_msg(char const* msg,
                       unsigned int len,
                       type log_type,
                       level l) throw() = 0;
};
}  // namespace logging
}  // namespace broker
}  // namespace centreon
}  // namespace com

#endif /* !CCB_LOGGING_BACKEND_HH_ */
