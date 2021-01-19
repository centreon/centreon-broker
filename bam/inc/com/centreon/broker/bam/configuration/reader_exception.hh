/*
** Copyright 2014 Centreon
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

#ifndef CCB_CONFIGURATION_READER_EXCEPTION_HH
#define CCB_CONFIGURATION_READER_EXCEPTION_HH

#include "com/centreon/exceptions/msg_fmt.hh"

CCB_BEGIN()

namespace bam {
namespace configuration {
/**
 *  @class reader_exception reader_exception.hh
 * "com/centreon/broker/bam/configuration/reader_exception.hh"
 *  @brief Exception thrown when the reader fails to read from a database
 *
 *  Reader_exception.
 */
class reader_exception : public com::centreon::exceptions::msg_fmt {
 public:
  reader_exception() = delete;

  template <typename... Args>
  explicit reader_exception(std::string const& str, const Args&... args)
    : msg_fmt(str, args...) {}
  reader_exception(const reader_exception&);
  ~reader_exception() noexcept {};
};
}  // namespace configuration
}  // namespace bam

CCB_END()

#endif  // !CCB_CONFIGURATION_READER_EXCEPTION_HH
