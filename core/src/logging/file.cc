/*
** Copyright 2009-2011 Merethis
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

#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/file.hh"

using namespace com::centreon::broker::logging;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy constructor.
 *
 *  @param[in] f Unused.
 */
file::file(file const& f) : ostream(f) {
  (void)f;
  assert(false);
  abort();
}

/**
 *  Assignment operator.
 *
 *  @param[in] f Unused.
 *
 *  @return This object.
 */
file& file::operator=(file const& f) {
  (void)f;
  assert(false);
  abort();
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
file::file() {}

/**
 *  Constructor.
 *
 *  @param[in] filename Name of the file to open.
 */
file::file(char const* filename) {
  open(filename);
}

/**
 *  Destructor.
 */
file::~file() {}

/**
 *  Open a log file.
 *
 *  @param[in] filename Name of the file to open.
 */
void file::open(char const* filename) {
  // Check filename is not NULL.
  if (!filename)
    throw (exceptions::msg() << "log: trying to open empty file name");

  // Close file if previously opened.
  if (_ofs.is_open())
    _ofs.close();

  // Open file.
  _ofs.open(filename, std::ios_base::out
                      | std::ios_base::app
                      | std::ios_base::ate);
  if (_ofs.fail())
    throw (exceptions::msg() << "log: could not open \""
                             << filename << "\" file");

  // Don't forget to set the stream to write on.
  ostream::operator=(_ofs);

  // Opening message.
  _ofs << "-----------------------\n"
          "    Centreon Broker\n"
          "-----------------------\n"
          "PID " << getpid() << "\n\n";

  return ;
}
