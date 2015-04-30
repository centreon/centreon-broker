/*
** Copyright 2012-2013 Merethis
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

#ifndef CC_PROCESS_POSIX_HH
#  define CC_PROCESS_POSIX_HH

#  include <string>
#  include <sys/types.h>
#  include "com/centreon/concurrency/condvar.hh"
#  include "com/centreon/concurrency/mutex.hh"
#  include "com/centreon/namespace.hh"
#  include "com/centreon/timestamp.hh"

CC_BEGIN()

class process_listener;
class process_manager;

/**
 *  @class process process_posix.hh "com/centreon/process_posix.hh"
 *  @brief Process execution class.
 *
 *  Execute external process.
 */
class                process {
  friend class       process_manager;
public:
  enum               status {
    normal = 0,
    crash = 1,
    timeout = 2
  };
  enum               stream {
    in = 0,
    out = 1,
    err = 2
  };

                     process(process_listener* l = NULL);
  virtual            ~process() throw ();
  void               enable_stream(stream s, bool enable);
  timestamp const&   end_time() const throw ();
  void               exec(
                       char const* cmd,
                       char** env = NULL,
                       unsigned int timeout = 0);
  void               exec(
                       std::string const& cmd,
                       unsigned int timeout = 0);
  int                exit_code() const throw ();
  status             exit_status() const throw ();
  void               kill();
  void               read(std::string& data);
  void               read_err(std::string& data);
  void               setpgid_on_exec(bool enable) throw ();
  bool               setpgid_on_exec() const throw ();
  timestamp const&   start_time() const throw ();
  void               terminate();
  void               wait() const;
  bool               wait(unsigned long timeout) const;
  unsigned int       write(std::string const& data);
  unsigned int       write(void const* data, unsigned int size);

private:
                     process(process const& p);
  process&           operator=(process const& p);
  static void        _close(int& fd) throw ();
  static pid_t       _create_process_with_setpgid(
                       char** args,
                       char** env);
  static pid_t       _create_process_without_setpgid(
                       char** args,
                       char** env);
  static void        _dev_null(int fd, int flags);
  static int         _dup(int oldfd);
  static void        _dup2(int oldfd, int newfd);
  bool               _is_running() const throw ();
  void               _kill(int sig);
  static void        _pipe(int fds[2]);
  unsigned int       _read(int fd, void* data, unsigned int size);
  static void        _set_cloexec(int fd);

  std::string        _buffer_err;
  std::string        _buffer_out;
  pid_t              (*_create_process)(char**, char**);
  mutable concurrency::condvar
                     _cv_buffer_err;
  mutable concurrency::condvar
                     _cv_buffer_out;
  mutable concurrency::condvar
                     _cv_process;
  bool               _enable_stream[3];
  timestamp          _end_time;
  bool               _is_timeout;
  process_listener*  _listener;
  mutable concurrency::mutex
                     _lock_process;
  pid_t              _process;
  timestamp          _start_time;
  int                _status;
  int                _stream[3];
  unsigned int       _timeout;
};

CC_END()

#endif // !CC_PROCESS_POSIX_HH
