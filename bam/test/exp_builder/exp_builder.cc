/*
** Copyright 2016 Centreon
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
#include <list>
#include <string>
#include "com/centreon/broker/bam/bool_value.hh"
#include "com/centreon/broker/bam/exp_builder.hh"
#include "com/centreon/broker/bam/exp_parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;

TEST(BamExpBuilder, Valid1) {
  bam::exp_parser p("OK IS OK");
  bam::hst_svc_mapping mapping;
  bam::exp_builder builder(p.get_postfix(), mapping);
  ASSERT_EQ(builder.get_calls().size(), 0u);
  ASSERT_EQ(builder.get_metrics().size(), 0u);
  ASSERT_EQ(builder.get_services().size(), 0u);
  bam::bool_value::ptr b(builder.get_tree());
  ASSERT_EQ(b->value_soft(), 1);
  ASSERT_EQ(b->value_hard(), 1);
}

TEST(BamExpBuilder, Valid2) {
  bam::exp_parser p("OK IS NOT OK");
  bam::hst_svc_mapping mapping;
  bam::exp_builder builder(p.get_postfix(), mapping);
  ASSERT_EQ(builder.get_calls().size(), 0u);
  ASSERT_EQ(builder.get_metrics().size(), 0u);
  ASSERT_EQ(builder.get_services().size(), 0u);
  bam::bool_value::ptr b(builder.get_tree());
  ASSERT_EQ(b->value_soft(), 0);
  ASSERT_EQ(b->value_hard(), 0);
}

TEST(BamExpBuilder, Valid3) {
  bam::exp_parser p("OK AND CRITICAL");
  bam::hst_svc_mapping mapping;
  bam::exp_builder builder(p.get_postfix(), mapping);
  ASSERT_EQ(builder.get_calls().size(), 0u);
  ASSERT_EQ(builder.get_metrics().size(), 0u);
  ASSERT_EQ(builder.get_services().size(), 0u);
  bam::bool_value::ptr b(builder.get_tree());
  ASSERT_EQ(b->value_soft(), 0);
  ASSERT_EQ(b->value_hard(), 0);
}

TEST(BamExpBuilder, Valid4) {
  bam::exp_parser p("OK OR CRITICAL");
  bam::hst_svc_mapping mapping;
  bam::exp_builder builder(p.get_postfix(), mapping);
  ASSERT_EQ(builder.get_calls().size(), 0u);
  ASSERT_EQ(builder.get_metrics().size(), 0u);
  ASSERT_EQ(builder.get_services().size(), 0u);
  bam::bool_value::ptr b(builder.get_tree());
  ASSERT_EQ(b->value_soft(), 1);
  ASSERT_EQ(b->value_hard(), 1);
}

TEST(BamExpBuilder, Valid5) {
  bam::exp_parser p("OK XOR CRITICAL");
  bam::hst_svc_mapping mapping;
  bam::exp_builder builder(p.get_postfix(), mapping);
  ASSERT_EQ(builder.get_calls().size(), 0u);
  ASSERT_EQ(builder.get_metrics().size(), 0u);
  ASSERT_EQ(builder.get_services().size(), 0u);
  bam::bool_value::ptr b(builder.get_tree());
  ASSERT_EQ(b->value_soft(), 1);
  ASSERT_EQ(b->value_hard(), 1);
}
