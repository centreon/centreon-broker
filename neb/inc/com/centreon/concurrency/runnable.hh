/*
** Copyright 2011-2013 Merethis
**
** This file is part of Centreon Clib.
**
** Centreon Clib is free software: you can redistribute it
** and/or modify it under the terms of the GNU Affero General Public
** License as published by the Free Software Foundation, either version
** 3 of the License, or (at your option) any later version.
**
** Centreon Clib is distributed in the hope that it will be
** useful, but WITHOUT ANY WARRANTY; without even the implied warranty
** of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** Affero General Public License for more details.
**
** You should have received a copy of the GNU Affero General Public
** License along with Centreon Clib. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CC_CONCURRENCY_RUNNABLE_HH
#  define CC_CONCURRENCY_RUNNABLE_HH

#  include "com/centreon/namespace.hh"

CC_BEGIN()

namespace        concurrency {
  /**
   *  @class runnable runnable.hh "com/centreon/concurrency/runnable.hh"
   *  @brief Base for all runnable objects.
   *
   *  This class is an interface for piece of code to needs to be
   *  executed.
   */
  class          runnable {
  public:
                 runnable();
                 runnable(runnable const& right);
    virtual      ~runnable() throw ();
    runnable&    operator=(runnable const& right);
    bool         get_auto_delete() const throw ();
    virtual void run() = 0;
    void         set_auto_delete(bool auto_delete) throw ();

  private:
    runnable&    _internal_copy(runnable const& right);

    bool         _auto_delete;
  };
}

CC_END()

#endif // !CC_CONCURRENCY_RUNNABLE_HH
