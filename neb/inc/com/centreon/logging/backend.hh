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

#ifndef CC_LOGGING_BACKEND_HH
#  define CC_LOGGING_BACKEND_HH

#  include "com/centreon/concurrency/mutex.hh"
#  include "com/centreon/namespace.hh"

CC_BEGIN()

namespace misc {
  class   stringifier;
}

namespace                  logging {
  enum                     time_precision {
    none = 0,
    microsecond = 1,
    millisecond = 2,
    second = 3
  };

  /**
   *  @class backend backend.hh "com/centreon/logging/backend.hh"
   *  @brief Base logging backend class.
   *
   *  This class defines an interface to create logger backend, to
   *  log data into many different objects.
   */
  class                    backend {
  public:
                           backend(
                             bool is_sync = true,
                             bool show_pid = true,
                             time_precision show_timestamp = second,
                             bool show_thread_id = false);
                           backend(backend const& right);
    virtual                ~backend() throw ();
    backend&               operator=(backend const& right);
    virtual void           close() throw () = 0;
    virtual bool           enable_sync() const ;
    virtual void           enable_sync(bool enable);
    virtual void           log(
                             unsigned long long types,
                             unsigned int verbose,
                             char const* msg) throw ();
    virtual void           log(
                             unsigned long long types,
                             unsigned int verbose,
                             char const* msg,
                             unsigned int size) throw () = 0;
    virtual void           open() = 0;
    virtual void           reopen() = 0;
    virtual bool           show_pid() const;
    virtual void           show_pid(bool enable);
    virtual time_precision show_timestamp() const;
    virtual void           show_timestamp(time_precision val);
    virtual bool           show_thread_id() const ;
    virtual void           show_thread_id(bool enable);

  protected:
    void                   _build_header(misc::stringifier& buffer);

    bool                   _is_sync;
    mutable concurrency::mutex
                           _lock;
    bool                   _show_pid;
    time_precision         _show_timestamp;
    bool                   _show_thread_id;

  protected:
    void                   _internal_copy(backend const& right);
  };
}

CC_END()

#endif // !CC_LOGGING_BACKEND_HH
