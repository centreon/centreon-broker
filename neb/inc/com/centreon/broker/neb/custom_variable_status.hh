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

#ifndef CCB_NEB_CUSTOM_VARIABLE_STATUS_HH
#  define CCB_NEB_CUSTOM_VARIABLE_STATUS_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace                   neb {
  /**
   *  @class custom_variable_status custom_variable_status.hh "com/centreon/broker/neb/custom_variable_status.hh"
   *  @brief Custom variable update.
   *
   *  The value of a custom variable has changed.
   */
  class                     custom_variable_status : public io::data {
  public:
                            custom_variable_status();
                            custom_variable_status(
                              custom_variable_status const& cvs);
    virtual                 ~custom_variable_status();
    custom_variable_status& operator=(custom_variable_status const& cvs);
    virtual unsigned int    type() const;

    unsigned int            host_id;
    bool                    modified;
    QString                 name;
    unsigned int            service_id;
    timestamp               update_time;
    QString                 value;

  private:
    void                    _internal_copy(custom_variable_status const& cvs);
  };
}

CCB_END()

#endif // !CCB_NEB_CUSTOM_VARIABLE_STATUS_HH
