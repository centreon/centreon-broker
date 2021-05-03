/*
** Copyright 2014, 2021 Centreon
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

#include "com/centreon/broker/bam/kpi_service.hh"
#include <fmt/format.h>
#include <gtest/gtest.h>
#include <cstdlib>
#include <memory>
#include <stack>
#include <vector>
#include "com/centreon/broker/bam/ba.hh"
#include "com/centreon/broker/bam/configuration/applier/state.hh"
#include "com/centreon/broker/bam/kpi_ba.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/neb/acknowledgement.hh"
#include "com/centreon/broker/neb/downtime.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "test-visitor.hh"

using namespace com::centreon::broker;

class BamBA : public ::testing::Test {
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
 * Check that KPI change at BA recompute does not mess with the BA
 * value.
 */
TEST_F(BamBA, KpiServiceRecompute) {
  // Build BAM objects.
  std::shared_ptr<bam::ba> test_ba{std::make_shared<bam::ba>(
      1, 1, 1, bam::configuration::ba::state_source_impact)};

  std::shared_ptr<bam::kpi_service> kpi{
      std::make_shared<bam::kpi_service>(1, 1, 1, 1)};

  kpi->set_impact_critical(100.0);
  kpi->set_state_hard(bam::kpi_service::state::state_ok);
  kpi->set_state_soft(kpi->get_state_hard());
  test_ba->add_impact(kpi);
  kpi->add_parent(test_ba);

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));
  for (int i = 0; i < 100 + 2; ++i) {
    std::shared_ptr<neb::service_status> ss(new neb::service_status);
    ss->host_id = 1;
    ss->service_id = 1;
    ss->last_check = now + i;
    ss->last_hard_state = ((i & 1) ? 0 : 2);
    ss->current_state = ss->last_hard_state;
    kpi->service_update(ss, _visitor.get());
  }

  ASSERT_EQ(test_ba->get_state_hard(), 0);
}

/**
 *  Check that a KPI change at BA recompute does not mess with the BA
 *  value.
 *
 *                 ----------------
 *         ________| BA(C40%:W70%)|___________
 *        /        ----------------           \
 *       |                  |                 |
 *  KPI1(C20%:W10%)   KPI2(C20%:W10%)  KPI3(C20%:W10%)
 *       |                  |                 |
 *      H1S1               H2S1             H3S1
 *
 *  @return EXIT_SUCCESS on success.
 */
TEST_F(BamBA, KpiServiceImpactState) {
  // Build BAM objects.
  std::shared_ptr<bam::ba> test_ba{std::make_shared<bam::ba>(
      1, 1, 2, bam::configuration::ba::state_source_impact)};

  std::vector<std::shared_ptr<bam::kpi_service>> kpis;
  std::vector<short> results{0, 0, 1, 1, 1, 2};

  for (int i = 0; i < 3; i++) {
    auto s = std::make_shared<bam::kpi_service>(i + 1, 1, i + 1, 1);
    s->set_impact_warning(10);
    s->set_impact_critical(20);
    s->set_state_hard(bam::kpi_service::state::state_ok);
    s->set_state_soft(s->get_state_hard());
    test_ba->add_impact(s);
    s->add_parent(test_ba);
    kpis.push_back(s);
  }

  test_ba->set_level_warning(70.0);
  test_ba->set_level_critical(40.0);

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));

  std::shared_ptr<neb::service_status> ss{
      std::make_shared<neb::service_status>()};
  ss->service_id = 1;

  auto it = results.begin();
  for (int i = 0; i < 2; i++) {
    for (size_t j = 0; j < kpis.size(); j++) {
      ss->last_check = now + 1 + i;
      ss->host_id = j + 1;
      ss->last_hard_state = i + 1;
      ss->current_state = ss->last_hard_state;
      kpis[j]->service_update(ss, _visitor.get());

      short val = *it;
      ASSERT_EQ(test_ba->get_state_soft(), val);
      ASSERT_EQ(test_ba->get_state_hard(), val);
      ++it;
    }
  }
  auto events = _visitor->queue();
  ASSERT_EQ(events.size(), 14u);

  _visitor->print_events();

  {
    auto it = events.begin();
    while (it->typ != test_visitor::test_event::kpi)
      ++it;
    /* The three kpi have a status 1 and downtime set to false */
    for (int i = 0; i < 3; i++) {
      ASSERT_EQ(it->end_time, -1);
      ASSERT_EQ(it->status, 1);
      ASSERT_FALSE(it->in_downtime);
      do {
        ++it;
      } while (it->typ != test_visitor::test_event::kpi);
    }

    /* We close the event with status 1 and we create a new one with status 2 */
    for (int i = 0; i < 3; i++) {
      ASSERT_EQ(it->start_time, now + 1);
      ASSERT_EQ(it->end_time, now + 2);
      ASSERT_EQ(it->status, 1);
      ASSERT_FALSE(it->in_downtime);
      do {
        ++it;
      } while (it->typ != test_visitor::test_event::kpi);

      ASSERT_EQ(it->start_time, now + 2);
      ASSERT_EQ(it->end_time, -1);
      ASSERT_EQ(it->status, 2);
      ASSERT_FALSE(it->in_downtime);
      do {
        ++it;
      } while (it->typ != test_visitor::test_event::kpi);
    }
  }
}

