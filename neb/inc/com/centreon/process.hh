/*
** Copyright 2012-2013,2019-2021 Centreon
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

#ifndef CC_PROCESS_POSIX_HH
#define CC_PROCESS_POSIX_HH

#include <sys/types.h>
#include <condition_variable>
#include <mutex>
#include <string>
#include "com/centreon/timestamp.hh"

CC_BEGIN()

class process_listener;
class process_manager;

/**
 *  @class process process_posix.hh "com/centreon/process_posix.hh"
 *  @brief Process execution class.
 *
 *  Execute external process.
 */
class process {
  friend class process_manager;

 public:
  enum status { normal = 0, crash = 1, timeout = 2 };
  enum stream { in = 0, out = 1, err = 2 };

 private:
  std::string _buffer_err;
  std::string _buffer_out;
  pid_t (*_create_process)(char**, char**);
  mutable std::condition_variable _cv_buffer_err;
  mutable std::condition_variable _cv_buffer_out;
  mutable std::condition_variable _cv_process_running;
  const std::array<bool, 3> _enable_stream;
  std::array<int, 3> _stream;
  timestamp _end_time;
  bool _is_timeout;
  process_listener* _listener;
  mutable std::mutex _lock_process;
  pid_t _process;
  timestamp _start_time;
  int _status;
  uint32_t _timeout;

  static void _close(int& fd) noexcept;
  static pid_t _create_process_with_setpgid(char** args, char** env);
  static pid_t _create_process_without_setpgid(char** args, char** env);
  static void _dev_null(int fd, int flags);
  static int _dup(int oldfd);
  static void _dup2(int oldfd, int newfd);
  bool _is_running() const noexcept;
  void _kill(int sig);
  static void _pipe(int fds[2]);
  ssize_t do_read(int fd);
  void do_close(int fd);
  static void _set_cloexec(int fd);

 public:
  process(process_listener* l = nullptr,
          bool in_stream = true,
          bool out_stream = true,
          bool err_stream = true);
  virtual ~process() noexcept;
  process(const process&) = delete;
  process& operator=(const process&) = delete;
  // void enable_stream(stream s, bool enable);
  timestamp const& end_time() const noexcept;
  void exec(char const* cmd, char** env = nullptr, uint32_t timeout = 0);
  void exec(std::string const& cmd, uint32_t timeout = 0);
  int exit_code() const noexcept;
  status exit_status() const noexcept;
  void kill();
  void read(std::string& data);
  void read_err(std::string& data);
  void setpgid_on_exec(bool enable) noexcept;
  bool setpgid_on_exec() const noexcept;
  timestamp const& start_time() const noexcept;
  void terminate();
  void wait() const;
  bool wait(uint32_t timeout) const;
  void update_ending_process(int status);
  uint32_t write(std::string const& data);
  uint32_t write(void const* data, uint32_t size);
};

CC_END()

#endif  // !CC_PROCESS_POSIX_HH
