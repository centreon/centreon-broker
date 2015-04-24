/*
** Copyright 2013-2015 Merethis
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
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/cbd.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

/**
 *  Check that statistics are generated when the statistics module is
 *  used.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  // Variables that need cleaning.
  std::string cbd_config_path(tmpnam(NULL));
  std::string stats_pipe(tmpnam(NULL));
  cbd daemon;

  try {
    // Write cbd configuration file.
    {
      std::ofstream ofs;
      ofs.open(
            cbd_config_path.c_str(),
            std::ios_base::out | std::ios_base::trunc);
      if (ofs.fail())
        throw (exceptions::msg()
               << "cannot open cbd configuration file '"
               << cbd_config_path.c_str() << "'");
      ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
          << "<centreonbroker>\n"
          << "  <include>" PROJECT_SOURCE_DIR "/test/cfg/broker_modules.xml</include>\n"
          << "  <!--\n"
          << "  <logger>\n"
          << "    <type>file</type>\n"
          << "    <name>cbd.log</name>\n"
          << "    <config>1</config>\n"
          << "    <debug>1</debug>\n"
          << "    <error>1</error>\n"
          << "    <info>1</info>\n"
          << "    <level>3</level>\n"
          << "  </logger>\n"
          << "  -->\n"
          << "  <stats>\n"
          << "    <fifo>" << stats_pipe << "</fifo>\n"
          << "  </stats>\n"
          << "  <output>\n"
          << "    <name>TCPOutput</name>\n"
          << "    <type>tcp</type>\n"
          << "    <host>localhost</host>\n"
          << "    <port>5569</port>\n"
          << "    <protocol>bbdo</protocol>\n"
          << "  </output>\n"
          << "</centreonbroker>\n";
      ofs.close();
    }

    // Run cbd.
    daemon.set_config_file(cbd_config_path);
    daemon.start();
    sleep_for(3);
    daemon.update();

    // Read data from statistics pipe.
    std::string data;
    {
      char buffer[BUFSIZ];
      std::ifstream ifs(stats_pipe.c_str());
      if (ifs.fail() || ifs.eof())
        throw (exceptions::msg() << "cannot open statistics file '"
               << stats_pipe.c_str() << "'");
      while (!ifs.fail() && !ifs.eof()) {
        ifs.read(buffer, sizeof(buffer));
        data.append(buffer, ifs.gcount());
      }
    }

    // Check that some data was read.
    error = data.empty();
  }
  catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  daemon.stop();
  ::remove(cbd_config_path.c_str());
  ::remove(stats_pipe.c_str());

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
