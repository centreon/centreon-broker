/*
** Copyright 2012-2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
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
