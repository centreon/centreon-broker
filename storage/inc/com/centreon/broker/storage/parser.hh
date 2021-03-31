/*
** Copyright 2011-2013, 2021 Centreon
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

#ifndef CCB_STORAGE_PARSER_HH
#define CCB_STORAGE_PARSER_HH

#include <list>
#include <string>

#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/storage/perfdata.hh"

CCB_BEGIN()

namespace storage {
/**
 *  @class parser parser.hh "com/centreon/broker/storage/parser.hh"
 *  @brief Parse raw perfdatas.
 *
 *  Parse perfdata as given by plugins output and convert them to a
 *  perfdata structure.
 */
class parser {
 public:
  parser() = default;
  ~parser() noexcept = default;
  parser(parser const& p) = delete;
  parser& operator=(parser const& p) = delete;
  void parse_perfdata(uint32_t host_id,
                      uint32_t service_id,
                      const char* str,
                      std::list<perfdata>& pd);
};
}  // namespace storage

CCB_END()

#endif  // !CCB_STORAGE_PARSER_HH
