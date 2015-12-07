/*
** Copyright 2012,2015 Centreon
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

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/misc/misc.hh"

using namespace com::centreon::broker;

/**
 *  Check that the 'include' keyword is properly parsed by the
 *  configuration parser.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Return value.
  int retval(EXIT_FAILURE);

  // File names.
  std::string included(misc::temp_path());
  std::string including(misc::temp_path());

  try {
    // Create included file.
    {
      // Open file.
      FILE* included_stream(fopen(included.c_str(), "w"));
      if (!included_stream)
        throw (exceptions::msg() << "could not open '"
               << included.c_str() << "'");

      // Create data.
      std::string data;
      data =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<centreonbroker>\n"
        "  <event_queue_max_size>42</event_queue_max_size>\n"
        "  <flush_logs>0</flush_logs>\n"
        "  <log_thread_id>1</log_thread_id>\n"
        "</centreonbroker>\n";

      // Write data.
      if (fwrite(data.c_str(), data.size(), 1, included_stream) != 1)
        throw (exceptions::msg() << "could not write content of '"
               << included.c_str() << "'");

      // Close file.
      fclose(included_stream);
    }

    // Create file including.
    {
      // Open file.
      FILE* including_stream(fopen(including.c_str(), "w"));
      if (!including_stream)
        throw (exceptions::msg() << "could not open '"
               << including.c_str() << "'");

      // Create data.
      std::string data;
      data =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<centreonbroker>\n"
        "  <event_queue_max_size>24</event_queue_max_size>\n"
        "  <flush_logs>1</flush_logs>\n"
        "  <log_thread_id>0</log_thread_id>\n"
        "  <include>";
      data.append(included);
      data.append(
        "</include>\n"
        "</centreonbroker>\n");

      // Write data.
      if (fwrite(data.c_str(), data.size(), 1, including_stream) != 1)
        throw (exceptions::msg() << "could not write content of '"
               << including.c_str() << "'");

      // Close file.
      fclose(including_stream);
    }

    // Parse.
    config::state s;
    config::parser p;
    p.parse(including.c_str(), s);

    // Check against expected result.
    if ((s.flush_logs() != 0)
        || (s.log_thread_id() != 1)
        || (s.event_queue_max_size() != 42))
      throw (exceptions::msg() << "invalid parsing");

    // Success !
    retval = EXIT_SUCCESS;
  }
  catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Remove temporary files.
  ::remove(included.c_str());
  ::remove(including.c_str());

  return (retval);
}
