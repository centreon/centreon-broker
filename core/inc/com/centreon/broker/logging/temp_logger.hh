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

#ifndef CCB_LOGGING_TEMP_LOGGER_HH_
#define CCB_LOGGING_TEMP_LOGGER_HH_

#include <string>
#include "com/centreon/broker/logging/defines.hh"
#include "com/centreon/broker/misc/stringifier.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace logging {
/**
 *  @class temp_logger temp_logger.hh
 * "com/centreon/broker/logging/temp_logger.hh"
 *  @brief Log messages.
 *
 *  This class is used to temporarily buffer log messages before
 *  writing them to backends.
 */
class temp_logger : private misc::stringifier {
 private:
  struct redir {
    temp_logger& (temp_logger::*redirect_bool)(bool)throw();
    temp_logger& (temp_logger::*redirect_double)(double)throw();
    temp_logger& (temp_logger::*redirect_int)(int)throw();
    temp_logger& (temp_logger::*redirect_long)(long)throw();
    temp_logger& (temp_logger::*redirect_long_long)(long long)throw();
    temp_logger& (temp_logger::*redirect_std_string)(std::string const&)throw();
    temp_logger& (temp_logger::*redirect_unsigned_int)(unsigned int)throw();
    temp_logger& (temp_logger::*redirect_unsigned_long)(unsigned long)throw();
    temp_logger& (temp_logger::*redirect_unsigned_long_long)(
        unsigned long long)throw();
    temp_logger& (temp_logger::*redirect_string)(char const*)throw();
    temp_logger& (temp_logger::*redirect_pointer)(void const*)throw();
  };
  level _level;
  mutable redir const* _redir;
  static redir const _redir_nothing;
  static redir const _redir_stringifier;
  type _type;
  void _internal_copy(temp_logger const& t);
  template <typename T>
  temp_logger& _nothing(T t) throw();
  template <typename T>
  temp_logger& _to_stringifier(T t) throw();

 public:
  temp_logger(type log_type, level l, bool enable = true);
  temp_logger(temp_logger const& t);
  ~temp_logger();
  temp_logger& operator=(temp_logger const& t);
  temp_logger& operator<<(bool b) throw();
  temp_logger& operator<<(double d) throw();
  temp_logger& operator<<(int i) throw();
  temp_logger& operator<<(long l) throw();
  temp_logger& operator<<(long long ll) throw();
  temp_logger& operator<<(std::string const& q) throw();
  temp_logger& operator<<(unsigned int u) throw();
  temp_logger& operator<<(unsigned long ul) throw();
  temp_logger& operator<<(unsigned long long ull) throw();
  temp_logger& operator<<(char const* str) throw();
  temp_logger& operator<<(void const* ptr) throw();
};
}  // namespace logging

CCB_END()

#endif /* !CCB_LOGGING_TEMP_LOGGER_HH_ */
