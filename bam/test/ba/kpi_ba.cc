/*
** Copyright 2021 Centreon
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

#include "com/centreon/broker/bam/kpi_ba.hh"
#include <fmt/format.h>
#include <gtest/gtest.h>
#include <cstdlib>
#include <memory>
#include <stack>
#include <vector>
#include "com/centreon/broker/bam/ba.hh"
#include "com/centreon/broker/bam/configuration/applier/state.hh"
#include "com/centreon/broker/bam/kpi_service.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/neb/acknowledgement.hh"
#include "com/centreon/broker/neb/downtime.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "test-visitor.hh"

using namespace com::centreon::broker;

class KpiBA : public ::testing::Test {
 protected:
  std::unique_ptr<bam::configuration::applier::state> _aply_state;
  std::unique_ptr<bam::configuration::state> _state;
  std::unique_ptr<test_visitor> _visitor;

 public:
  void SetUp() override {
    // Initialization.
    config::applier::init(0, "test_broker");

    _aply_state.reset(new bam::configuration::applier::state);
    _state.reset(new bam::configuration::state);
    _visitor.reset(new test_visitor("test-visitor"));
  }

  void TearDown() override {
    // Cleanup.
    config::applier::deinit();
  }
};

/**
 *     kpi-service-1 (critical)
 *                  \
 *                   X kpi-ba (3, 2) (test-ba-child) ---- test-ba (1)
 *                  /
 *     kpi-service-2 (ok)
 */
TEST_F(KpiBA, KpiBa) {
  /* Construction of BA1 */
  std::shared_ptr<bam::ba> test_ba{std::make_shared<bam::ba>(
      1, 5, 13, bam::configuration::ba::state_source_worst)};
  test_ba->set_name("test-ba");
  test_ba->set_state_source(bam::configuration::ba::state_source_worst);
  test_ba->set_level_critical(100);
  test_ba->set_level_warning(75);
  test_ba->set_downtime_behaviour(bam::configuration::ba::dt_inherit);

  /* Construction of BA2 */
  std::shared_ptr<bam::ba> test_ba_child{std::make_shared<bam::ba>(
      2, 5, 14, bam::configuration::ba::state_source_worst)};
  test_ba_child->set_name("test-ba-child");
  test_ba_child->set_level_critical(100);
  test_ba_child->set_level_warning(75);
  test_ba_child->set_downtime_behaviour(bam::configuration::ba::dt_inherit);

  std::vector<std::shared_ptr<bam::kpi_service>> kpis;

  /* Construction of kpi_services */
  for (int i = 0; i < 2; i++) {
    auto s = std::make_shared<bam::kpi_service>(i + 1, 2, 3, 1 + i);
    s->set_downtimed(false);
    s->set_impact_critical(100);
    s->set_impact_unknown(0);
    s->set_impact_warning(75);
    s->set_state_hard(bam::kpi_service::state::state_ok);
    s->set_state_type(1);

    // test_ba_child->add_impact(s);
    // s->add_parent(test_ba_child);
    kpis.push_back(s);
  }

  /* Construction of kpi_ba */
  auto kpi_ba_child = std::make_shared<bam::kpi_ba>(3, 2);
  kpi_ba_child->set_impact_critical(100);
  kpi_ba_child->set_impact_warning(75);

  auto kpi_ba = std::make_shared<bam::kpi_ba>(4, 1);
  kpi_ba->set_impact_critical(100);
  kpi_ba->set_impact_warning(75);

  /* Resolutions */
  /* Link between the kpi_ba_child and its ba test_ba_child */
  kpi_ba_child->link_ba(test_ba_child);
  test_ba_child->add_parent(kpi_ba_child);

  /* Link between the kpi_ba and its ba test_ba */
  kpi_ba->link_ba(test_ba);
  test_ba->add_impact(kpi_ba_child);

  /* Link between the kpi_ba_child and the ba parent test_ba */
  test_ba->add_impact(kpi_ba_child);
  kpi_ba_child->add_parent(test_ba);

  for (int i = 0; i < 2; i++) {
    test_ba_child->add_impact(kpis[i]);
    kpis[i]->add_parent(test_ba_child);
  }

  time_t now{time(nullptr)};

  for (int i = 0; i < 2; i++) {
    auto ss = std::make_shared<neb::service_status>();
    ss->host_id = 3;
    ss->service_id = i + 1;

    /* The first kpi is set to status critical. */
    ss->last_check = now;
    ss->last_hard_state = 0;
    kpis[i]->service_update(ss, _visitor.get());
  }

  auto ss = std::make_shared<neb::service_status>();
  ss->host_id = 3;
  ss->service_id = 1;

  /* The first kpi is set to status critical. */
  ss->last_check = now + 10;
  ss->last_hard_state = 2;
  kpis[0]->service_update(ss, _visitor.get());

  auto events = _visitor->queue();

  _visitor->print_events();

  /* We have to check that the test-ba is also critical now */

  /* ba2 set to critical, event open because there was no previous
   * state */
  auto it = events.rbegin();
  ASSERT_EQ(it->typ, test_visitor::test_event::ba);
  ASSERT_EQ(it->ba_id, 1u);
  ASSERT_EQ(it->status, 2);
  ASSERT_FALSE(it->in_downtime);
  ASSERT_EQ(it->end_time, -1);
}

