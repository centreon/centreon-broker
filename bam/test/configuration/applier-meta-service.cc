/*
 * Copyright 2021 Centreon
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

#include <gtest/gtest.h>
#include "com/centreon/broker/bam/configuration/applier/state.hh"
#include "com/centreon/broker/bam/configuration/kpi.hh"
#include "com/centreon/broker/config/applier/init.hh"

using namespace com::centreon::broker;

class ApplierMetaservice : public ::testing::Test {
 public:
  void SetUp() override {
    // Initialization.
    config::applier::init(2, "test_broker");

    _aply_state.reset(new bam::configuration::applier::state);
    _state.reset(new bam::configuration::state);
  }

  void TearDown() override {
    // Cleanup.
    config::applier::deinit();
  }

 protected:
  std::unique_ptr<bam::configuration::applier::state> _aply_state;
  std::unique_ptr<bam::configuration::state> _state;
};

// Adding a conf with one meta_service
// Complete the conf with another meta_service
// Remove the first meta and modify the second one.
TEST_F(ApplierMetaservice, Simple) {
  bam::configuration::meta_service meta_srv(12, 5, 18, "meta_srv", "SOM", 0.7, 0.9, "", "");
  bam::configuration::kpi kpi_meta_srv;
  kpi_meta_srv.set_id(17);
  kpi_meta_srv.set_boolexp_id(12);
  _state->mutable_meta_services().insert({meta_srv.get_id(), meta_srv});
  _state->get_kpis().insert({kpi_meta_srv.get_id(), kpi_meta_srv});

  ASSERT_NO_THROW(_aply_state->apply(*_state));

  meta_srv = bam::configuration::meta_service(15, 7, 20, "meta_srv1", "AVE", 0.7, 0.9, "", "");
  kpi_meta_srv.set_id(19);
  kpi_meta_srv.set_boolexp_id(15);
  _state->mutable_meta_services().insert({meta_srv.get_id(), meta_srv});
  _state->get_kpis().insert({kpi_meta_srv.get_id(), kpi_meta_srv});

  ASSERT_NO_THROW(_aply_state->apply(*_state));

  _state->mutable_meta_services().erase(12);
  auto f = _state->mutable_meta_services().find(15);
  f->second.set_name("meta_srv2");
  ASSERT_NO_THROW(_aply_state->apply(*_state));
}