/**
 *  Check that a KPI change at BA recompute does not mess with the BA
 *  value.
 *
 *                 ----------------
 *         ________| BA(BEST)     |___________
 *        /        ----------------           \
 *       |                  |                 |
 *  KPI1(C20%:W10%)   KPI2(C20%:W10%)  KPI3(C20%:W10%)
 *       |                  |                 |
 *      H1S1               H2S1             H3S1
 *
 *  @return EXIT_SUCCESS on success.
 */
TEST_F(BamBA, KpiServiceBestState) {
  // Build BAM objects.
  std::shared_ptr<bam::ba> test_ba{std::make_shared<bam::ba>(
      1, 1, 3, bam::configuration::ba::state_source_best)};

  std::vector<std::shared_ptr<bam::kpi_service>> kpis;
  std::vector<short> results{0, 0, 1, 1, 1, 2};

  for (size_t i = 0; i < 3; i++) {
    auto s = std::make_shared<bam::kpi_service>(i + 1, 1, i + 1, 1);
    s->set_state_hard(bam::kpi_service::state::state_ok);
    s->set_state_soft(s->get_state_hard());
    test_ba->add_impact(s);
    s->add_parent(test_ba);
    kpis.push_back(s);
  }

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));

  std::shared_ptr<neb::service_status> ss(new neb::service_status);
  ss->service_id = 1;

  auto it = results.begin();
  for (int i = 0; i < 2; i++) {
    for (size_t j = 0; j < kpis.size(); j++) {
      ss->last_check = now + 1;
      ss->host_id = j + 1;
      ss->last_hard_state = i + 1;
      ss->current_state = ss->last_hard_state;
      kpis[j]->service_update(ss, _visitor.get());

      short val = *it;
      ASSERT_EQ(test_ba->get_state_soft(), val);
      ASSERT_EQ(test_ba->get_state_hard(), val);
      ++it;
    }
  }
}

/**
 *  Check that a KPI change at BA recompute does not mess with the BA
 *  value.
 *
 *                 ----------------
 *         ________| BA(WORST)     |___________
 *        /        ----------------           \
 *       |                  |                 |
 *  KPI1(C20%:W10%)   KPI2(C20%:W10%)  KPI3(C20%:W10%)
 *       |                  |                 |
 *      H1S1               H2S1             H3S1
 *
 *  @return EXIT_SUCCESS on success.
 */
TEST_F(BamBA, KpiServiceWorstState) {
  // Build BAM objects.
  std::shared_ptr<bam::ba> test_ba{std::make_shared<bam::ba>(
      1, 1, 4, bam::configuration::ba::state_source_worst)};

  std::vector<std::shared_ptr<bam::kpi_service>> kpis;
  std::vector<short> results{1, 1, 1, 2, 2, 2};

  for (int i = 0; i < 3; i++) {
    auto s = std::make_shared<bam::kpi_service>(i + 1, 1, i + 1, 1);
    s->set_impact_warning(10);
    s->set_impact_critical(20);
    s->set_state_hard(bam::kpi_service::state::state_ok);
    s->set_state_soft(s->get_state_hard());
    test_ba->add_impact(s);
    s->add_parent(test_ba);
    kpis.push_back(s);
  }

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));

  std::shared_ptr<neb::service_status> ss(new neb::service_status);
  ss->service_id = 1;

  auto it = results.begin();
  for (int i = 0; i < 2; i++) {
    for (size_t j = 0; j < kpis.size(); j++) {
      ss->last_check = now + 1 + i;
      ss->host_id = j + 1;
      ss->last_hard_state = i + 1;
      ss->current_state = ss->last_hard_state;
      kpis[j]->service_update(ss, _visitor.get());

      short val = *it;
      ASSERT_EQ(test_ba->get_state_soft(), val);
      ASSERT_EQ(test_ba->get_state_hard(), val);
      ++it;
    }
  }

  auto events = _visitor->queue();
  ASSERT_EQ(events.size(), 12u);

  _visitor->print_events();

  {
    auto it = events.begin();
    /* The three kpi have a status 1 and downtime set to false */
    for (int i = 0; i < 3; i++) {
      ASSERT_EQ(it->end_time, -1);
      ASSERT_EQ(it->status, 1);
      ASSERT_FALSE(it->in_downtime);
      do {
        ++it;
      } while (it->typ != test_visitor::test_event::kpi);
    }

    /* We close the event with status 1 and we create a new one with status 2 */
    for (int i = 0; i < 3; i++) {
      ASSERT_EQ(it->start_time, now + 1);
      ASSERT_EQ(it->end_time, now + 2);
      ASSERT_EQ(it->status, 1);
      ASSERT_FALSE(it->in_downtime);
      do {
        ++it;
      } while (it->typ != test_visitor::test_event::kpi);

      ASSERT_EQ(it->start_time, now + 2);
      ASSERT_EQ(it->end_time, -1);
      ASSERT_EQ(it->status, 2);
      ASSERT_FALSE(it->in_downtime);
      do {
        ++it;
      } while (it->typ != test_visitor::test_event::kpi);
    }
  }
}