/**
 *     kpi-service-1 (critical)
 *                  \
 *                   X kpi-ba (3, 2) (test-ba-child) ---- test-ba (1)
 *                  /
 *     kpi-service-2 (ok)
 */
TEST_F(KpiBA, KpiBaDt) {
  /* Construction of BA1 */
  std::shared_ptr<bam::ba> test_ba{std::make_shared<bam::ba>(
      1, 5, 13, bam::configuration::ba::state_source_worst)};
  test_ba->set_name("test-ba");
  test_ba->set_state_source(bam::configuration::ba::state_source_worst);
  test_ba->set_level_critical(100);
  test_ba->set_level_warning(75);
  test_ba->set_downtime_behaviour(bam::configuration::ba::dt_inherit);

  /* Construction of BA2 */
  std::shared_ptr<bam::ba> test_ba_child{std::make_shared<bam::ba>(
      2, 5, 14, bam::configuration::ba::state_source_worst)};
  test_ba_child->set_name("test-ba-child");
  test_ba_child->set_level_critical(100);
  test_ba_child->set_level_warning(75);
  test_ba_child->set_downtime_behaviour(bam::configuration::ba::dt_inherit);

  std::vector<std::shared_ptr<bam::kpi_service>> kpis;

  /* Construction of kpi_services */
  for (int i = 0; i < 2; i++) {
    auto s = std::make_shared<bam::kpi_service>(i + 1, 2, 3, 1 + i);
    s->set_downtimed(false);
    s->set_impact_critical(100);
    s->set_impact_unknown(0);
    s->set_impact_warning(75);
    s->set_state_hard(bam::kpi_service::state::state_ok);
    s->set_state_type(1);

    // test_ba_child->add_impact(s);
    // s->add_parent(test_ba_child);
    kpis.push_back(s);
  }

  /* Construction of kpi_ba */
  auto kpi_ba_child = std::make_shared<bam::kpi_ba>(3, 2);
  kpi_ba_child->set_impact_critical(100);
  kpi_ba_child->set_impact_warning(75);

  auto kpi_ba = std::make_shared<bam::kpi_ba>(4, 1);
  kpi_ba->set_impact_critical(100);
  kpi_ba->set_impact_warning(75);

  /* Resolutions */
  /* Link between the kpi_ba_child and its ba test_ba_child */
  kpi_ba_child->link_ba(test_ba_child);
  test_ba_child->add_parent(kpi_ba_child);

  /* Link between the kpi_ba and its ba test_ba */
  kpi_ba->link_ba(test_ba);
  test_ba->add_impact(kpi_ba_child);

  /* Link between the kpi_ba_child and the ba parent test_ba */
  test_ba->add_impact(kpi_ba_child);
  kpi_ba_child->add_parent(test_ba);

  for (int i = 0; i < 2; i++) {
    test_ba_child->add_impact(kpis[i]);
    kpis[i]->add_parent(test_ba_child);
  }

  time_t now{time(nullptr)};

  for (int i = 0; i < 2; i++) {
    auto ss = std::make_shared<neb::service_status>();
    ss->host_id = 3;
    ss->service_id = i + 1;

    /* The first kpi is set to status critical. */
    ss->last_check = now;
    ss->last_hard_state = 0;
    kpis[i]->service_update(ss, _visitor.get());
  }

  auto ss = std::make_shared<neb::service_status>();
  ss->host_id = 3;
  ss->service_id = 1;

  /* The first kpi is set to status critical. */
  ss->last_check = now + 10;
  ss->last_hard_state = 2;
  kpis[0]->service_update(ss, _visitor.get());

  /* Let's put a downtime on the service. */
  auto dt = std::make_shared<neb::downtime>();
  dt->host_id = 3;
  dt->service_id = 1;
  dt->entry_time = now + 12;
  dt->was_started = true;
  kpis[0]->service_update(dt, _visitor.get());

  auto events = _visitor->queue();

  _visitor->print_events();

  /* We have to check that the test-ba is also critical now */

  /* ba2 set to critical, event open because there was no previous
   * state */
  auto it = events.rbegin();
  ASSERT_EQ(it->typ, test_visitor::test_event::ba);
  ASSERT_EQ(it->ba_id, 1u);
  ASSERT_TRUE(it->in_downtime);
  ASSERT_EQ(it->end_time, -1);
}

