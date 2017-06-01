/*
** Copyright 2017 Centreon
**
** This file is part of Centreon Engine.
**
** Centreon Engine is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Engine is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Engine. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <gtest/gtest.h>
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/logging/manager.hh"

class  CentreonBrokerEnvironment : public testing::Environment {
 public:
  void SetUp() {
    com::centreon::broker::logging::manager::load();
    com::centreon::broker::config::applier::state::load();
    return ;
  }

  void TearDown() {
    com::centreon::broker::config::applier::state::unload();
    com::centreon::broker::logging::manager::unload();
    return ;
  }
};

/**
 *  Tester entry point.
 *
 *  @param[in] argc  Argument count.
 *  @param[in] argv  Argument values.
 *
 *  @return 0 on success, any other value on failure.
 */
int main(int argc, char* argv[]) {
  // GTest initialization.
  testing::InitGoogleTest(&argc, argv);

  // Set specific environment.
  testing::AddGlobalTestEnvironment(new CentreonBrokerEnvironment());

  // Run all tests.
  return (RUN_ALL_TESTS());
}
