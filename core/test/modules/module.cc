/*
** Copyright 2009-2015 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
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
