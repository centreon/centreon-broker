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

#ifndef CCB_BAM_META_SERVICE_STATUS_HH
#define CCB_BAM_META_SERVICE_STATUS_HH

#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class meta_service_status meta_service_status.hh
 * "com/centreon/broker/bam/meta_service_status.hh"
 *  @brief Update status of a meta-service.
 *
 *  Update the status of a meta-service. This will update the
 *  meta_service table.
 */
class meta_service_status : public io::data {
 public:
  meta_service_status();
  meta_service_status(meta_service_status const& other);
  ~meta_service_status();
  meta_service_status& operator=(meta_service_status const& other);
  unsigned int type() const;
  static unsigned int static_type();

  unsigned int meta_service_id;
  bool state_changed;
  double value;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(meta_service_status const& other);
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_META_SERVICE_STATUS_HH
