/*
** Copyright 2011 Merethis
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

#ifndef CCB_EXCEPTIONS_WITH_POINTER_HH_
# define CCB_EXCEPTIONS_WITH_POINTER_HH_

# include <QSharedPointer>
# include "com/centreon/broker/exceptions/msg.hh"
# include "com/centreon/broker/io/data.hh"

namespace                          com {
  namespace                        centreon {
    namespace                      broker {
      namespace                    exceptions {
        /**
         *  @class with_pointer with_pointer.hh "com/centreon/broker/exceptions/with_pointer.hh"
         *  @brief Exception with data pointer attached.
         *
         *  This class is used to provide exception chaining when it is
         *  necessary to attach a data pointer to the exception.
         */
        class                      with_pointer : public msg {
         private:
          mutable QScopedPointer<msg>
                                   _base;
          mutable QSharedPointer<io::data>
                                   _ptr;

         public:
                                   with_pointer(msg const& base,
                                     QSharedPointer<io::data> ptr);
                                   with_pointer(with_pointer const& wp);
          virtual                  ~with_pointer() throw ();
          with_pointer&            operator=(with_pointer const& wp);
          virtual msg*             clone() const;
          QSharedPointer<io::data> ptr() const;
          virtual void             rethrow();
          char const*              what() const throw ();
        };
      }
    }
  }
}

#endif /* !CCB_EXCEPTIONS_WITH_POINTER_HH_ */