/**
 *     kpi-service-1 (critical)
 *                  \
 *                   X kpi-ba (3, 2) (test-ba-child) ---- test-ba (1)
 *                  /
 *     kpi-service-2 (ok)
 */
TEST_F(KpiBA, KpiBaDtOff) {
  /* Construction of BA1 */
  std::shared_ptr<bam::ba> test_ba{std::make_shared<bam::ba>(
      1, 5, 13, bam::configuration::ba::state_source_worst)};
  test_ba->set_name("test-ba");
  test_ba->set_state_source(bam::configuration::ba::state_source_worst);
  test_ba->set_level_critical(100);
  test_ba->set_level_warning(75);
  test_ba->set_downtime_behaviour(bam::configuration::ba::dt_inherit);

  /* Construction of BA2 */
  std::shared_ptr<bam::ba> test_ba_child{std::make_shared<bam::ba>(
      2, 5, 14, bam::configuration::ba::state_source_worst)};
  test_ba_child->set_name("test-ba-child");
  test_ba_child->set_level_critical(100);
  test_ba_child->set_level_warning(75);
  test_ba_child->set_downtime_behaviour(bam::configuration::ba::dt_inherit);

  std::vector<std::shared_ptr<bam::kpi_service>> kpis;

  /* Construction of kpi_services */
  for (int i = 0; i < 2; i++) {
    auto s = std::make_shared<bam::kpi_service>(i + 1, 2, 3, 1 + i);
    s->set_downtimed(false);
    s->set_impact_critical(100);
    s->set_impact_unknown(0);
    s->set_impact_warning(75);
    s->set_state_hard(bam::kpi_service::state::state_ok);
    s->set_state_type(1);

    // test_ba_child->add_impact(s);
    // s->add_parent(test_ba_child);
    kpis.push_back(s);
  }

  /* Construction of kpi_ba */
  auto kpi_ba_child = std::make_shared<bam::kpi_ba>(3, 2);
  kpi_ba_child->set_impact_critical(100);
  kpi_ba_child->set_impact_warning(75);

  auto kpi_ba = std::make_shared<bam::kpi_ba>(4, 1);
  kpi_ba->set_impact_critical(100);
  kpi_ba->set_impact_warning(75);

  /* Resolutions */
  /* Link between the kpi_ba_child and its ba test_ba_child */
  kpi_ba_child->link_ba(test_ba_child);
  test_ba_child->add_parent(kpi_ba_child);

  /* Link between the kpi_ba and its ba test_ba */
  kpi_ba->link_ba(test_ba);
  test_ba->add_impact(kpi_ba_child);

  /* Link between the kpi_ba_child and the ba parent test_ba */
  test_ba->add_impact(kpi_ba_child);
  kpi_ba_child->add_parent(test_ba);

  for (int i = 0; i < 2; i++) {
    test_ba_child->add_impact(kpis[i]);
    kpis[i]->add_parent(test_ba_child);
  }

  time_t now{time(nullptr)};

  for (int i = 0; i < 2; i++) {
    auto ss = std::make_shared<neb::service_status>();
    ss->host_id = 3;
    ss->service_id = i + 1;

    ss->last_check = now;
    ss->last_hard_state = 0;
    kpis[i]->service_update(ss, _visitor.get());
  }

  auto ss = std::make_shared<neb::service_status>();
  ss->host_id = 3;
  ss->service_id = 1;

  /* The first kpi is set to status critical. */
  ss->last_check = now + 10;
  ss->last_hard_state = 2;
  kpis[0]->service_update(ss, _visitor.get());

  /* Let's put a downtime on the service. */
  auto dt = std::make_shared<neb::downtime>();
  dt->host_id = 3;
  dt->service_id = 1;
  dt->entry_time = now + 12;
  dt->actual_start_time = now + 12;
  dt->actual_end_time = -1;
  dt->was_started = true;
  kpis[0]->service_update(dt, _visitor.get());

  /* Let's remove the downtime from the service. */
  dt = std::make_shared<neb::downtime>();
  dt->host_id = 3;
  dt->service_id = 1;
  dt->entry_time = now + 12;
  dt->actual_end_time = now + 20;
  dt->deletion_time = now + 20;
  dt->was_started = true;
  dt->was_cancelled = true;
  kpis[0]->service_update(dt, _visitor.get());
  ASSERT_TRUE(!test_ba->get_in_downtime());

  auto events = _visitor->queue();

  _visitor->print_events();

  /* We have to check that the test-ba is no more in downtime */

  auto it = events.rbegin();
  ASSERT_EQ(it->typ, test_visitor::test_event::ba);
  ASSERT_EQ(it->ba_id, 1u);
  ASSERT_FALSE(it->in_downtime);
  ASSERT_EQ(it->end_time, -1);
}

