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

#ifndef CCB_NEB_DOWNTIME_SERIALIZABLE_HH
#  define CCB_NEB_DOWNTIME_SERIALIZABLE_HH

#  include <string>
#  include <istream>
#  include <ostream>
#  include <QString>
#  include "com/centreon/broker/neb/downtime.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/neb/ceof_serializable.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace   neb {
  /**
   *  @class downtime_serializable downtime_serializable.hh "com/centreon/broker/neb/downtime_serializable.hh"
   *  @brief Represent a serializable Centreon Engine Object File downtime.
   */
  class          downtime_serializable
                   : public ceof_serializable<downtime_serializable> {
  public:
                 downtime_serializable();
                 downtime_serializable(
                 downtime_serializable const& other);
    downtime_serializable&
                 operator=(downtime_serializable const& other);
    virtual      ~downtime_serializable();

    template <typename U, U (downtime::* member)>
    std::string  get_downtime_member() const;

    template <typename U, U (downtime::* member)>
    void         set_downtime_member(std::string const& val);

    misc::shared_ptr<downtime>
                 get_downtime() const;

    virtual void init_bindings();

  private:
    misc::shared_ptr<downtime>
                _downtime;
  };

  // Stream operators for QString.
  std::istream& operator>>(std::istream& stream, QString& fake_str);
  std::ostream& operator<<(std::ostream& stream, QString const& fake_str);
}

CCB_END()

#endif // !CCB_NEB_TIMEPERIOD_SERIALIZABLE_HH
