/*
** Copyright 2020-2021 Centreon
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

#ifndef CCB_PROCESSING_ENDPOINT_HH
#define CCB_PROCESSING_ENDPOINT_HH

#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/processing/stat_visitable.hh"

CCB_BEGIN()

namespace processing {
class endpoint : public stat_visitable {
  const bool _is_feeder;

 public:
  endpoint(bool is_feeder, const std::string& name)
      : stat_visitable(name), _is_feeder(is_feeder) {}
  bool is_feeder() const { return _is_feeder; }
  virtual ~endpoint() noexcept {}
  virtual void update() {}
  virtual void start() = 0;
  virtual void exit() = 0;
};
}  // namespace processing

CCB_END()

#endif /* CCB_PROCESSING_ENDPOINT_HH */
