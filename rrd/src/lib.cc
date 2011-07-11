/*
** Copyright 2011 Merethis
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

#include <QFile>
#include <rrd.h>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/rrd/exceptions/open.hh"
#include "com/centreon/broker/rrd/exceptions/update.hh"
#include "com/centreon/broker/rrd/lib.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::rrd;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
lib::lib() {}

/**
 *  Copy constructor.
 *
 *  @param[in] l Object to copy.
 */
lib::lib(lib const& l)
  : backend(l), _filename(l._filename), _metric(l._metric) {}

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
  _filename = l._filename;
  _metric = l._metric;
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
  _metric.clear();
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
 *  Normalize a metric name.
 *
 *  @param[in] metric Metric name.
 *
 *  @return Normalized metric name.
 */
QString lib::normalize_metric_name(QString const& metric) {
  QString normalized(metric);
  normalized.replace('.', '-');
  normalized.replace(':', '-');
  normalized.replace(',', '-');
  normalized.replace('{', '-');
  normalized.replace('}', '-');
  normalized.replace('[', '-');
  normalized.replace(']', '-');
  normalized.replace(' ', '-');
  normalized.replace("/", "_slash");
  normalized.replace("\\", "_bslash");
  return (normalized);
}

/**
 *  Open a RRD file which already exists.
 *
 *  @param[in] filename Path to the RRD file.
 *  @param[in] metric   Metric name.
 */
void lib::open(QString const& filename,
               QString const& metric) {
  // Close previous file.
  this->close();

  // Check that the file exists.
  if (!QFile::exists(filename))
    throw (exceptions::open() << "RRD: file '"
             << filename << "' does not exist");

  // Remember information for further operations.
  _filename = filename;
  _metric = normalize_metric_name(metric);

  return ;
}

/**
 *  Open a RRD file and create it if it does not exists.
 *
 *  @param[in] filename Path to the RRD file.
 *  @param[in] metric   Metric name.
 *  @param[in] length   Number of recording in the RRD file.
 *  @param[in] from     Timestamp of the first record.
 *  @param[in] interval Time interval between each record.
 */
void lib::open(QString const& filename,
               QString const& metric,
               unsigned int length,
               time_t from,
               time_t interval) {
  logging::debug << logging::HIGH << "RRD: opening file '"
    << filename << "'";

  // Close previous file.
  this->close();

  // Remember informations for further operations.
  _filename = filename;
  _metric = normalize_metric_name(metric);

  /* Find step of RRD file if already existing. */
  /* XXX : why is it here ?
  rrd_info_t* rrdinfo(rrd_info_r(_filename));
  time_t interval_offset(0);
  for (rrd_info_t* tmp = rrdinfo; tmp; tmp = tmp->next)
    if (!strcmp(rrdinfo->key, "step"))
      if (interval < static_cast<time_t>(rrdinfo->value.u_cnt))
        interval_offset = rrdinfo->value.u_cnt / interval - 1;
  rrd_info_free(rrdinfo);
  */

  /* Remove previous file. */
  QFile::remove(_filename);

  /* Set parameters. */
  std::ostringstream ds_oss;
  std::ostringstream rra1_oss;
  std::ostringstream rra2_oss;
  ds_oss << "DS:" << _metric.toStdString() << ":GAUGE:" << interval << ":U:U";
  rra1_oss << "RRA:AVERAGE:0.5:1:" << length;
  rra2_oss << "RRA:AVERAGE:0.5:12:" << length;
  std::string ds(ds_oss.str());
  std::string rra1(rra1_oss.str());
  std::string rra2(rra2_oss.str());
  char const* argv[4];
  argv[0] = ds.c_str();
  argv[1] = rra1.c_str();
  argv[2] = rra2.c_str();
  argv[3] = NULL;

  /* Create RRD file. */
  rrd_clear_error();
  if (rrd_create_r(_filename.toStdString().c_str(),
        interval,
        from,
        sizeof(argv) / sizeof(*argv) - 1,
        argv))
    throw (exceptions::open() << "RRD: could not create file '"
             << _filename << "': " << rrd_get_error());
  // XXX : is tuning really needed ?

  return ;
}

/**
 *  Update the RRD file with new value.
 *
 *  @param[in] t     Timestamp of value.
 *  @param[in] value Associated value.
 */
void lib::update(time_t t, QString const& value) {
  logging::debug << logging::HIGH << "RRD: updating file";

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

  // Update RRD file.
  rrd_clear_error();
  if (rrd_update_r(_filename.toStdString().c_str(),
        _metric.toStdString().c_str(),
        sizeof(argv) / sizeof(*argv) - 1,
        argv))
    throw (exceptions::update() << "RRD: failed to update value for " \
             "metric " << _metric << ": " << rrd_get_error());

  return ;
}
