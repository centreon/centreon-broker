/*
** Copyright 2011-2013 Merethis
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

#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <QFile>
#include <rrd.h>
#include <sstream>
#include <unistd.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/rrd/exceptions/open.hh"
#include "com/centreon/broker/rrd/exceptions/update.hh"
#include "com/centreon/broker/rrd/lib.hh"
#include "com/centreon/broker/storage/perfdata.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::rrd;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] tmpl_path  The template path.
 *  @param[in] cache_size The maximum number of cache element.
 */
lib::lib(std::string const& tmpl_path, unsigned int cache_size)
  : _creator(tmpl_path, cache_size) {}

/**
 *  Copy constructor.
 *
 *  @param[in] l Object to copy.
 */
lib::lib(lib const& l)
  : backend(l), _creator(l._creator), _filename(l._filename) {}

/**
 *  Destructor.
 */
lib::~lib() {}

/**
 *  Assignment operator.
 *
 *  @param[in] l Object to copy.
 *
 *  @return This object.
 */
lib& lib::operator=(lib const& l) {
  backend::operator=(l);
  _creator = l._creator;
  _filename = l._filename;
  return (*this);
}

/**
 *  @brief Initiates the bulk load of multiple commands.
 *
 *  With the librrd backend, this method does nothing.
 */
void lib::begin() {
  return ;
}

/**
 *  Close the RRD file.
 */
void lib::close() {
  _filename.clear();
  return ;
}

/**
 *  @brief Commit transaction started with begin().
 *
 *  With the librrd backend, the method does nothing.
 */
void lib::commit() {
  return ;
}

/**
 *  Open a RRD file which already exists.
 *
 *  @param[in] filename Path to the RRD file.
 */
void lib::open(QString const& filename) {
  // Close previous file.
  this->close();

  // Check that the file exists.
  if (!QFile::exists(filename))
    throw (exceptions::open() << "RRD: file '"
             << filename << "' does not exist");

  // Remember information for further operations.
  _filename = filename.toStdString();

  return ;
}

/**
 *  Open a RRD file and create it if it does not exists.
 *
 *  @param[in] filename   Path to the RRD file.
 *  @param[in] length     Number of recording in the RRD file.
 *  @param[in] from       Timestamp of the first record.
 *  @param[in] step       Time interval between each record.
 *  @param[in] value_type Type of the metric.
 */
void lib::open(
            QString const& filename,
            unsigned int length,
            time_t from,
            unsigned int step,
            short value_type) {
  // Close previous file.
  this->close();

  // Remember informations for further operations.
  _filename = filename.toStdString();

  _creator.create(filename.toStdString(), length, from, step, value_type);

  return ;
}

/**
 *  Remove the RRD file.
 *
 *  @param[in] filename Path to the RRD file.
 */
void lib::remove(QString const& filename) {
  if (::remove(filename.toStdString().c_str())) {
    char const* msg(strerror(errno));
    logging::error(logging::high) << "RRD: could not remove file '"
      << qPrintable(filename) << "': " << msg;
  }
  return ;
}

/**
 *  Update the RRD file with new value.
 *
 *  @param[in] t     Timestamp of value.
 *  @param[in] value Associated value.
 */
void lib::update(time_t t, QString const& value) {
  // Build argument string.
  std::string arg;
  {
    std::ostringstream oss;
    oss << t << ":" << value.toStdString();
    arg = oss.str();
  }

  // Set argument table.
  char const* argv[2];
  argv[0] = arg.c_str();
  argv[1] = NULL;

  // Debug message.
  logging::debug(logging::high) << "RRD: updating file '"
    << _filename << "' (" << argv[0] << ")";

  // Update RRD file.
  rrd_clear_error();
  if (rrd_update_r(
        _filename.c_str(),
        NULL,
        sizeof(argv) / sizeof(*argv) - 1,
        argv)) {
    char const* msg(rrd_get_error());
    if (!strstr(msg, "illegal attempt to update using time"))
      throw (exceptions::update()
             << "RRD: failed to update value: " << msg);
    else
      logging::error(logging::low)
        << "RRD: ignored update error: " << msg;
  }

  return ;
}
