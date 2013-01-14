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

#ifndef CCB_EXCEPTIONS_WITH_POINTER_HH
#  define CCB_EXCEPTIONS_WITH_POINTER_HH

#  include <memory>
#  include "com/centreon/broker/exceptions/msg.hh"
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                      exceptions {
  /**
   *  @class with_pointer with_pointer.hh "com/centreon/broker/exceptions/with_pointer.hh"
   *  @brief Exception with data pointer attached.
   *
   *  This class is used to provide exception chaining when it is
   *  necessary to attach a data pointer to the exception.
   */
  class                        with_pointer : public msg {
  public:
                               with_pointer(
                                 msg const& base,
                                 misc::shared_ptr<io::data> ptr);
                               with_pointer(with_pointer const& wp);
    virtual                    ~with_pointer() throw ();
    with_pointer&              operator=(with_pointer const& wp);
    virtual msg*               clone() const;
    misc::shared_ptr<io::data> ptr() const;
    virtual void               rethrow() const;
    char const*                what() const throw ();

  private:
    mutable std::auto_ptr<msg> _base;
    mutable misc::shared_ptr<io::data>
                               _ptr;
  };
}

CCB_END()

#endif // !CCB_EXCEPTIONS_WITH_POINTER_HH
