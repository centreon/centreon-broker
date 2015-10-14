/*
** Copyright 2015 Centreon
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

#include <cstdlib>
#include <iostream>
#include "test/db.hh"
#include "test/centengine.hh"
#include "test/centengine_config.hh"
#include "test/file.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define TEST_NAME "rtmonitoring_instances_v2"
#define DB_NAME "broker_" TEST_NAME

/**
 *  Check that the instances table work properly.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  try {
    // Database.
    char const* tables[] = { "instances", NULL };
    test::db db(DB_NAME, tables);

    // Monitoring engine.
    test::file cbmod_cfg;
    cbmod_cfg.set_template(
      PROJECT_SOURCE_DIR "/test/cfg/sql_v2.xml.in");
    cbmod_cfg.set("TEST_NAME", TEST_NAME);
    cbmod_cfg.set("DB_NAME", DB_NAME);
    test::centengine_config engine_config;
    engine_config.generate_hosts(3);
    engine_config.generate_services(5);
    engine_config.set_cbmod_cfg_file(cbmod_cfg.generate());
    test::centengine engine(engine_config);
    engine.start();
    test::sleep_for(2);

    // #1 Check entry insertion.
    // XXX

    // Success.
    error = false;
    db.set_remove_db_on_close(true);
  }
  catch (std::exception const& e) {
    std::cerr << e.what() << "\n";
  }
  catch (...) {
    std::cerr << "unknown exception\n";
  }

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
