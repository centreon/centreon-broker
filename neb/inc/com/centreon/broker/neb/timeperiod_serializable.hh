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

#ifndef CCB_NEB_TIMEPERIOD_SERIALIZABLE_HH
#  define CCB_NEB_TIMEPERIOD_SERIALIZABLE_HH

#  include <string>
#  include "com/centreon/broker/time/timeperiod.hh"
#  include "com/centreon/broker/neb/ceof_serializable.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace   neb {
  /**
   *  @class timeperiod_serializable timeperiod_serializable.hh "com/centreon/broker/neb/timeperiod_serializable.hh"
   *  @brief Represent a serializable Centreon Engine Object File timeperiod.
   */
  class          timeperiod_serializable
                   : public ceof_serializable<timeperiod_serializable> {
  public:
                 timeperiod_serializable();
                 timeperiod_serializable(
                 timeperiod_serializable const& other);
    timeperiod_serializable&
                 operator=(timeperiod_serializable const& other);
    virtual      ~timeperiod_serializable();

    std::string  get_name() const;
    std::string  get_alias() const;
    std::string  get_sunday() const;
    std::string  get_monday() const;
    std::string  get_tuesday() const;
    std::string  get_wednesday() const;
    std::string  get_thursday() const;
    std::string  get_friday() const;
    std::string  get_saturday() const;

    void         set_name(std::string const& name);
    void         set_alias(std::string const& val);
    void         set_sunday(std::string const& val);
    void         set_monday(std::string const& val);
    void         set_tuesday(std::string const& val);
    void         set_wednesday(std::string const& val);
    void         set_thursday(std::string const& val);
    void         set_friday(std::string const& val);
    void         set_saturday(std::string const& val);

    time::timeperiod::ptr
                 get_timeperiod() const;

    virtual void init_bindings();

  private:
    time::timeperiod::ptr
                 _tp;  };
}

CCB_END()

#endif // !CCB_NEB_TIMEPERIOD_SERIALIZABLE_HH