/**
 *  Check that a KPI change at BA recompute does not mess with the BA
 *  value.
 *
 *                 ----------------
 *         ________| BA(RAtioNUm) |____________________________
 *        /        ----------------           \                \
 *       |                  |                 |                \
 *        ---------------2 C -> W , 4 C -> C -------------------
 *       |                  |                 |                \
 *      H1S1               H2S1             H3S1               H4S1
 *
 *  @return EXIT_SUCCESS on success.
 */
TEST_F(BamBA, KpiServiceRatioNum) {
  // Build BAM objects.
  std::shared_ptr<bam::ba> test_ba{std::make_shared<bam::ba>(
      1, 1, 4, bam::configuration::ba::state_source_ratio_number)};
  test_ba->set_level_critical(4);
  test_ba->set_level_warning(2);

  std::vector<std::shared_ptr<bam::kpi_service>> kpis;
  std::stack<short> results;

  results.push(2);
  results.push(1);
  results.push(1);
  results.push(0);

  for (int i = 0; i < 4; i++) {
    std::shared_ptr<bam::kpi_service> s =
        std::make_shared<bam::kpi_service>(i + 1, 1, i + 1, 1);
    s->set_state_hard(bam::kpi_service::state::state_ok);
    s->set_state_soft(s->get_state_hard());
    test_ba->add_impact(s);
    s->add_parent(test_ba);
    kpis.push_back(s);
  }

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));

  std::shared_ptr<neb::service_status> ss(new neb::service_status);
  ss->service_id = 1;

  for (size_t j = 0; j < kpis.size(); j++) {
    ss->last_check = now + 1;
    ss->host_id = j + 1;
    ss->last_hard_state = 2;
    ss->current_state = ss->last_hard_state;
    kpis[j]->service_update(ss, _visitor.get());

    short val = results.top();
    ASSERT_EQ(test_ba->get_state_soft(), val);
    ASSERT_EQ(test_ba->get_state_hard(), val);
    results.pop();
  }
}

/**
 *  Check that a KPI change at BA recompute does not mess with the BA
 *  value.
 *
 *                 ----------------
 *         ________| BA(RAtio%  ) |____________________________
 *        /        ----------------           \                \
 *       |                  |                 |                \
 *        ---------------75% C -> W , 100% C -> C -------------------
 *       |                  |                 |                \
 *      H1S1               H2S1             H3S1               H4S1
 *
 *  @return EXIT_SUCCESS on success.
 */
TEST_F(BamBA, KpiServiceRatioPercent) {
  // Build BAM objects.
  std::shared_ptr<bam::ba> test_ba{std::make_shared<bam::ba>(
      1, 1, 4, bam::configuration::ba::state_source_ratio_percent)};
  test_ba->set_level_critical(100);
  test_ba->set_level_warning(75);

  std::vector<std::shared_ptr<bam::kpi_service>> kpis;
  std::stack<short> results;

  results.push(2);
  results.push(1);
  results.push(0);
  results.push(0);

  for (int i = 0; i < 4; i++) {
    auto s = std::make_shared<bam::kpi_service>(i + 1, 1, i + 1, 1);
    s->set_state_hard(bam::kpi_service::state::state_ok);
    s->set_state_soft(s->get_state_hard());
    test_ba->add_impact(s);
    s->add_parent(test_ba);
    kpis.push_back(s);
  }

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));

  std::shared_ptr<neb::service_status> ss(new neb::service_status);
  ss->service_id = 1;

  for (size_t j = 0; j < kpis.size(); j++) {
    ss->last_check = now + 1;
    ss->host_id = j + 1;
    ss->last_hard_state = 2;
    ss->current_state = ss->last_hard_state;
    kpis[j]->service_update(ss, _visitor.get());

    short val = results.top();
    ASSERT_EQ(test_ba->get_state_soft(), val);
    ASSERT_EQ(test_ba->get_state_hard(), val);
    results.pop();
  }
}