/**
 *     kpi-service-1 (ok)
 *                  \
 *                   X kpi-ba (3, 2) (test-ba-child) ---- test-ba (1)
 *                  /
 *     kpi-service-2 (ok)
 */
TEST_F(KpiBA, KpiBaOkDtOff) {
  /* Construction of BA1 */
  std::shared_ptr<bam::ba> test_ba{std::make_shared<bam::ba>(
      1, 5, 13, bam::configuration::ba::state_source_worst)};
  test_ba->set_name("test-ba");
  test_ba->set_state_source(bam::configuration::ba::state_source_worst);
  test_ba->set_level_critical(100);
  test_ba->set_level_warning(75);
  test_ba->set_downtime_behaviour(bam::configuration::ba::dt_inherit);

  /* Construction of BA2 */
  std::shared_ptr<bam::ba> test_ba_child{std::make_shared<bam::ba>(
      2, 5, 14, bam::configuration::ba::state_source_worst)};
  test_ba_child->set_name("test-ba-child");
  test_ba_child->set_level_critical(100);
  test_ba_child->set_level_warning(75);
  test_ba_child->set_downtime_behaviour(bam::configuration::ba::dt_inherit);

  std::vector<std::shared_ptr<bam::kpi_service>> kpis;

  /* Construction of kpi_services */
  for (int i = 0; i < 2; i++) {
    auto s = std::make_shared<bam::kpi_service>(i + 1, 2, 3, 1 + i);
    s->set_downtimed(false);
    s->set_impact_critical(100);
    s->set_impact_unknown(0);
    s->set_impact_warning(75);
    s->set_state_hard(bam::kpi_service::state::state_ok);
    s->set_state_type(1);

    // test_ba_child->add_impact(s);
    // s->add_parent(test_ba_child);
    kpis.push_back(s);
  }

  /* Construction of kpi_ba */
  auto kpi_ba_child = std::make_shared<bam::kpi_ba>(3, 2);
  kpi_ba_child->set_impact_critical(100);
  kpi_ba_child->set_impact_warning(75);

  auto kpi_ba = std::make_shared<bam::kpi_ba>(4, 1);
  kpi_ba->set_impact_critical(100);
  kpi_ba->set_impact_warning(75);

  /* Resolutions */
  /* Link between the kpi_ba_child and its ba test_ba_child */
  kpi_ba_child->link_ba(test_ba_child);
  test_ba_child->add_parent(kpi_ba_child);

  /* Link between the kpi_ba and its ba test_ba */
  kpi_ba->link_ba(test_ba);
  test_ba->add_impact(kpi_ba_child);

  /* Link between the kpi_ba_child and the ba parent test_ba */
  test_ba->add_impact(kpi_ba_child);
  kpi_ba_child->add_parent(test_ba);

  for (int i = 0; i < 2; i++) {
    test_ba_child->add_impact(kpis[i]);
    kpis[i]->add_parent(test_ba_child);
  }

  time_t now{time(nullptr)};

  for (int i = 0; i < 2; i++) {
    auto ss = std::make_shared<neb::service_status>();
    ss->host_id = 3;
    ss->service_id = i + 1;

    ss->last_check = now;
    ss->last_hard_state = 0;
    kpis[i]->service_update(ss, _visitor.get());
  }

  auto ss = std::make_shared<neb::service_status>();
  ss->host_id = 3;
  ss->service_id = 1;

  /* Let's put a downtime on the service. */
  auto dt = std::make_shared<neb::downtime>();
  dt->host_id = 3;
  dt->service_id = 1;
  dt->entry_time = now + 12;
  dt->actual_start_time = now + 12;
  dt->actual_end_time = -1;
  dt->was_started = true;
  kpis[0]->service_update(dt, _visitor.get());
  ASSERT_FALSE(test_ba->get_in_downtime());

  /* Let's remove the downtime from the service. */
  dt = std::make_shared<neb::downtime>();
  dt->host_id = 3;
  dt->service_id = 1;
  dt->entry_time = now + 12;
  dt->actual_end_time = now + 20;
  dt->deletion_time = now + 20;
  dt->was_started = true;
  dt->was_cancelled = true;
  kpis[0]->service_update(dt, _visitor.get());
  ASSERT_FALSE(test_ba->get_in_downtime());
}

