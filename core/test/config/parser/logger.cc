/*
** Copyright 2012 Merethis
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

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;

/**
 *  Check that 'logger's are properly parsed by the configuration
 *  parser.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Return value.
  int retval(EXIT_FAILURE);

  // File name.
  std::string config_file(tmpnam(NULL));

  try {
    // Create configuration file.
    {
      // Open file.
      FILE* file_stream(fopen(config_file.c_str(), "w"));
      if (!file_stream)
        throw (exceptions::msg() << "could not open '"
               << config_file.c_str() << "'");

      // Data.
      std::string data;
      data =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<centreonbroker>\n"
        "  <logger>\n"
        "    <type>file</type>\n"
        "    <name>my_log_file</name>\n"
        "    <config>1</config>\n"
        "    <debug>0</debug>\n"
        "    <error>1</error>\n"
        "    <info>0</info>\n"
        "    <level>2</level>\n"
        "  </logger>\n"
        "  <logger>\n"
        "    <type>standard</type>\n"
        "    <name>stderr</name>\n"
        "    <config>0</config>\n"
        "    <error>1</error>\n"
        "    <debug>0</debug>\n"
        "    <info>1</info>\n"
        "    <level>3</level>\n"
        "  </logger>\n"
        "</centreonbroker>\n";

      // Write data.
      if (fwrite(data.c_str(), data.size(), 1, file_stream) != 1)
        throw (exceptions::msg() << "could not write content of '"
               << config_file.c_str() << "'");

      // Close file.
      fclose(file_stream);
    }

    // Parse.
    config::state s;
    config::parser p;
    p.parse(config_file.c_str(), s);

    // Check against expected result.
    if (s.loggers().size() != 2)
      throw (exceptions::msg() << "only " << s.loggers().size()
             << " loggers, expected 2");

    // Check logger #1.
    QList<config::logger>::iterator it(s.loggers().begin());
    config::logger l1(*(it++));
    if ((l1.type() != config::logger::file)
        || (l1.name() != "my_log_file")
        || !l1.config()
        || l1.debug()
        || !l1.error()
        || l1.info()
        || (l1.level() != 2))
      throw (exceptions::msg() << "logger #1 is not valid");

    // Check logger #2.
    config::logger l2(*it);
    if ((l2.type() != config::logger::standard)
        || (l2.name() != "stderr")
        || l2.config()
        || l2.debug()
        || !l2.error()
        || !l2.info()
        || (l2.level() != 3))
      throw (exceptions::msg() << "logger #2 is not valid");

    // Success !
    retval = EXIT_SUCCESS;
  }
  catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Remove temporary file.
  ::remove(config_file.c_str());

  return (retval);
}