TEST_F(BamBA, KpiServiceDtInheritAllCritical) {
  std::shared_ptr<bam::ba> test_ba{std::make_shared<bam::ba>(
      1, 1, 4, bam::configuration::ba::state_source_ratio_percent)};
  test_ba->set_level_critical(100);
  test_ba->set_level_warning(75);
  test_ba->set_downtime_behaviour(bam::configuration::ba::dt_inherit);

  std::vector<std::shared_ptr<bam::kpi_service>> kpis;
  std::stack<bool> results;

  results.push(true);
  results.push(false);
  results.push(false);
  results.push(false);

  for (int i = 0; i < 4; i++) {
    auto s = std::make_shared<bam::kpi_service>(i + 1, 1, i + 1, 1);
    s->set_state_hard(bam::kpi_service::state::state_critical);
    s->set_state_soft(s->get_state_hard());
    test_ba->add_impact(s);
    s->add_parent(test_ba);
    kpis.push_back(s);
  }

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));

  std::shared_ptr<neb::service_status> ss(new neb::service_status);
  std::shared_ptr<neb::downtime> dt(new neb::downtime);
  ss->service_id = 1;

  for (size_t j = 0; j < kpis.size(); j++) {
    ss->last_check = now + 1;
    ss->host_id = j + 1;
    ss->last_hard_state = 2;
    kpis[j]->service_update(ss, _visitor.get());

    dt->host_id = ss->host_id;
    dt->service_id = 1;
    dt->was_started = true;
    dt->actual_end_time = 0;
    kpis[j]->service_update(dt, _visitor.get());

    short val = results.top();
    ASSERT_EQ(test_ba->get_in_downtime(), val);
    results.pop();
  }
}

TEST_F(BamBA, KpiServiceDtInheritOneOK) {
  std::shared_ptr<bam::ba> test_ba{std::make_shared<bam::ba>(
      1, 1, 4, bam::configuration::ba::state_source_ratio_percent)};
  test_ba->set_level_critical(100);
  test_ba->set_level_warning(90);
  test_ba->set_downtime_behaviour(bam::configuration::ba::dt_inherit);

  std::vector<std::shared_ptr<bam::kpi_service>> kpis;
  std::stack<bool> results;

  results.push(false);
  results.push(false);
  results.push(false);
  results.push(false);

  for (int i = 0; i < 4; i++) {
    auto s = std::make_shared<bam::kpi_service>(i + 1, 1, i + 1, 1);
    if (i == 0)
      s->set_state_hard(bam::kpi_service::state::state_ok);
    else
      s->set_state_hard(bam::kpi_service::state::state_critical);
    s->set_state_soft(s->get_state_hard());
    test_ba->add_impact(s);
    s->add_parent(test_ba);
    kpis.push_back(s);
  }

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));

  std::shared_ptr<neb::service_status> ss(new neb::service_status);
  std::shared_ptr<neb::downtime> dt(new neb::downtime);
  ss->service_id = 1;

  for (size_t j = 0; j < kpis.size(); j++) {
    ss->last_check = now + 1;
    ss->host_id = j + 1;
    ss->service_id = 1;
    if (j == 0)
      ss->last_hard_state = 0;
    else
      ss->last_hard_state = 2;
    kpis[j]->service_update(ss, _visitor.get());

    dt->host_id = ss->host_id;
    dt->service_id = 1;
    dt->was_started = true;
    dt->actual_end_time = 0;
    kpis[j]->service_update(dt, _visitor.get());

    short val = results.top();
    ASSERT_EQ(test_ba->get_in_downtime(), val);
    results.pop();
  }

  auto events = _visitor->queue();
  ASSERT_EQ(events.size(), 5u);
  _visitor->print_events();
}

TEST_F(BamBA, KpiServiceIgnoreDt) {
  std::shared_ptr<bam::ba> test_ba{std::make_shared<bam::ba>(
      1, 1, 4, bam::configuration::ba::state_source_ratio_percent)};
  test_ba->set_level_critical(100);
  test_ba->set_level_warning(75);
  test_ba->set_downtime_behaviour(bam::configuration::ba::dt_ignore);

  std::vector<std::shared_ptr<bam::kpi_service>> kpis;
  std::stack<bool> results;

  results.push(false);
  results.push(false);
  results.push(false);
  results.push(false);

  for (int i = 0; i < 4; i++) {
    auto s = std::make_shared<bam::kpi_service>(i + 1, 1, i + 1, 1);
    s->set_state_hard(bam::kpi_service::state::state_critical);
    s->set_state_soft(s->get_state_hard());
    test_ba->add_impact(s);
    s->add_parent(test_ba);
    kpis.push_back(s);
  }

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));

  std::shared_ptr<neb::service_status> ss(new neb::service_status);
  std::shared_ptr<neb::downtime> dt(new neb::downtime);
  ss->service_id = 1;

  for (size_t j = 0; j < kpis.size(); j++) {
    ss->last_check = now + 1;
    ss->host_id = j + 1;
    ss->last_hard_state = 2;
    kpis[j]->service_update(ss, _visitor.get());

    dt->host_id = ss->host_id;
    dt->service_id = 1;
    dt->was_started = true;
    dt->actual_end_time = 0;
    kpis[j]->service_update(dt, _visitor.get());

    short val = results.top();
    ASSERT_EQ(test_ba->get_in_downtime(), val);
    results.pop();
  }
}

