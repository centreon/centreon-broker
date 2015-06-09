/*
** Copyright 2012,2015 Merethis
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
 *  Check that 'input' and 'output' are properly parsed by the
 *  configuration parser.
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
        "  <input>\n"
        "    <name>CentreonInput</name>\n"
        "    <type>tcp</type>\n"
        "    <port>5668</port>\n"
        "    <protocol>ndo</protocol>\n"
        "    <compression>yes</compression>\n"
        "  </input>\n"
        "  <output>\n"
        "    <name>CentreonDatabase</name>\n"
        "    <type>sql</type>\n"
        "    <db_type>mysql</db_type>\n"
        "    <db_host>localhost</db_host>\n"
        "    <db_port>3306</db_port>\n"
        "    <db_user>centreon</db_user>\n"
        "    <db_password>merethis</db_password>\n"
        "    <db_name>centreon_storage</db_name>\n"
        "    <failover>CentreonRetention</failover>\n"
        "    <secondary_failover>CentreonSecondaryFailover1</secondary_failover>\n"
        "    <secondary_failover>CentreonSecondaryFailover2</secondary_failover>\n"
        "    <buffering_timeout>10</buffering_timeout>\n"
        "    <read_timeout>5</read_timeout>\n"
        "    <retry_interval>300</retry_interval>\n"
        "  </output>\n"
        "  <output>\n"
        "    <name>CentreonRetention</name>\n"
        "    <type>file</type>\n"
        "    <path>retention.dat</path>\n"
        "    <protocol>ndo</protocol>\n"
        "  </output>\n"
          "  <output>\n"
          "    <name>CentreonSecondaryFailover1</name>\n"
          "    <type>file</type>\n"
          "    <path>retention.dat</path>\n"
          "    <protocol>ndo</protocol>\n"
          "  </output>\n"
          "  <output>\n"
          "    <name>CentreonSecondaryFailover2</name>\n"
          "    <type>file</type>\n"
          "    <path>retention.dat</path>\n"
          "    <protocol>ndo</protocol>\n"
          "  </output>\n"
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
    if ((s.loggers().size() != 0)
        || (s.endpoints().size() != 5))
      throw (exceptions::msg() << "invalid parsing: "
             << s.loggers().size() << " loggers, "
             << s.endpoints().size() << " endpoints (expected 0, 5)");

    // Check input #1.
    std::list<config::endpoint>::iterator it(s.endpoints().begin());
    config::endpoint input1(*(it++));
    if ((input1.name != "CentreonInput")
        || (input1.type != "tcp")
        || (input1.params["port"] != "5668")
        || (input1.params["protocol"] != "ndo")
        || (input1.params["compression"] != "yes"))
      throw (exceptions::msg() << "invalid input #1");

    // Check output #1.
    config::endpoint output1(*(it++));
    if ((output1.name != "CentreonDatabase")
        || (output1.type != "sql")
        || (output1.failover != "CentreonRetention")
        || (output1.secondary_failovers.size() != 2)
        || (output1.secondary_failovers.count("CentreonSecondaryFailover1") != 1)
        || (output1.secondary_failovers.count("CentreonSecondaryFailover2") != 1)
        || (output1.buffering_timeout != 10)
        || (output1.read_timeout != 5)
        || (output1.retry_interval != 300)
        || (output1.params["db_type"] != "mysql")
        || (output1.params["db_host"] != "localhost")
        || (output1.params["db_port"] != "3306")
        || (output1.params["db_user"] != "centreon")
        || (output1.params["db_password"] != "merethis")
        || (output1.params["db_name"] != "centreon_storage"))
      throw (exceptions::msg() << "invalid output #1");

    // Check output #2.
    config::endpoint output2(*(it++));
    if ((output2.name != "CentreonRetention")
        || (output2.type != "file")
        || (output2.params["path"] != "retention.dat")
        || (output2.params["protocol"] != "ndo"))
      throw (exceptions::msg() << "invalid output #2");

    // Check output #3.
    config::endpoint output3(*(it++));
    if ((output3.name != "CentreonSecondaryFailover1")
        || (output3.type != "file")
        || (output3.params["path"] != "retention.dat")
        || (output3.params["protocol"] != "ndo"))
      throw (exceptions::msg() << "invalid output #3");

    // Check output #4.
    config::endpoint output4(*it);
    if ((output4.name != "CentreonSecondaryFailover2")
        || (output4.type != "file")
        || (output4.params["path"] != "retention.dat")
        || (output4.params["protocol"] != "ndo"))
      throw (exceptions::msg() << "invalid output #4");

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
