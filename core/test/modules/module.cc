/*
 * Copyright 2011 - 2019 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */

#include <iostream>
#include <string>
#include <QCoreApplication>
#include <QString>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/modules/loader.hh"

using namespace com::centreon::broker;

/**
 *  Check that a module doesn't load with a specific error message.
 *
 *  @param[in] module              The module that doesn't load.
 *  @param[in] expected_error_msg  The expected error message.
 *
 *  @return                        True if the module didn't load.
 */
bool check_for(
       std::string const& module,
       std::string const& expected_error_msg) {
  try {
    modules::loader l;

    l.load_file(module);
    return (false);
  } catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
    return (QString::fromStdString(e.what())
              .contains(QString::fromStdString(expected_error_msg)));
  }
}

/**
 *  Verify that the module version checks work.
 */
int main(int argc, char *argv[]) {
  // Qt core object.
  QCoreApplication app(argc, argv);

  // Initialization.
  config::applier::init();

  int retval =
    check_for(
      "./null_module",
      "Cannot resolve symbol \"broker_module_version\" in")
    && check_for(
         "./bad_version_module",
         "version mismatch in") ? 0 : -1;

  return (retval);
}