/**
 *  kpi-service-1 (critical)
 *                  \
 *                X kpi-ba(worst, 3, 2) (test-ba-child) ---- test-ba (impact, 1)
 *                  /
 *  kpi-service-2 (ok)
 */
TEST_F(KpiBA, KpiBaWorstImpact) {
  /* Construction of BA1 */
  std::shared_ptr<bam::ba> test_ba{std::make_shared<bam::ba>(
      1, 5, 13, bam::configuration::ba::state_source_impact)};
  test_ba->set_name("test-ba");
  test_ba->set_state_source(bam::configuration::ba::state_source_worst);
  test_ba->set_level_critical(100);
  test_ba->set_level_warning(75);
  test_ba->set_downtime_behaviour(bam::configuration::ba::dt_inherit);

  /* Construction of BA2 */
  std::shared_ptr<bam::ba> test_ba_child{std::make_shared<bam::ba>(
      2, 5, 14, bam::configuration::ba::state_source_worst)};
  test_ba_child->set_name("test-ba-child");
  test_ba_child->set_level_critical(100);
  test_ba_child->set_level_warning(75);
  test_ba_child->set_downtime_behaviour(bam::configuration::ba::dt_inherit);

  std::vector<std::shared_ptr<bam::kpi_service>> kpis;

  /* Construction of kpi_services */
  for (int i = 0; i < 2; i++) {
    auto s = std::make_shared<bam::kpi_service>(i + 1, 2, 3, 1 + i);
    s->set_downtimed(false);
    s->set_impact_critical(100);
    s->set_impact_unknown(0);
    s->set_impact_warning(75);
    s->set_state_hard(bam::kpi_service::state::state_ok);
    s->set_state_type(1);

    kpis.push_back(s);
  }

  /* Construction of kpi_ba */
  auto kpi_ba_child = std::make_shared<bam::kpi_ba>(3, 2);
  kpi_ba_child->set_impact_critical(100);
  kpi_ba_child->set_impact_warning(75);
  kpi_ba_child->set_impact_unknown(27);

  auto kpi_ba = std::make_shared<bam::kpi_ba>(4, 1);
  kpi_ba->set_impact_critical(100);
  kpi_ba->set_impact_warning(75);

  /* Resolutions */
  /* Link between the kpi_ba_child and its ba test_ba_child */
  kpi_ba_child->link_ba(test_ba_child);
  test_ba_child->add_parent(kpi_ba_child);

  /* Link between the kpi_ba and its ba test_ba */
  kpi_ba->link_ba(test_ba);
  test_ba->add_impact(kpi_ba_child);

  /* Link between the kpi_ba_child and the ba parent test_ba */
  test_ba->add_impact(kpi_ba_child);
  kpi_ba_child->add_parent(test_ba);

  for (int i = 0; i < 2; i++) {
    test_ba_child->add_impact(kpis[i]);
    kpis[i]->add_parent(test_ba_child);
  }

  time_t now{time(nullptr)};

  for (int i = 0; i < 2; i++) {
    auto ss = std::make_shared<neb::service_status>();
    ss->host_id = 3;
    ss->service_id = i + 1;

    ss->last_check = now;
    ss->last_hard_state = 0;
    kpis[i]->service_update(ss, _visitor.get());
  }

  auto ss = std::make_shared<neb::service_status>();
  ss->host_id = 3;
  ss->service_id = 1;
  ss->last_check = now + 10;
  ss->last_hard_state = 3;
  kpis[0]->service_update(ss, _visitor.get());

  _visitor->print_events();

  std::cout << "ba state: " << test_ba->get_state_hard() << std::endl;
  std::cout << "ba hard impact: " << test_ba->get_downtime_impact_hard() << std::endl;
  std::cout << "ba perfdata: " << test_ba->get_perfdata() << std::endl;

}