TEST_F(BamBA, KpiServiceDtIgnoreKpi) {
  std::shared_ptr<bam::ba> test_ba{std::make_shared<bam::ba>(
      1, 1, 4, bam::configuration::ba::state_source_ratio_percent)};
  test_ba->set_level_critical(100);
  test_ba->set_level_warning(75);
  test_ba->set_downtime_behaviour(bam::configuration::ba::dt_ignore_kpi);

  std::vector<std::shared_ptr<bam::kpi_service>> kpis;
  std::stack<bool> results;

  results.push(false);
  results.push(false);
  results.push(false);
  results.push(false);

  for (int i = 0; i < 4; i++) {
    auto s = std::make_shared<bam::kpi_service>(i + 1, 1, i + 1, 1);
    s->set_state_hard(bam::kpi_service::state::state_critical);
    s->set_state_soft(s->get_state_hard());
    test_ba->add_impact(s);
    s->add_parent(test_ba);
    kpis.push_back(s);
  }

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));

  std::shared_ptr<neb::service_status> ss(new neb::service_status);
  std::shared_ptr<neb::downtime> dt(new neb::downtime);
  ss->service_id = 1;

  for (size_t j = 0; j < kpis.size(); j++) {
    ss->last_check = now + 1;
    ss->host_id = j + 1;
    ss->last_hard_state = 2;
    kpis[j]->service_update(ss, _visitor.get());

    dt->host_id = ss->host_id;
    dt->service_id = 1;
    dt->was_started = true;
    dt->actual_end_time = 0;
    kpis[j]->service_update(dt, _visitor.get());

    short val = results.top();
    ASSERT_EQ(test_ba->get_in_downtime(), val);
    results.pop();
  }
}

TEST_F(BamBA, KpiServiceDtIgnoreKpiImpact) {
  std::shared_ptr<bam::ba> test_ba{std::make_shared<bam::ba>(
      1, 1, 4, bam::configuration::ba::state_source_impact)};
  test_ba->set_level_critical(50);
  test_ba->set_level_warning(75);
  test_ba->set_downtime_behaviour(bam::configuration::ba::dt_ignore_kpi);

  std::vector<std::shared_ptr<bam::kpi_service>> kpis;
  std::stack<short> results;

  results.push(0);
  results.push(0);
  results.push(1);
  results.push(2);

  for (int i = 0; i < 4; i++) {
    auto s = std::make_shared<bam::kpi_service>(i + 1, 1, i + 1, 1);
    if (i == 3)
      s->set_state_hard(bam::kpi_service::state::state_ok);
    else
      s->set_state_hard(bam::kpi_service::state::state_critical);
    s->set_state_soft(s->get_state_hard());
    s->set_impact_critical(25);
    test_ba->add_impact(s);
    s->add_parent(test_ba);
    kpis.push_back(s);
  }

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));

  std::shared_ptr<neb::service_status> ss(new neb::service_status);
  std::shared_ptr<neb::downtime> dt(new neb::downtime);
  ss->service_id = 1;

  for (size_t j = 0; j < kpis.size(); j++) {
    ss->last_check = now + 1;
    ss->host_id = j + 1;
    if (j == 3)
      ss->last_hard_state = 0;
    else
      ss->last_hard_state = 2;
    kpis[j]->service_update(ss, _visitor.get());

    dt->host_id = ss->host_id;
    dt->service_id = 1;
    dt->was_started = true;
    dt->actual_end_time = 0;
    kpis[j]->service_update(dt, _visitor.get());

    short val = results.top();
    ASSERT_EQ(test_ba->get_state_hard(), val);
    results.pop();
  }
}

TEST_F(BamBA, KpiServiceDtIgnoreKpiBest) {
  std::shared_ptr<bam::ba> test_ba{std::make_shared<bam::ba>(
      1, 1, 4, bam::configuration::ba::state_source_best)};
  test_ba->set_downtime_behaviour(bam::configuration::ba::dt_ignore_kpi);

  std::vector<std::shared_ptr<bam::kpi_service>> kpis;
  std::stack<short> results;

  results.push(0);
  results.push(2);
  results.push(1);
  results.push(0);

  for (int i = 0; i < 4; i++) {
    auto s = std::make_shared<bam::kpi_service>(i + 1, 1, i + 1, 1);
    switch (i) {
      case 0:
      case 1:
        s->set_state_hard(bam::kpi_service::state::state_ok);
        break;
      case 2:
        s->set_state_hard(bam::kpi_service::state::state_warning);
        break;
      case 3:
        s->set_state_hard(bam::kpi_service::state::state_critical);
        break;
    }
    s->set_state_soft(s->get_state_hard());
    test_ba->add_impact(s);
    s->add_parent(test_ba);
    kpis.push_back(s);
  }

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));

  std::shared_ptr<neb::service_status> ss(new neb::service_status);
  std::shared_ptr<neb::downtime> dt(new neb::downtime);
  ss->service_id = 1;

  for (size_t j = 0; j < kpis.size(); j++) {
    ss->last_check = now + 1;
    ss->host_id = j + 1;
    ss->last_hard_state = kpis[j]->get_state_hard();
    kpis[j]->service_update(ss, _visitor.get());

    dt->host_id = ss->host_id;
    dt->service_id = 1;
    dt->was_started = true;
    dt->actual_end_time = 0;
    kpis[j]->service_update(dt, _visitor.get());

    short val = results.top();
    ASSERT_EQ(test_ba->get_state_hard(), val);
    results.pop();
  }
}

