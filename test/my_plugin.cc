/*
** Copyright 2013 Centreon
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

/**
 *  This is a fake plugin that return the requested return code and
 *  output. By default return 0 and print nothing.
 *
 *  @return The configured return value, 0 by default.
 */
int main(int argc, char* argv[]) {
  int retval(0);
  char const* output("");
  if (argc > 1)
    retval = strtol(argv[1], NULL, 0);
  if (argc > 2)
    output = argv[2];
  std::cout << output;
  return (retval);
}
