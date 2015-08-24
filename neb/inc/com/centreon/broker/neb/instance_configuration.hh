/*
** Copyright 2009-2012 Merethis
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

#ifndef CCB_NEB_INSTANCE_CONFIGURATION_HH
#  define CCB_NEB_INSTANCE_CONFIGURATION_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace          neb {
  /**
   *  @class instance_configuration instance_configuration.hh "com/centreon/broker/neb/instance_configuration.hh"
   *  @brief Information about the instance configuration.
   *
   *  An event of this class is sent when centreon engine has finished
   *  reloading its configuration, either successfully or not.
   */
  class            instance_configuration : public io::data {
  public:
                   instance_configuration();
                   instance_configuration(instance_configuration const& i);
                   ~instance_configuration();
    instance_configuration&
                   operator=(instance_configuration const& i);
    unsigned int   type() const;

    bool           loaded;

  private:
    void           _internal_copy(instance_configuration const& i);
  };
}

CCB_END()

#endif // !CCB_NEB_INSTANCE_CONFIGURATION_HH
