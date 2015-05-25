/*
** Copyright 2011-2013,2015 Merethis
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

#ifndef CCB_MODULES_HANDLE_HH
#  define CCB_MODULES_HANDLE_HH

#  include <QLibrary>
#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace              modules {
  /**
   *  @class handle handle.hh "com/centreon/broker/modules/handle.hh"
   *  @brief Plugin library handle.
   *
   *  Centreon Broker can load plugins. This class represents such
   *  plugins.
   */
  class                handle {
  public:
                       handle();
                       handle(handle const& other);
                       ~handle();
    handle&            operator=(handle const& other);
    void               close();
    bool               is_open() const;
    void               open(
                         std::string const& filename,
                         void const* arg = NULL);
    void               update(void const* arg = NULL);

    static char const* deinitialization;
    static char const* initialization;
    static char const* updatization;

  private:
    void               _init(void const* arg = NULL);

    QLibrary           _handle;
  };
}

CCB_END()

#endif // !CCB_MODULES_HANDLE_HH
