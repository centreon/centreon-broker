/*
** Copyright 2014 Centreon
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

#include <fstream>
#include <iostream>
#include <string>

// This utility executable is used to write into files in a portable way.
// Usage: ./executable "what_to_write" filename
int main(int argc, char** argv) {
  if (argc != 3) {
    std::cout << "usage: ./executable \"what_to_write\" filename" << std::endl;
    return (-1);
  }

  std::string content = argv[1];
  std::string filename = argv[2];

  std::ofstream ofs;
  ofs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    ofs.open(filename.c_str());
    ofs << content;
  } catch (std::exception e) {
    std::cerr << "could not write file: " << e.what() << std::endl;
    return (-1);
  }

  return (0);
}
