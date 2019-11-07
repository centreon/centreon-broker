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

#include <gtest/gtest.h>
#include <cstdlib>
#include <memory>
#include <stack>
#include <vector>
#include "com/centreon/broker/bam/ba.hh"
#include "com/centreon/broker/bam/configuration/applier/state.hh"
#include "com/centreon/broker/bam/kpi_service.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/neb/service_status.hh"

using namespace com::centreon::broker;

class BamBA : public ::testing::Test {
 public:
  void SetUp() override {
    // Initialization.
    config::applier::init();

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

/**
 * Check that KPI change at BA recompute does not mess with the BA
 * value.
 */
TEST_F(BamBA, Recompute) {
  // Build BAM objects.
  std::shared_ptr<bam::ba> test_ba(new bam::ba);

  std::shared_ptr<bam::kpi_service> kpi(new bam::kpi_service);

  kpi->set_host_id(1);
  kpi->set_service_id(1);
  kpi->set_impact_critical(100.0);
  kpi->set_state_hard(bam::kpi_service::state::state_ok);
  kpi->set_state_soft(kpi->get_state_hard());
  test_ba->add_impact(kpi);
  kpi->add_parent(test_ba);

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));
  for (int i(0); i < 100 + 2; ++i) {
    std::shared_ptr<neb::service_status> ss(new neb::service_status);
    ss->host_id = 1;
    ss->service_id = 1;
    ss->last_check = now + i;
    ss->last_hard_state = ((i & 1) ? 0 : 2);
    ss->current_state = ss->last_hard_state;
    kpi->service_update(ss);
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
TEST_F(BamBA, ImpactState) {
  // Build BAM objects.
  std::shared_ptr<bam::ba> test_ba(new bam::ba);

  std::vector<std::shared_ptr<bam::kpi_service> > kpis;
  std::stack<short> results;

  results.push(2);
  results.push(1);
  results.push(1);
  results.push(1);
  results.push(0);
  results.push(0);

  std::shared_ptr<bam::kpi_service> s1{new bam::kpi_service};
  kpis.push_back(s1);
  std::shared_ptr<bam::kpi_service> s2{new bam::kpi_service};
  kpis.push_back(s2);
  std::shared_ptr<bam::kpi_service> s3{new bam::kpi_service};
  kpis.push_back(s3);

  for (int i = 0; i < kpis.size(); i++) {
    kpis[i]->set_host_id(i + 1);
    kpis[i]->set_service_id(1);
    kpis[i]->set_impact_warning(10);
    kpis[i]->set_impact_critical(20);
    kpis[i]->set_state_hard(bam::kpi_service::state::state_ok);
    kpis[i]->set_state_soft(kpis[i]->get_state_hard());
    test_ba->add_impact(kpis[i]);
    kpis[i]->add_parent(test_ba);
  }

  test_ba->set_level_warning(70.0);
  test_ba->set_level_critical(40.0);

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));

