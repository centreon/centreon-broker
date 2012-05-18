/*
** Copyright 2012 Merethis
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

#ifndef CCB_NEB_CALLBACK_HH
#  define CCB_NEB_CALLBACK_HH

#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace     neb {
  /**
   *  @class callback callback.hh "com/centreon/broker/neb/callback.hh"
   *  @brief Manager NEB callbacks.
   *
   *  Handle callback registration/deregistration with Nagios.
   */
  class       callback {
  public:
              callback(
                int id,
                void* handle,
                int (* function)(int, void*));
              ~callback() throw ();

  private:
              callback(callback const& right);
    callback& operator=(callback const& right);
    void      _internal_copy(callback const& right);

    int (*    _function)(int, void*);
    int       _id;
  };
}

CCB_END()

#endif /* !CCB_NEB_CALLBACK_HH */
