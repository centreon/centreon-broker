/*
** Copyright 2018 Centreon
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
#include <cstdio>
#include <fstream>
#include "../../core/test/test_server.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/modules/loader.hh"
#include "com/centreon/broker/neb/custom_variable.hh"
#include "com/centreon/broker/neb/host.hh"
#include "com/centreon/broker/neb/instance.hh"
#include "com/centreon/broker/neb/module.hh"
#include "com/centreon/broker/neb/service.hh"
#include "com/centreon/broker/storage/conflict_manager.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::sql;

class ConflictManagerTest : public ::testing::Test {
 public:
  void SetUp() override {
    try {
      config::applier::init();
    } catch (std::exception const& e) {
      (void)e;
    }
  }
  void TearDown() override {
    config::applier::deinit();
  }
};

TEST_F(ConflictManagerTest, OpenClose) {
  database_config dbcfg("MySQL", "127.0.0.1", 3306, "root", "root",
                        "centreon_storage", 5, true, 5);
  uint32_t loop_timeout = 5;
  uint32_t instance_timeout = 5;

  ASSERT_FALSE(conflict_manager::init_storage(true, 100000, 18));
  ASSERT_NO_THROW(
      conflict_manager::init_sql(dbcfg, loop_timeout, instance_timeout));
  ASSERT_TRUE(conflict_manager::init_storage(true, 100000, 18));
  conflict_manager::close();
}

TEST_F(ConflictManagerTest, InstCVConflict) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  uint32_t loop_timeout = 5;
  uint32_t instance_timeout = 5;
  database_config dbcfg("MySQL", "127.0.0.1", 3306, "root", "root",
                         "centreon_storage", 5, true, 5);
  ASSERT_NO_THROW(
      conflict_manager::init_sql(dbcfg, loop_timeout, instance_timeout));

  std::shared_ptr<neb::instance> inst{std::make_shared<neb::instance>()};
  inst->poller_id = 1;
  inst->name = "Central";
  inst->program_start = time(nullptr) - 100;
  inst->program_end = time(nullptr) - 1;
  inst->version = "1.8.1";
  inst->is_running = true;
  conflict_manager::instance().send_event(conflict_manager::sql, inst);

  std::shared_ptr<neb::host> h{std::make_shared<neb::host>()};
  h->address = "10.0.2.15";
  h->alias = "central";
  h->flap_detection_on_down = true;
  h->flap_detection_on_unreachable = true;
  h->flap_detection_on_up = true;
  h->host_id = 31;
  h->host_name = "central_9";
  h->notify_on_down = true;
  h->notify_on_unreachable = true;
  h->poller_id = 1;
  h->stalk_on_down = false;
  h->stalk_on_unreachable = false;
  h->stalk_on_up = false;
  h->statusmap_image = "";
  h->timezone = "Europe/Paris";
  conflict_manager::instance().send_event(conflict_manager::sql, h);

  std::shared_ptr<neb::service> s{std::make_shared<neb::service>()};
  s->host_id = 31;
  s->service_id = 498;
  s->default_active_checks_enabled = true;
  s->default_event_handler_enabled = true;
  s->default_flap_detection_enabled = true;
  s->default_notifications_enabled = true;
  s->default_passive_checks_enabled = true;
  s->display_name = "test-dbr";
  s->icon_image = "";
  s->icon_image_alt = "";
  s->notification_interval = 30;
  s->notification_period = "";
  s->notify_on_downtime = true;
  s->notify_on_flapping = true;
  s->notify_on_recovery = true;
  s->retain_nonstatus_information = true;
  s->retain_status_information = true;

  conflict_manager::instance().send_event(conflict_manager::sql, s);

  std::shared_ptr<neb::custom_variable> cv{std::make_shared<neb::custom_variable>()};
  cv->service_id = 498;
  cv->update_time = time(nullptr);
  cv->modified = false;
  cv->host_id = 31;
  cv->name = "PROCESSNAME";
  cv->value = "centengine";
  cv->default_value = "centengine";

  conflict_manager::instance().send_event(conflict_manager::sql, cv);

  conflict_manager::close();
}