  std::shared_ptr<neb::service_status> ss(new neb::service_status);
  ss->service_id = 1;

  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < kpis.size(); j++) {
      ss->last_check = now + 1;
      ss->host_id = j + 1;
      ss->last_hard_state = i + 1;
      ss->current_state = ss->last_hard_state;
      kpis[j]->service_update(ss);

      short val = results.top();
      ASSERT_EQ(test_ba->get_state_soft(), val);
      ASSERT_EQ(test_ba->get_state_hard(), val);
      results.pop();
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
TEST_F(BamBA, BestState) {
  // Build BAM objects.
  std::shared_ptr<bam::ba> test_ba(new bam::ba);
  test_ba->set_state_source(bam::configuration::ba::state_source_best);

  std::vector<std::shared_ptr<bam::kpi_service> > kpis;
  std::stack<short> results;

  results.push(2);
  results.push(1);
  results.push(1);
  results.push(1);
  results.push(0);
  results.push(0);

  std::shared_ptr<bam::kpi_service> s1{new bam::kpi_service};
  kpis.push_back(s1);
  std::shared_ptr<bam::kpi_service> s2{new bam::kpi_service};
  kpis.push_back(s2);
  std::shared_ptr<bam::kpi_service> s3{new bam::kpi_service};
  kpis.push_back(s3);

  for (int i = 0; i < kpis.size(); i++) {
    kpis[i]->set_host_id(i + 1);
    kpis[i]->set_service_id(1);
    kpis[i]->set_state_hard(bam::kpi_service::state::state_ok);
    kpis[i]->set_state_soft(kpis[i]->get_state_hard());
    test_ba->add_impact(kpis[i]);
    kpis[i]->add_parent(test_ba);
  }

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));

  std::shared_ptr<neb::service_status> ss(new neb::service_status);
  ss->service_id = 1;

  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < kpis.size(); j++) {
      ss->last_check = now + 1;
      ss->host_id = j + 1;
      ss->last_hard_state = i + 1;
      ss->current_state = ss->last_hard_state;
      kpis[j]->service_update(ss);

      short val = results.top();
      ASSERT_EQ(test_ba->get_state_soft(), val);
      ASSERT_EQ(test_ba->get_state_hard(), val);
      results.pop();
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
TEST_F(BamBA, WorstState) {
  // Build BAM objects.
  std::shared_ptr<bam::ba> test_ba(new bam::ba);
  test_ba->set_state_source(bam::configuration::ba::state_source_worst);

  std::vector<std::shared_ptr<bam::kpi_service> > kpis;
  std::stack<short> results;

  results.push(2);
  results.push(2);
  results.push(2);
  results.push(1);
  results.push(1);
  results.push(1);

  std::shared_ptr<bam::kpi_service> s1{new bam::kpi_service};
  kpis.push_back(s1);
  std::shared_ptr<bam::kpi_service> s2{new bam::kpi_service};
  kpis.push_back(s2);
  std::shared_ptr<bam::kpi_service> s3{new bam::kpi_service};
  kpis.push_back(s3);

  for (int i = 0; i < kpis.size(); i++) {
    kpis[i]->set_host_id(i + 1);
    kpis[i]->set_service_id(1);
    kpis[i]->set_state_hard(bam::kpi_service::state::state_ok);
    kpis[i]->set_state_soft(kpis[i]->get_state_hard());
    test_ba->add_impact(kpis[i]);
    kpis[i]->add_parent(test_ba);
  }

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));

  std::shared_ptr<neb::service_status> ss(new neb::service_status);
  ss->service_id = 1;

  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < kpis.size(); j++) {
      ss->last_check = now + 1;
      ss->host_id = j + 1;
      ss->last_hard_state = i + 1;
      ss->current_state = ss->last_hard_state;
      kpis[j]->service_update(ss);

      short val = results.top();
      ASSERT_EQ(test_ba->get_state_soft(), val);
      ASSERT_EQ(test_ba->get_state_hard(), val);
      results.pop();
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
TEST_F(BamBA, RatioNum) {
  // Build BAM objects.
  std::shared_ptr<bam::ba> test_ba(new bam::ba);
  test_ba->set_state_source(bam::configuration::ba::state_source_ratio_number);
  test_ba->set_level_critical(4);
  test_ba->set_level_warning(2);

  std::vector<std::shared_ptr<bam::kpi_service> > kpis;
  std::stack<short> results;

  results.push(2);
  results.push(1);
  results.push(1);
  results.push(0);

  std::shared_ptr<bam::kpi_service> s1{new bam::kpi_service};
  kpis.push_back(s1);
  std::shared_ptr<bam::kpi_service> s2{new bam::kpi_service};
  kpis.push_back(s2);
  std::shared_ptr<bam::kpi_service> s3{new bam::kpi_service};
  kpis.push_back(s3);
  std::shared_ptr<bam::kpi_service> s4{new bam::kpi_service};
  kpis.push_back(s4);

  for (int i = 0; i < kpis.size(); i++) {
    kpis[i]->set_host_id(i + 1);
    kpis[i]->set_service_id(1);
    kpis[i]->set_state_hard(bam::kpi_service::state::state_ok);
    kpis[i]->set_state_soft(kpis[i]->get_state_hard());
    test_ba->add_impact(kpis[i]);
    kpis[i]->add_parent(test_ba);
  }

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));

  std::shared_ptr<neb::service_status> ss(new neb::service_status);
  ss->service_id = 1;

  for (int j = 0; j < kpis.size(); j++) {
    ss->last_check = now + 1;
    ss->host_id = j + 1;
    ss->last_hard_state = 2;
    ss->current_state = ss->last_hard_state;
    kpis[j]->service_update(ss);

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
TEST_F(BamBA, RatioPercent) {
  // Build BAM objects.
  std::shared_ptr<bam::ba> test_ba(new bam::ba);
  test_ba->set_state_source(bam::configuration::ba::state_source_ratio_percent);
  test_ba->set_level_critical(100);
  test_ba->set_level_warning(75);

  std::vector<std::shared_ptr<bam::kpi_service> > kpis;
  std::stack<short> results;

  results.push(2);
  results.push(1);
  results.push(0);
  results.push(0);

  std::shared_ptr<bam::kpi_service> s1{new bam::kpi_service};
  kpis.push_back(s1);
  std::shared_ptr<bam::kpi_service> s2{new bam::kpi_service};
  kpis.push_back(s2);
  std::shared_ptr<bam::kpi_service> s3{new bam::kpi_service};
  kpis.push_back(s3);
  std::shared_ptr<bam::kpi_service> s4{new bam::kpi_service};
  kpis.push_back(s4);

  for (int i = 0; i < kpis.size(); i++) {
    kpis[i]->set_host_id(i + 1);
    kpis[i]->set_service_id(1);
    kpis[i]->set_state_hard(bam::kpi_service::state::state_ok);
    kpis[i]->set_state_soft(kpis[i]->get_state_hard());
    test_ba->add_impact(kpis[i]);
    kpis[i]->add_parent(test_ba);
  }

  // Change KPI state as much time as needed to trigger a
  // recomputation. Note that the loop must terminate on a odd number
  // for the test to be correct.
  time_t now(time(nullptr));

  std::shared_ptr<neb::service_status> ss(new neb::service_status);
  ss->service_id = 1;

  for (int j = 0; j < kpis.size(); j++) {
    ss->last_check = now + 1;
    ss->host_id = j + 1;
    ss->last_hard_state = 2;
    ss->current_state = ss->last_hard_state;
    kpis[j]->service_update(ss);

    short val = results.top();
    ASSERT_EQ(test_ba->get_state_soft(), val);
    ASSERT_EQ(test_ba->get_state_hard(), val);
    results.pop();
  }
}
