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

#ifndef CC_PROCESS_WIN32_HH
#  define CC_PROCESS_WIN32_HH

#  include <string>
#  include <windows.h>
#  include "com/centreon/namespace.hh"
#  include "com/centreon/timestamp.hh"

CC_BEGIN()

class process_listener;
class process_manager;

/**
 *  @class process process_win32.hh "com/centreon/process_win32.hh"
 *  @brief Process execution class.
 *
 *  Execute external process.
 */
class                  process {
  friend class         process_manager;
public:
  enum                 status {
    normal = 0,
    crash = 1,
    timeout = 2
  };
  enum                 stream {
    in = 0,
    out = 1,
    err = 2
  };

                       process(process_listener* l = NULL);
  virtual              ~process() throw ();
  void                 enable_stream(stream s, bool enable);
  timestamp const&     end_time() const throw ();
  void                 exec(
                         char const* cmd,
                         char** env = NULL,
                         unsigned int timeout = 0);
  void                 exec(
                         std::string const& cmd,
                         unsigned int timeout = 0);
  int                  exit_code() const throw ();
  status               exit_status() const throw ();
  void                 kill();
  unsigned int         read(void* data, unsigned int size);
  unsigned int         read_err(void* data, unsigned int size);
  timestamp const&     start_time() const throw ();
  void                 terminate();
  void                 wait();
  bool                 wait(unsigned long timeout);
  unsigned int         write(std::string const& data);
  unsigned int         write(void const* data, unsigned int size);

private:
                       process(process const& p);
  process&             operator=(process const& p);
  void                 _close(HANDLE& fd) throw ();
  static void          _pipe(HANDLE* rh, HANDLE* wh);
  unsigned int         _read(HANDLE h, void* data, unsigned int size);
  static BOOL          _terminate_window(HWND hwnd, LPARAM proc_id);
  bool                 _wait(DWORD timeout, int* exit_code);

  std::string          _buffer_err;
  std::string          _buffer_out;
  bool                 _enable_stream[3];
  timestamp            _end_time;
  DWORD                _exit_code;
  bool                 _is_timeout;
  process_listener*    _listener;
  mutable concurrency::mutex
                       _lock_process;
  PROCESS_INFORMATION* _process;
  timestamp            _start_time;
  HANDLE               _stream[3];
  unsigned int         _timeout;
};

CC_END()

#endif // !CC_PROCESS_WIN32_HH
