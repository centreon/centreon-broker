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

#include "com/centreon/broker/bam/monitoring_stream.hh"

#include <gtest/gtest.h>

#include <memory>

#include "com/centreon/broker/bam/ba_status.hh"
#include "com/centreon/broker/bam/kpi_status.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/multiplexing/engine.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

class BamMonitoringStream : public testing::Test {
  void SetUp() override { config::applier::init(); }
  void TearDown() override { config::applier::deinit(); }
};

TEST_F(BamMonitoringStream, WriteKpi) {
  database_config cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                      "centreon");
  database_config storage("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                          "centreon_storage");

  std::shared_ptr<persistent_cache> cache;
  std::unique_ptr<monitoring_stream> ms;

  ASSERT_NO_THROW(ms.reset(new monitoring_stream("", cfg, storage, cache)));

  std::shared_ptr<kpi_status> st{std::make_shared<kpi_status>(kpi_status())};

  ms->write(std::static_pointer_cast<io::data>(st));
}

TEST_F(BamMonitoringStream, WriteBA) {
  database_config cfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                      "centreon");
  database_config storage("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                          "centreon_storage");
  ;
  std::shared_ptr<persistent_cache> cache;
  std::unique_ptr<monitoring_stream> ms;

  ASSERT_NO_THROW(ms.reset(new monitoring_stream("", cfg, storage, cache)));

  std::shared_ptr<ba_status> st{std::make_shared<ba_status>(ba_status())};

  ms->write(std::static_pointer_cast<io::data>(st));
}
