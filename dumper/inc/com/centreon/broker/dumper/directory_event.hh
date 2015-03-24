/*
** Copyright 2013 Merethis
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

#ifndef CCB_DUMPER_DIRECTORY_EVENT_HH
#  define CCB_DUMPER_DIRECTORY_EVENT_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace              dumper {
  /**
   *  @class directory_event directory_event.hh "com/centreon/broker/dumper/directory_event.hh"
   *  @brief Event of a watched directory.
   *
   *  This represents an event happening to a watched directory.
   */
  class          directory_event {
  public:
    enum         type {
                 created,
                 modified,
                 deleted,
                 directory_deleted
    };
                 directory_event();
                 directory_event(
                   std::string const& path,
                   type type);
                 directory_event(directory_event const& o);
    directory_event&
                 operator=(directory_event const& o);
                 ~directory_event();

    std::string const&
                 get_path() const;
    type         get_type() const;

  private:
    std::string  _path;
    type         _type;
  };
}

CCB_END()

#endif // !CCB_DUMPER_DIRECTORY_EVENT_HH
