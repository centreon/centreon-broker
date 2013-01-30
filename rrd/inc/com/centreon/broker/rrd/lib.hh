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

#ifndef CCB_RRD_LIB_HH
#  define CCB_RRD_LIB_HH

#  include <QString>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/rrd/backend.hh"

CCB_BEGIN()

namespace            rrd {
  /**
   *  @class lib lib.hh "com/centreon/broker/rrd/lib.hh"
   *  @brief Handle RRD file access through librrd.
   *
   *  Handle creation, deletion, tuning and update of an RRD file with
   *  librrd.
   */
  class              lib : public backend {
  public:
    static int const max_metric_length = 19;
                     lib();
                     lib(lib const& l);
                     ~lib();
    lib&             operator=(lib const& l);
    void             begin();
    void             close();
    void             commit();
    static QString   normalize_metric_name(QString const& metric);
    void             open(
                       QString const& filename,
                       QString const& metric);
    void             open(
                       QString const& filename,
                       QString const& metric,
                       unsigned int length,
                       time_t from,
                       time_t interval,
                       short value_type = 0);
    void             remove(QString const& filename);
    void             update(
                       time_t t,
                       QString const& value);

   private:
    QString          _filename;
    QString          _metric;
  };
}

CCB_END()

#endif // !CCB_RRD_LIB_HH
