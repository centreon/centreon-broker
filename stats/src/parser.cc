/*
 * Copyright 2011 - 2019 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */

#include "com/centreon/broker/stats/parser.hh"
#include <json11.hpp>
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker::stats;
using namespace json11;

/**
 *  Default constructor.
 */
parser::parser() {}

/**
 *  Destructor.
 */
parser::~parser() noexcept {}

/**
 *  Parse a XML buffer.
 *
 *  @param[out] entries Parsed entries.
 *  @param[in]  content XML content.
 */
void parser::parse(std::vector<std::string>& cfg, std::string const& content) {
  std::string err;
  auto json_fifo = [&cfg](Json const& js) -> void {
    if (js.is_string() && !js.string_value().empty())
      cfg.push_back(js.string_value());
  };

  Json const& js{Json::parse(content, err)};
  if (!err.empty())
    throw msg_fmt("stats: invalid json file");

  if (js.is_object()) {
    Json const& field{js["json_fifo"]};
    json_fifo(field);
  } else if (js.is_array()) {
    for (auto it = js.array_items().begin(), end = js.array_items().end();
         it != end; ++it) {
      Json const& field{(*it)["json_fifo"]};
      json_fifo(field);
    }
  }
}
