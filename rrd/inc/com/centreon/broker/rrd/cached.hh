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

#ifndef CCB_RRD_CACHED_HH
#  define CCB_RRD_CACHED_HH

#  include <memory>
#  include <QIODevice>
#  include <QString>
#  include <string>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/rrd/backend.hh"
#  include "com/centreon/broker/rrd/lib.hh"

CCB_BEGIN()

namespace   rrd {
  /**
   *  @class cached cached.hh "com/centreon/broker/rrd/cached.hh"
   *  @brief Handle RRD file access through rrdcached.
   *
   *  Handle creation, deletion, tuning and update of an RRD file with
   *  rrdcached. The advantage of rrdcached is to have a batch mode that
   *  allows bulk operations.
   *
   *  @see begin()
   *  @see commit()
   */
  class     cached : public backend {
  public:
            cached(
              std::string const& tmpl_path,
              unsigned int cache_size);
            ~cached();
    void    begin();
    void    clean();
    void    close();
    void    commit();
#  if QT_VERSION >= 0x040400
    void    connect_local(QString const& name);
#  endif // Qt >= 4.4.0
    void    connect_remote(
              QString const& address,
              unsigned short port);
    void    open(std::string const& filename);
    void    open(
              std::string const& filename,
              unsigned int length,
              time_t from,
              unsigned int step,
              short value_type = 0);
    void    remove(std::string const& filename);
    void    update(time_t t, std::string const& value);

  private:
            cached(cached const& c);
    cached& operator=(cached const& c);
    void    _send_to_cached(
              char const* command,
              unsigned int size = 0);

    bool    _batch;
    std::string
            _filename;
    lib     _lib;
    std::auto_ptr<QIODevice>
            _socket;
  };
}

CCB_END()

#endif // !CCB_RRD_CACHED_HH
