/*
** Copyright 2013,2015 Merethis
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

#ifndef CCB_DUMPER_RELOAD_HH
#  define CCB_DUMPER_RELOAD_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace               dumper {
  /**
   *  @class reload reload.hh "com/centreon/broker/dumper/reload.hh"
   *  @brief Event for an asked reload.
   *
   *  Reload asked.
   */
  class                 reload : public io::data {
  public:
                        reload();
                        reload(reload const& right);
                        ~reload();
    reload&             operator=(reload const& right);
    unsigned int        type() const;

    QString             tag;

  private:
    void                _internal_copy(reload const& right);
  };
}

CCB_END()

#endif // !CCB_DUMPER_RELOAD_HH
