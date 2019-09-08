/*
** Copyright 2013 Centreon
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

#ifndef CCB_CORRELATION_INTERNAL_HH
#define CCB_CORRELATION_INTERNAL_HH

#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace correlation {
// Data elements.
enum data_element {
  de_engine_state = 1,
  de_issue,
  de_issue_parent,
  de_state,
  de_log_issue
};
}  // namespace correlation

CCB_END()

#endif  // !CCB_CORRELATION_INTERNAL_HH
