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

#include "com/centreon/broker/bam/reporting_stream.hh"

#include <gtest/gtest.h>

#include <memory>

#include "com/centreon/broker/bam/dimension_kpi_event.hh"
#include "com/centreon/broker/bam/dimension_truncate_table_signal.hh"
#include "com/centreon/broker/persistent_cache.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

class BamReportingStream : public testing::Test {
 public:
};

TEST_F(BamReportingStream, WriteKpi) {
  database_config cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                      "centreon");
  database_config storage("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                          "centreon_storage");

  std::shared_ptr<persistent_cache> cache;
  std::unique_ptr<reporting_stream> rs;

  ASSERT_NO_THROW(rs.reset(new reporting_stream(storage)));

  std::shared_ptr<dimension_kpi_event> st{
      std::make_shared<dimension_kpi_event>(dimension_kpi_event())};
  rs->write(std::static_pointer_cast<io::data>(st));

  std::shared_ptr<dimension_truncate_table_signal> flush{
      std::make_shared<dimension_truncate_table_signal>(
          dimension_truncate_table_signal())};
  flush->update_started = false;
  rs->write(std::static_pointer_cast<io::data>(flush));
}