TEST_F(BamBA, KpiServiceDtIgnoreKpiWorst) {
  std::shared_ptr<bam::ba> test_ba{std::make_shared<bam::ba>(
      1, 1, 4, bam::configuration::ba::state_source_worst)};
  test_ba->set_downtime_behaviour(bam::configuration::ba::dt_ignore_kpi);

  std::vector<std::shared_ptr<bam::kpi_service>> kpis;
  std::stack<short> results;

  results.push(0);
  results.push(0);
  results.push(1);
  results.push(2);

  for (int i = 0; i < 4; i++) {
    auto s = std::make_shared<bam::kpi_service>(i + 1, 1, i + 1, 1);
    switch (i) {
      case 0:
      case 1:
        s->set_state_hard(bam::kpi_service::state::state_critical);
        break;
      case 2:
        s->set_state_hard(bam::kpi_service::state::state_warning);
        break;
      case 3:
        s->set_state_hard(bam::kpi_service::state::state_ok);
        break;
    }
    s->set_state_soft(s->get_state_hard());
    test_ba->add_impact(s);
    s->add_parent(test_ba);
    kpis.push_back(s);
  }

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));

  std::shared_ptr<neb::service_status> ss(new neb::service_status);
  std::shared_ptr<neb::downtime> dt(new neb::downtime);
  ss->service_id = 1;

  for (size_t j = 0; j < kpis.size(); j++) {
    ss->last_check = now + 1;
    ss->host_id = j + 1;
    ss->last_hard_state = kpis[j]->get_state_hard();
    kpis[j]->service_update(ss, _visitor.get());

    dt->host_id = ss->host_id;
    dt->service_id = 1;
    dt->was_started = true;
    dt->actual_end_time = 0;
    kpis[j]->service_update(dt, _visitor.get());

    short val = results.top();
    ASSERT_EQ(test_ba->get_state_hard(), val);
    results.pop();
  }
}

TEST_F(BamBA, KpiServiceDtIgnoreKpiRatio) {
  std::shared_ptr<bam::ba> test_ba{std::make_shared<bam::ba>(
      1, 1, 4, bam::configuration::ba::state_source_ratio_number)};
  test_ba->set_downtime_behaviour(bam::configuration::ba::dt_ignore_kpi);
  test_ba->set_level_warning(1);
  test_ba->set_level_critical(2);

  std::vector<std::shared_ptr<bam::kpi_service>> kpis;
  std::stack<short> results;

  results.push(0);
  results.push(1);
  results.push(2);
  results.push(2);

  for (int i = 0; i < 4; i++) {
    auto s = std::make_shared<bam::kpi_service>(i + 1, 1, i + 1, 1);
    s->set_state_hard(bam::kpi_service::state::state_critical);
    s->set_state_soft(s->get_state_hard());
    test_ba->add_impact(s);
    s->add_parent(test_ba);
    kpis.push_back(s);
  }

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));

  std::shared_ptr<neb::service_status> ss(new neb::service_status);
  std::shared_ptr<neb::downtime> dt(new neb::downtime);
  ss->service_id = 1;

  for (size_t j = 0; j < kpis.size(); j++) {
    ss->last_check = now + 1;
    ss->host_id = j + 1;
    ss->last_hard_state = kpis[j]->get_state_hard();
    kpis[j]->service_update(ss, _visitor.get());

    dt->host_id = ss->host_id;
    dt->service_id = 1;
    dt->was_started = true;
    dt->actual_end_time = 0;
    kpis[j]->service_update(dt, _visitor.get());

    short val = results.top();
    ASSERT_EQ(test_ba->get_state_hard(), val);
    results.pop();
  }
}

