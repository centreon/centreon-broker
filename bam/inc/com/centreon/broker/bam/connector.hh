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

#ifndef CCB_BAM_CONNECTOR_HH
#  define CCB_BAM_CONNECTOR_HH

#  include <QString>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          bam {
  /**
   *  @class connector connector.hh "com/centreon/broker/bam/connector.hh"
   *  @brief Connect to a database.
   *
   *  Send perfdata in a Centreon bam database.
   */
  class            connector : public io::endpoint {
  public:
    enum stream_type {
      bam_type = 1,
      bam_bi_type
    };

                   connector();
                   connector(connector const& c);
                   ~connector();
    connector&     operator=(connector const& c);
    io::endpoint*  clone() const;
    void           close();
    void           connect_to(
                     stream_type type,
                     std::string const& db_type,
                     std::string const& db_host,
                     unsigned short db_port,
                     std::string const& db_user,
                     std::string const& db_password,
                     std::string const& db_name,
                     std::string const& ext_cmd_file,
                     unsigned int queries_per_transaction,
                     bool check_replication);
    misc::shared_ptr<io::stream>
                   open();
    misc::shared_ptr<io::stream>
                   open(QString const& id);

   private:
    void           _internal_copy(connector const& c);

    bool           _check_replication;
    std::string    _db_host;
    std::string    _db_name;
    std::string    _db_password;
    unsigned short _db_port;
    std::string    _db_user;
    std::string    _db_type;
    std::string    _ext_cmd_file;
    unsigned int   _queries_per_transaction;
    stream_type    _type;
  };
}

CCB_END()

#endif // !CCB_BAM_CONNECTOR_HH
