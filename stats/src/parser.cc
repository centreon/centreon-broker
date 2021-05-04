/*
 * Copyright 2011 - 2021 Centreon (https://www.centreon.com/)
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
#include <nlohmann/json.hpp>
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker::stats;

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
  auto json_fifo = [&cfg](nlohmann::json const& js) -> void {
    if (js.is_string() && !js.get<std::string>().empty())
      cfg.push_back(js.get<std::string>());
  };

  nlohmann::json js;
  try {
    js = nlohmann::json::parse(content);
  } catch (const nlohmann::json::parse_error& e) {
    throw msg_fmt("stats: invalid json file: {}", e.what());
  }
  if (js.is_object()) {
    nlohmann::json const& field{js["json_fifo"]};
    json_fifo(field);
  } else if (js.is_array()) {
    for (auto it = js.begin(), end = js.end(); it != end; ++it) {
      nlohmann::json const& field{(*it)["json_fifo"]};
      json_fifo(field);
    }
  }
}