TEST_F(BamBA, KpiServiceDt) {
  std::shared_ptr<bam::ba> test_ba{std::make_shared<bam::ba>(
      1, 1, 4, bam::configuration::ba::state_source_ratio_percent)};
  test_ba->set_level_critical(100);
  test_ba->set_level_warning(75);
  test_ba->set_downtime_behaviour(bam::configuration::ba::dt_inherit);

  std::vector<std::shared_ptr<bam::kpi_service>> kpis;
  std::vector<bool> results{false, false, false, true};

  for (int i = 0; i < 4; i++) {
    auto s = std::make_shared<bam::kpi_service>(i + 1, 1, i + 1, 1);
    s->set_state_hard(bam::kpi_service::state::state_critical);
    s->set_state_soft(s->get_state_hard());
    test_ba->add_impact(s);
    s->add_parent(test_ba);
    kpis.push_back(s);
  }

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));

  std::shared_ptr<neb::service_status> ss(new neb::service_status);
  ss->service_id = 1;

  std::shared_ptr<neb::downtime> dt(std::make_shared<neb::downtime>());

  auto it = results.begin();
  for (size_t j = 0; j < kpis.size(); j++) {
    ss->last_check = now + 1;
    ss->host_id = j + 1;
    ss->last_hard_state = 2;
    kpis[j]->service_update(ss, _visitor.get());

    dt->host_id = ss->host_id;
    dt->service_id = 1;
    dt->was_started = true;
    dt->actual_start_time = now + 1;
    dt->actual_end_time = 0;
    kpis[j]->service_update(dt, _visitor.get());

    short val = *it;
    ASSERT_EQ(test_ba->get_in_downtime(), val);
    ++it;
  }

  for (int i = 0; i < 3; i++) {
    dt->host_id = 1;
    dt->service_id = 1;
    dt->actual_start_time = now + 2 + 10 * i;
    dt->actual_end_time = 0;
    dt->was_started = true;
    std::cout << "service_update 1" << std::endl;
    kpis[0]->service_update(dt, _visitor.get());

    dt->actual_end_time = now + 2 + 10 * i + 5;
    dt->was_cancelled = true;
    std::cout << "service_update 2" << std::endl;
    kpis[0]->service_update(dt, _visitor.get());
  }
  auto events = _visitor->queue();

  _visitor->print_events();

  ASSERT_EQ(events.size(), 41u);
  {
    auto it = events.begin();
    /* For each kpi... */
    for (int i = 0; i < 4; i++) {
      /* the kpi is set to hard critical and not in downtime */
      ASSERT_EQ(it->start_time, now + 1);
      ASSERT_EQ(it->end_time, -1);
      ASSERT_EQ(it->in_downtime, false);
      do {
        ++it;
      } while (it->typ != test_visitor::test_event::kpi);

      /* the kpi is set in downtime */
      /* The previous event is closed */
      ASSERT_EQ(it->start_time, now + 1);
      ASSERT_EQ(it->end_time, now + 1);
      ASSERT_EQ(it->in_downtime, false);
      do {
        ++it;
      } while (it->typ != test_visitor::test_event::kpi);

      /* The new event on downtime is added and open */
      ASSERT_EQ(it->start_time, now + 1);
      ASSERT_EQ(it->end_time, -1);
      ASSERT_EQ(it->in_downtime, true);
      do {
        ++it;
      } while (it->typ != test_visitor::test_event::kpi);
    }

    ////////////////////////////////////////////////////////////////

    /* We set the downtime to true for the kpi1, but it is already in downtime,
     * the event is skipped. Then we remove the downtime.
     *   1. closure of the event concerning the downtime open
     *   2. new event for downtime.
     *   3. closure of the downtime event.
     *   4. new event with downtime set to false.
     */

    ASSERT_EQ(it->start_time, now + 1);
    ASSERT_EQ(it->end_time, now + 7);
    ASSERT_TRUE(it->in_downtime);
    do {
      ++it;
    } while (it->typ != test_visitor::test_event::kpi);

    ASSERT_EQ(it->start_time, now + 7);
    ASSERT_EQ(it->end_time, -1);
    ASSERT_FALSE(it->in_downtime);
    do {
      ++it;
    } while (it->typ != test_visitor::test_event::kpi);

    /* New downtime:
     *   1. closure of the event concerning the downtime off.
     *   2. new event with downtime on.
     */
    ASSERT_EQ(it->start_time, now + 7);
    ASSERT_EQ(it->end_time, now + 12);
    ASSERT_FALSE(it->in_downtime);
    do {
      ++it;
    } while (it->typ != test_visitor::test_event::kpi);

    ASSERT_EQ(it->start_time, now + 12);
    ASSERT_EQ(it->end_time, -1);
    ASSERT_TRUE(it->in_downtime);
    do {
      ++it;
    } while (it->typ != test_visitor::test_event::kpi);

    /* New downtime:
     *   1. closure of the event concerning the downtime on.
     *   2. new event with downtime on.
     */
    ASSERT_EQ(it->start_time, now + 12);
    ASSERT_EQ(it->end_time, now + 17);
    ASSERT_TRUE(it->in_downtime);
    do {
      ++it;
    } while (it->typ != test_visitor::test_event::kpi);

    ASSERT_EQ(it->start_time, now + 17);
    ASSERT_EQ(it->end_time, -1);
    ASSERT_FALSE(it->in_downtime);
    do {
      ++it;
    } while (it->typ != test_visitor::test_event::kpi);
  }
  ASSERT_FALSE(test_ba->get_in_downtime());
}

