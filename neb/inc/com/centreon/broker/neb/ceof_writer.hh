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

#ifndef CCB_NEB_CEOF_WRITER_HH
#  define CCB_NEB_CEOF_WRITER_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace   neb {
  /**
   *  @class ceof_writer ceof_writer.hh "com/centreon/broker/neb/ceof_writer.hh"
   *  @brief Centreon Engine Object File writer.
   */
  class          ceof_writer {
  public:
                 ceof_writer();
                 ceof_writer(ceof_writer const& other);
   ceof_writer&  operator=(ceof_writer const& other);
                 ~ceof_writer() throw();

   void          open_object(std::string const& object_type);
   void          add_key(std::string const& key);
   void          add_value(std::string const& value);
   void          close_object();

   std::string const&
                 get_string() const throw();

  private:
    std::string  _str;
  };
}

CCB_END()

#endif // !CCB_NEB_CEOF_WRITER_HH
