/*
** Copyright 2014 Merethis
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

#include <fstream>
#include <iostream>
#include <string>

// This utility executable is used to write into files in a portable way.
// Usage: ./executable "what_to_write" filename
int main(int argc, char** argv) {
  if (argc != 3)
  {
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
  }
  catch (std::exception e) {
    std::cerr << "could not write file: " << e.what() << std::endl;
    return (-1);
  }

  return (0);
}
