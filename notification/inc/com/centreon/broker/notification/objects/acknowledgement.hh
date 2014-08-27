/*
** Copyright 2011-2013 Merethis
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

#ifndef CCB_NOTIFICATION_ACKNOWLEDGMENT_HH
#  define CCB_NOTIFICATION_ACKNOWLEDGMENT_HH

#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/utilities/ptr_typedef.hh"

CCB_BEGIN()

namespace       notification {

  class acknowledgement {
  public:
    DECLARE_SHARED_PTR(acknowledgement);

    enum type {
      unknown = 0,
      host = 1,
      service = 2
    };
                  acknowledgement();
                  acknowledgement(acknowledgement const& obj);
                  acknowledgement& operator=(acknowledgement const& obj);

    type          get_type() const throw();
    void          set_type(type val) throw();
    int           get_acknowledgement_type() const throw();
    void          set_acknowledgement_type(int val) throw();
    unsigned int  get_host_id() const throw();
    void          set_host_id(unsigned int val) throw();
    unsigned int  get_service_id() const throw();
    void          set_service_id(unsigned int val) throw();

  private:
    type          _type;
    int           _acknowledgement_type;
    unsigned int  _host_id;
    unsigned int  _service_id;
  };

}

CCB_END()

#endif //!CCB_NOTIFICATION_ACKNOWLEDGMENT_HH
