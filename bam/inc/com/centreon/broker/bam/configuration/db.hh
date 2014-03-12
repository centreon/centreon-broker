/*
** Copyright 2009-2014 Merethis
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

#ifndef CCB_BAM_CONFIGURATION_DB_HH
#  define CCB_BAM_CONFIGURATION_DB_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                 bam {
  namespace               configuration{
    /**
     *  @class db db.hh "com/centreon/broker/bam/configuration/db.hh"
     *  @brief Database configuration.
     *
     *  Hold the database information.
     */
    class                 db{
    public:
                          db(
                            std::string const& type,
                            unsigned short port,
                            std::string const& host,
                            std::string const& user,
                            std::string const& password,
                            std::string const& name);
                          db(db const& other);
                          ~db();
      db&                 operator=(db const& other);

      std::string const & get_type() const;
      std::string const & get_host() const;
      unsigned short      get_port() const;
      std::string const & get_user() const;
      std::string const & get_password() const;
      std::string const & get_name() const;

      void                set_type(std::string const& type);
      void                set_host(std::string const& host);
      void                set_port(unsigned short port);
      void                set_user(std::string const& user);
      void                set_password(std::string const& password);
      void                set_name(std::string const& name);

    private:
      std::string         _type;
      std::string         _host;
      unsigned short      _port;
      std::string         _user;
      std::string         _password;
      std::string         _name;
    };
  }
}

CCB_END()

#endif // !CCB_BAM_CONFIGURATION_DB_HH
