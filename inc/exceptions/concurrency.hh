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

#ifndef EXCEPTIONS_CONCURRENCY_HH_
# define EXCEPTIONS_CONCURRENCY_HH_

# include <pthread.h>
# include "exceptions/retval.hh"

namespace        exceptions {
  /**
   *  @class concurrency concurrency.hh "exceptions/concurrency.hh"
   *  @brief Concurrency related exception.
   *
   *  This kind of exception is thrown by concurrency classes (mutex,
   *  thread, ...) and embed the originating thread ID.
   */
  class          concurrency : public retval {
   private:
    pthread_t    _thread_id;

   public:
                 concurrency(int r = 0, pthread_t id = pthread_self());
                 concurrency(concurrency const& c) throw ();
    virtual      ~concurrency() throw ();
    concurrency& operator=(concurrency const& c) throw ();
    concurrency& operator<<(bool b) throw ();
    concurrency& operator<<(double d) throw ();
    concurrency& operator<<(int i) throw ();
    concurrency& operator<<(unsigned int i) throw ();
    concurrency& operator<<(char const* str) throw ();
    concurrency& operator<<(time_t t) throw ();
    pthread_t    get_thread_id() const throw ();
  };
}

#endif /* !EXCEPTIONS_CONCURRENCY_HH_ */
