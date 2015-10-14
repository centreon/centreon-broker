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

// Dummy com::centreon::engine::get_host_id and
// com::centreon::engine::get_service_id.
// Used for binary linkage of the set_log_data unit test.
namespace      com {
  namespace    centreon {
    namespace  engine {
      unsigned int get_host_id(char const*) {
        return (0);
      }
      unsigned int get_service_id(char const*, char const*) {
        return (0);
      }
    }
  }
}
