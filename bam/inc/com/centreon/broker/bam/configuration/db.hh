/*
** Copyright 2009-2013 Merethis
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

#ifndef CCB_CONFIGURATION_DB_HH
#  define CCB_CONFIGURATION_DB_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"
CCB_BEGIN()

namespace              bam {
  namespace            configuration{

    class db{
    
    public:
      db( std::string const& type,
	  unsigned short port,
	  std::string const& host,
	  std::string const& user,
	  std::string const& password,
	  std::string const& name
	  );

      db( db const&);
      db& operator=( db const&);

      std::string const & get_type();
      std::string const & get_host();
      unsigned short get_port();
      std::string const & get_user();
      std::string const & get_password();
      std::string const & get_name();

      void set_type(std::string const &  );
      void set_host(std::string const &  );
      void set_port(unsigned short  );
      void set_user(std::string const &  );
      void set_password(std::string const &  );
      void set_name(std::string const &  );
    
    private:
      std::string _type;
      std::string _host;
      unsigned short _port;
      std::string _user;
      std::string _password;
      std::string _name;
    };
  }
}

CCB_END()

#endif // !CCB_CONFIGURATION_DB_HH
