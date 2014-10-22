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

#ifndef BAM_CCB_CONFIGURATION_TIMEPERIOD_HH
#  define BAM_CCB_CONFIGURATION_TIMEPERIOD_HH

#  include <utility>
#  include <vector>
#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                   bam {
  namespace                 configuration {
    /**
     *  @class timeperiod timeperiod.hh  "com/centreon/broker/bam/configuration/timeperiod.h"
     *  @brief Store a timeperiod from the database.
     */
    class                   timeperiod {
    public:
                            timeperiod();
                            timeperiod(
                                  unsigned int id,
                                  std::string const& name,
                                  std::string const& alias,
                                  std::string const& sunday,
                                  std::string const& monday,
                                  std::string const& tuesday,
                                  std::string const& wednesday,
                                  std::string const& thursday,
                                  std::string const& friday,
                                  std::string const& saturday);
                            timeperiod(timeperiod const& other);
      timeperiod&           operator=(timeperiod const& other);
      bool                  operator==(timeperiod const& other) const;

      void                  add_exception(std::string const& day,
                                          std::string const& timerange);
      void                  add_include_relation(unsigned int included_tp_id);
      void                  add_exclude_relation(unsigned int excluded_tp_id);

      unsigned int          get_id() const;
      std::string const&    get_name() const;
      std::string const&    get_alias() const;
      std::string const&    get_sunday() const;
      std::string const&    get_monday() const;
      std::string const&    get_tuesday() const;
      std::string const&    get_wednesday() const;
      std::string const&    get_thursday() const;
      std::string const&    get_friday() const;
      std::string const&    get_saturday() const;
      std::vector<std::pair<std::string,
                            std::string> > const&
                            get_exceptions() const;
      std::vector<unsigned int> const&
                            get_include() const;
      std::vector<unsigned int> const&
                            get_exclude() const;

    private:
      unsigned int          _id;
      std::string           _name;
      std::string           _alias;
      std::string           _sunday;
      std::string           _monday;
      std::string           _tuesday;
      std::string           _wednesday;
      std::string           _thursday;
      std::string           _friday;
      std::string           _saturday;
      std::vector<std::pair<std::string,
                            std::string> >
                            _exceptions;
      std::vector<unsigned int>
                            _include;
      std::vector<unsigned int>
                            _exclude;
    };
  }
}

CCB_END()

#endif // !BAM_CCB_CONFIGURATION_TIMEPERIOD_HH
