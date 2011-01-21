/*
** Copyright 2009-2011 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#ifndef EVENTS_MODULE_HH_
# define EVENTS_MODULE_HH_

# include <string>
# include "events/event.hh"

namespace       events {
  /**
   *  @class module module.hh "events/module.hh"
   *  @brief Represents a module loaded in a Nagios instance.
   *
   *  Nagios supports modules that extend its original capabilities.
   *  This class represents such modules.
   */
  class         module : public event {
  private:
    void        _internal_copy(module const& m);

  public:
    std::string args;
    std::string filename;
    int         instance_id;
    bool        loaded;
    bool        should_be_loaded;
                module();
                module(module const& m);
                ~module();
    module&     operator=(module const& m);
    int         get_type() const;
  };
}

#endif /* !EVENTS_MODULE_HH_ */
