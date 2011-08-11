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

#ifndef CCB_STORAGE_STREAM_HH_
# define CCB_STORAGE_STREAM_HH_

# include <map>
# include <QScopedPointer>
# include <QSharedPointer>
# include <QSqlDatabase>
# include <QString>
# include <utility>
# include "com/centreon/broker/io/stream.hh"

namespace              com {
  namespace            centreon {
    namespace          broker {
      namespace        storage {
        /**
         *  @class stream stream.hh "com/centreon/broker/storage/stream.hh"
         *  @brief Storage stream.
         *
         *  Handle perfdata and insert proper informations in index_data
         *  and metrics table of a centstorage DB.
         */
        class          stream : public io::stream {
         private:
          std::map<std::pair<unsigned int, unsigned int>, unsigned int>
                       _index_cache;
          QScopedPointer<QSqlQuery>
                       _insert_data_bin;
          time_t       _interval_length;
          std::map<std::pair<unsigned int, QString>, unsigned int>
                       _metric_cache;
          QString      _metrics_path;
          unsigned int _rrd_len;
          bool         _store_in_db;
          QScopedPointer<QSqlQuery>
                       _update_metrics;
          QScopedPointer<QSqlDatabase>
                       _storage_db;
          void         _clear_qsql();
          unsigned int _find_index_id(unsigned int host_id,
                         unsigned int service_id);
          unsigned int _find_metric_id(unsigned int index_id,
                         QString const& metric_name);
          void         _prepare();
          stream&      operator=(stream const& s);

         public:
                       stream(QString const& storage_type,
                         QString const& storage_host,
                         unsigned short storage_port,
                         QString const& storage_user,
                         QString const& storage_password,
                         QString const& storage_db,
                         unsigned int rrd_len,
                         time_t interval_length,
                         bool store_in_db = true);
                       stream(stream const& s);
                       ~stream();
          QSharedPointer<io::data>
                       read();
          void         write(QSharedPointer<io::data> d);
        };
      }
    }
  }
}

#endif /* !CCB_STORAGE_STREAM_HH_ */
