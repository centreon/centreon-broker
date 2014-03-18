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

#include <cmath>
#include <cstdlib>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QThread>
#include <QVariant>
#include <QMutexLocker>
#include <sstream>
#include "com/centreon/broker/misc/global_lock.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/service.hh"
#include "com/centreon/broker/neb/service_status.hh"
//#include "com/centreon/broker/bam/exceptions/perfdata.hh"
//#include "com/centreon/broker/bam/metric.hh"
//#include "com/centreon/broker/bam/parser.hh"
//#include "com/centreon/broker/bam/perfdata.hh"
//#include "com/centreon/broker/bam/remove_graph.hh"
//#include "com/centreon/broker/bam/status.hh"
#include "com/centreon/broker/bam/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;
using namespace com::centreon::broker::bam;

#define BAM_NAME "_Module_"
#define EPSILON 0.0001

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

/**
 *  Check that the floating point value is a NaN, in which case return a
 *  NULL QVariant.
 *
 *  @param[in] f Floating point value.
 *
 *  @return NULL QVariant if f is a NaN, f casted as QVariant otherwise.
 */
static inline QVariant check_double(double f) {
  return (isnan(f) ? QVariant(QVariant::Double) : QVariant(f));
}

/**
 *  Check that two double are equal.
 *
 *  @param[in] a First value.
 *  @param[in] b Second value.
 *
 *  @return true if a and b are equal.
 */
static inline bool double_equal(double a, double b) {
  return ((isnan(a) && isnan(b))
          || (isinf(a)
              && isinf(b)
              && (std::signbit(a) == std::signbit(b)))
          || (std::isfinite(a)
              && std::isfinite(b)
              && !(fabs((a) - (b)) > (0.01 * fabs(a)))));
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] bam_type            Bam DB type.
 *  @param[in] bam_host            Bam DB host.
 *  @param[in] bam_port            Bam DB port.
 *  @param[in] bam_user            Bam DB user.
 *  @param[in] bam_password        Bam DB password.
 *  @param[in] bam_db              Bam DB name.
 *  @param[in] queries_per_transaction Queries per transaction.
 *  @param[in] rrd_len                 RRD length.
 *  @param[in] interval_length         Interval length.
 *  @param[in] rebuild_check_interval  How often the stream must check
 *                                     for graph rebuild.
 *  @param[in] store_in_db             Should we insert data in
 *                                     data_bin ?
 *  @param[in] check_replication       true to check replication status.
 *  @param[in] insert_in_index_data    Create entries in index_data or
 *                                     not.
 */
stream::stream(
          QString const& bam_type,
          QString const& bam_host,
          unsigned short bam_port,
          QString const& bam_user,
          QString const& bam_password,
          QString const& bam_db,
          unsigned int queries_per_transaction,
          unsigned int rrd_len,
          time_t interval_length,
          unsigned int rebuild_check_interval,
          bool store_in_db,
          bool check_replication,
          bool insert_in_index_data)


{

}

/**
 *  Copy constructor.
 *
 *  @param[in] s Object to copy.
 */
stream::stream(stream const& s) : multiplexing::hooker(s) {

}

/**
 *  Destructor.
 */
stream::~stream() {
}

/**
 *  Enable or disable output event processing.
 *
 *  @param[in] in  Unused.
 *  @param[in] out Set to true to enable output event processing.
 */
void stream::process(bool in, bool out) {
}

/**
 *  Read from the datbase.
 *
 *  @param[out] d Cleared.
 */
void stream::read(misc::shared_ptr<io::data>& d) {

}

/**
 *  Multiplexing starts.
 */
void stream::starting() {

}

/**
 *  Get endpoint statistics.
 *
 *  @param[out] tree Output tree.
 */
void stream::statistics(io::properties& tree) const {

}

/**
 *  Multiplexing stopped.
 */
void stream::stopping() {

}

/**
 *  Rebuild index and metrics cache.
 */
void stream::update() {


}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
unsigned int stream::write(misc::shared_ptr<io::data> const& data) {

}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Check for deleted index.
 */
void stream::_check_deleted_index() {

}

/**
 *  Clear QtSql objects.
 */
void stream::_clear_qsql() {

}

/**
 *  Delete specified metrics.
 *
 *  @param[in] metrics_to_delete Metrics to delete.
 */
void stream::_delete_metrics(
                    std::list<unsigned long long> const& metrics_to_delete){

}

/**
 *  @brief Find index ID.
 *
 *  Look through the index cache for the specified index. If it cannot
 *  be found, insert an entry in the database.
 *
 *  @param[in]  host_id      Host ID associated to the index.
 *  @param[in]  service_id   Service ID associated to the index.
 *  @param[in]  host_name    Host name associated to the index.
 *  @param[in]  service_desc Service description associated to the index.
 *  @param[out] rrd_len      Index RRD length.
 *  @param[out] locked       Locked flag.
 *
 *  @return Index ID matching host and service ID.
 */
unsigned int stream::_find_index_id(
                       unsigned int host_id,
                       unsigned int service_id,
                       QString const& host_name,
                       QString const& service_desc,
                       unsigned int* rrd_len,
                       bool* locked) {

}

/**
 *  @brief Find metric ID.
 *
 *  Look through the metric cache for the specified metric. If it cannot
 *  be found, insert an entry in the database.
 *
 *  @param[in]     index_id    Index ID of the metric.
 *  @param[in]     metric_name Name of the metric.
 *  @param[in]     unit_name   Metric unit.
 *  @param[in]     warn        High warning threshold.
 *  @param[in]     warn_low    Low warning threshold.
 *  @param[in]     warn_mode   Warning range mode.
 *  @param[in]     crit        High critical threshold.
 *  @param[in]     crit_low    Low critical threshold.
 *  @param[in]     crit_mode   Critical range mode.
 *  @param[in]     min         Minimal metric value.
 *  @param[in]     max         Maximal metric value.
 *  @param[in]     value       Most recent value.
 *  @param[in,out] type        If not null, set to the metric type.
 *  @param[in,out] locked      Whether or not the metric is locked.
 *
 *  @return Metric ID requested, 0 if it could not be found not
 *          inserted.
 */
unsigned int stream::_find_metric_id(
                       unsigned int index_id,
                       QString metric_name,
                       QString const& unit_name,
                       double warn,
                       double warn_low,
                       bool warn_mode,
                       double crit,
                       double crit_low,
                       bool crit_mode,
                       double min,
                       double max,
                       double value,
                       unsigned int* type,
                       bool* locked) {

}

/**
 *  Insert performance data entries in the data_bin table.
 */
void stream::_insert_perfdatas() {

}

/**
 *  Prepare queries.
 */
void stream::_prepare() {

}

/**
 *  Rebuild cache.
 */
void stream::_rebuild_cache() {

}

/**
 *  Update status of endpoint.
 *
 *  @param[in] status New status.
 */
void stream::_update_status(std::string const& status) {

}
