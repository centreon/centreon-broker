/*
** Copyright 2009-2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_NEB_TIMEPERIOD_SERIALIZABLE_HH
#  define CCB_NEB_TIMEPERIOD_SERIALIZABLE_HH

#  include <string>
#  include <unordered_map>
#  include <string>
#  include "com/centreon/broker/time/timeperiod.hh"
#  include "com/centreon/broker/ceof/ceof_serializable.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace   neb {
  /**
   *  @class timeperiod_serializable timeperiod_serializable.hh "com/centreon/broker/neb/timeperiod_serializable.hh"
   *  @brief Represent a serializable Centreon Engine Object File timeperiod.
   */
  class          timeperiod_serializable
                   : public ceof::ceof_serializable {
  public:
                 timeperiod_serializable(
                   std::unordered_map<std::string, time::timeperiod::ptr> const& tps);
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
    std::string  get_exceptions() const;
    std::string  get_excluded() const;
    std::string  get_included() const;

    void         set_name(std::string const& name);
    void         set_alias(std::string const& val);
    void         set_sunday(std::string const& val);
    void         set_monday(std::string const& val);
    void         set_tuesday(std::string const& val);
    void         set_wednesday(std::string const& val);
    void         set_thursday(std::string const& val);
    void         set_friday(std::string const& val);
    void         set_saturday(std::string const& val);
    void         set_exceptions(std::string const& val);
    void         set_excluded(std::string const& val);
    void         set_included(std::string const& val);

    time::timeperiod::ptr
                 get_timeperiod() const;

    virtual void visit(ceof::ceof_visitor& visitor);

  private:
    std::unordered_map<std::string, time::timeperiod::ptr> const*
                 _tps;
    time::timeperiod::ptr
                 _tp;
  };
}

CCB_END()

#endif // !CCB_NEB_TIMEPERIOD_SERIALIZABLE_HH