TEST_F(BamBA, KpiServiceDtInherited_set) {
  std::shared_ptr<bam::ba> test_ba{std::make_shared<bam::ba>(
      1, 1, 4, bam::configuration::ba::state_source_ratio_percent)};
  test_ba->set_level_critical(100);
  test_ba->set_level_warning(75);
  test_ba->set_downtime_behaviour(bam::configuration::ba::dt_inherit);

  std::vector<std::shared_ptr<bam::kpi_service>> kpis;
  std::vector<bool> results{false, false, false, true};

  for (int i = 0; i < 4; i++) {
    auto s = std::make_shared<bam::kpi_service>(i + 1, 1, i + 1, 1);
    s->set_state_hard(bam::kpi_service::state::state_critical);
    s->set_state_soft(s->get_state_hard());
    test_ba->add_impact(s);
    s->add_parent(test_ba);
    kpis.push_back(s);
  }

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));

  std::shared_ptr<neb::service_status> ss(new neb::service_status);
  ss->service_id = 1;

  std::shared_ptr<neb::downtime> dt(std::make_shared<neb::downtime>());

  auto it = results.begin();
  for (size_t j = 0; j < kpis.size(); j++) {
    ss->last_check = now + 1;
    ss->host_id = j + 1;
    ss->last_hard_state = 2;
    kpis[j]->service_update(ss, _visitor.get());

    dt->host_id = ss->host_id;
    dt->service_id = 1;
    dt->was_started = true;
    dt->actual_start_time = now + 1;
    dt->actual_end_time = 0;
    kpis[j]->service_update(dt, _visitor.get());

    short val = *it;
    ASSERT_EQ(test_ba->get_in_downtime(), val);
    ++it;
  }

  for (int i = 0; i < 3; i++) {
    dt->host_id = 1;
    dt->service_id = 1;
    dt->actual_start_time = now + 2 + 10 * i;
    dt->actual_end_time = 0;
    dt->was_started = true;
    kpis[0]->service_update(dt, _visitor.get());
  }
  ASSERT_TRUE(test_ba->get_in_downtime());
}

TEST_F(BamBA, KpiServiceDtInherited_unset) {
  std::shared_ptr<bam::ba> test_ba{std::make_shared<bam::ba>(
      1, 2, 4, bam::configuration::ba::state_source_ratio_percent)};
  test_ba->set_level_critical(100);
  test_ba->set_level_warning(75);
  test_ba->set_downtime_behaviour(bam::configuration::ba::dt_inherit);

  std::vector<std::shared_ptr<bam::kpi_service>> kpis;

  for (int i = 0; i < 4; i++) {
    auto s = std::make_shared<bam::kpi_service>(i + 1, 1, i + 1, 1);
    s->set_state_hard(bam::kpi_service::state::state_critical);
    s->set_state_soft(s->get_state_hard());
    test_ba->add_impact(s);
    s->add_parent(test_ba);
    kpis.push_back(s);
  }

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));

  std::shared_ptr<neb::service_status> ss(new neb::service_status);
  ss->service_id = 1;

  std::shared_ptr<neb::downtime> dt(std::make_shared<neb::downtime>());

  for (size_t j = 0; j < kpis.size(); j++) {
    ss->last_check = now + 1;
    ss->host_id = j + 1;
    ss->last_hard_state = 2;
    kpis[j]->service_update(ss, _visitor.get());

    dt->host_id = ss->host_id;
    dt->service_id = 1;
    dt->was_started = false;
    dt->actual_start_time = 0;
    dt->actual_end_time = 0;
    kpis[j]->service_update(dt, _visitor.get());
  }

  ASSERT_FALSE(test_ba->get_in_downtime());
}

TEST_F(BamBA, KpiServiceAcknowledgement) {
  std::shared_ptr<bam::ba> test_ba{std::make_shared<bam::ba>(
      1, 2, 4, bam::configuration::ba::state_source_ratio_percent)};
  test_ba->set_level_critical(100);
  test_ba->set_level_warning(75);
  test_ba->set_downtime_behaviour(bam::configuration::ba::dt_inherit);

  std::vector<std::shared_ptr<bam::kpi_service>> kpis;

  for (int i = 0; i < 4; i++) {
    auto s = std::make_shared<bam::kpi_service>(i + 1, 1, i + 1, 1);
    s->set_state_hard(bam::kpi_service::state::state_critical);
    s->set_state_soft(s->get_state_hard());
    test_ba->add_impact(s);
    s->add_parent(test_ba);
    kpis.push_back(s);
  }

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));

  std::shared_ptr<neb::service_status> ss(new neb::service_status);
  ss->service_id = 1;

  std::shared_ptr<neb::acknowledgement> ack(
      std::make_shared<neb::acknowledgement>());

  for (size_t j = 0; j < kpis.size(); j++) {
    ss->last_check = now + 1;
    ss->host_id = j + 1;
    ss->last_hard_state = 2;
    kpis[j]->service_update(ss, _visitor.get());

    ack->poller_id = 1;
    ack->host_id = ss->host_id;
    ack->service_id = 1;
    ack->entry_time = now + 2;
    ack->deletion_time = -1;
    kpis[j]->service_update(ack, _visitor.get());
  }

  auto events = _visitor->queue();
  ASSERT_EQ(events.size(), 5u);

  _visitor->print_events();
}
